#include "tier0/crashdump.h"
#include "tier0/icommandline.h"
#include "tier1/filetools.h"
#include "utils/interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "cvar/icvar.h"
#include "appframework/appframework.h"
#include "tools/ecscompiler/ecsfileparser.h"
#include "tools/ecscompiler/ecscppgenerator.h"

//-----------------------------------------------------------------------------
// ECS compiler app system group
//-----------------------------------------------------------------------------
class CEcsCompilerAppSystemGroup : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// IAppSystemGroup interface
	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

private:
	void PrintUsageHelp();
	bool ParseEcsFiles( const eastl::string& dir, const eastl::string rootDir, CEcsSystemStub& stubs );
	bool ParseEcsFile( const char* pPath, CEcsSystemStub& stubs );
	bool GenerateCppFiles( const char* pRootDir, const char* pOutputDir, const CEcsSystemStub& stubs, ecsCppFileType_t cppFileType );
};

/*
==================
CEcsCompilerAppSystemGroup::Main
==================
*/
int32 CEcsCompilerAppSystemGroup::Main()
{
	// Is need to print help of usage
	bool bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get a path to source ECS file or directory
	const char* pEcsFile			= CommandLine()->GetFirstValue( "file" );
	const char* pEcsDir				= CommandLine()->GetFirstValue( "dir" );
	bool		bInvalidEcsFilePath = !pEcsFile || pEcsFile[0] == '\0';
	bool		bInvalidEcsDir		= !pEcsDir || pEcsDir[0] == '\0';

	// Get an output directory for C++ file
	const char* pCppFileDir		   = CommandLine()->GetFirstValue( "output" );
	bool		bInvalidCppFileDir = !pCppFileDir || pCppFileDir[0] == '\0';

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || ( bInvalidEcsFilePath && bInvalidEcsDir ) || bInvalidCppFileDir )
	{
		PrintUsageHelp();
		return 0;
	}

	// Make sure that set only one flag
	if ( !( bInvalidEcsFilePath ^ bInvalidEcsDir ) )
	{
		Error( "EcsCompiler: Must be set only 'file' or 'dir'" );
		return 1;
	}

	// Parse the ECS file if set 'file'
	CEcsSystemStub ecsSystemStub;
	if ( bInvalidEcsDir )
	{
		Assert( !bInvalidEcsFilePath );
		if ( !ParseEcsFile( pEcsFile, ecsSystemStub ) )
		{
			return 2;
		}
	}
	// Otherwise parse whole directory
	else
	{
		if ( !ParseEcsFiles( pEcsDir, pEcsDir, ecsSystemStub ) )
		{
			return 2;
		}
	}

	// Generate C++ headers
	if ( !GenerateCppFiles( !bInvalidEcsDir ? pEcsDir : "", pCppFileDir, ecsSystemStub, ECS_CPP_FILE_TYPE_HEADER ) )
	{
		return 3;
	}

	// Generate C++ sources
	if ( !GenerateCppFiles( !bInvalidEcsDir ? pEcsDir : "", pCppFileDir, ecsSystemStub, ECS_CPP_FILE_TYPE_SOURCE ) )
	{
		return 3;
	}

	return 0;
}

/*
==================
CEcsCompilerAppSystemGroup::ParseEcsFiles
==================
*/
bool CEcsCompilerAppSystemGroup::ParseEcsFiles( const eastl::string& dir, const eastl::string rootDir, CEcsSystemStub& stubs )
{
	bool					  bResult = true;
	TRefPtr<IPathArrayResult> pFiles  = g_pFileSystem->FindFiles( dir.c_str(), true, true );
	for ( uint32 fileIdx = 0, numFiles = pFiles->GetNum(); fileIdx < numFiles; ++fileIdx )
	{
		// If the file is a directory look in
		const char* pPath = pFiles->GetPath( fileIdx );
		if ( g_pFileSystem->IsFileDirectory( pPath ) )
		{
			bResult &= ParseEcsFiles( S_Sprintf( "%s/", pPath ), rootDir, stubs );
			continue;
		}

		// If the file has 'ecs' extension its our ECS file and parse it
		const char* pFileExtension = S_GetFileExtension( pPath );
		if ( pFileExtension && !S_Stricmp( pFileExtension, "ecs" ) )
		{
			bResult &= ParseEcsFile( pPath, stubs );
		}
	}

	return bResult;
}

/*
==================
CEcsCompilerAppSystemGroup::ParseEcsFile
==================
*/
bool CEcsCompilerAppSystemGroup::ParseEcsFile( const char* pPath, CEcsSystemStub& stubs )
{
	// Read whole the file
	TRefPtr<IStreamDataReader> pFileReader = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFileReader )
	{
		Error( "EcsCompiler: Failed to open ECS file '%s'", pPath );
		return false;
	}

	eastl::string buffer;
	buffer.resize( pFileReader->GetSize() );
	pFileReader->Read( buffer.data(), buffer.size() );
	pFileReader = NULL;

	// Parse the ECS file
	CEcsFileParser ecsFileParser( stubs );
	if ( !ecsFileParser.ParseFile( pPath, buffer.c_str() ) )
	{
		Error( "EcsCompiler: Failed to parse ECS file '%s'", pPath );
		return false;
	}
	return true;
}

/*
==================
CEcsCompilerAppSystemGroup::GenerateCppFiles
==================
*/
bool CEcsCompilerAppSystemGroup::GenerateCppFiles( const char* pRootDir, const char* pOutputDir, const CEcsSystemStub& stubs, ecsCppFileType_t cppFileType )
{
	const eastl::vector<TRefPtr<CEcsStubModule>>& ecsStubModules = stubs.GetModules();
	CEcsCppGenerator							  ecsCppGenerator;

	// Get file extension and type name
	const char* pCppFileExtension;
	const char* pCppFileTypeName;
	switch ( cppFileType )
	{
	case ECS_CPP_FILE_TYPE_HEADER:
		pCppFileExtension = "h";
		pCppFileTypeName  = "header";
		break;

	case ECS_CPP_FILE_TYPE_SOURCE:
		pCppFileExtension = "cpp";
		pCppFileTypeName  = "source";
		break;

	default:
		AssertMsg( false, "Unknown C++ file type 0x%X", cppFileType );
		break;
	}

	// Convert the root and the output directory into absolute path
	eastl::string rootDir	= pRootDir;
	eastl::string outputDir = pOutputDir;
	if ( !S_IsAbsolutePath( pRootDir ) )
	{
		S_MakeAbsolutePath( pRootDir, rootDir );
	}
	if ( !S_IsAbsolutePath( pOutputDir ) )
	{
		S_MakeAbsolutePath( pOutputDir, outputDir );
	}

	for ( uint32 ecsStubModuleIdx = 0, numEcsStubModules = (uint32)ecsStubModules.size(); ecsStubModuleIdx < numEcsStubModules; ++ecsStubModuleIdx )
	{
		// Generate C++ header for this module
		CEcsStubModule* pEcsStubModule = ecsStubModules[ecsStubModuleIdx];
		ecsCppGenerator.Reset();
		ecsCppGenerator.Generate( pEcsStubModule, cppFileType );
		if ( ecsCppGenerator.HasError() )
		{
			return false;
		}
		const eastl::string& buffer = ecsCppGenerator.GetBuffer();

		// Make sub directories if we use 'dir' command
		eastl::string subDir;
		if ( !rootDir.empty() )
		{
			S_GetFilePath( pEcsStubModule->GetContext().file.AsChar() + rootDir.size(), subDir, false );
			S_AppendPathSeparator( subDir );
		}

		// Generate file path from the module name
		eastl::string filePath;
		{
			eastl::string moduleNameLower = pEcsStubModule->GetName();
			S_Strlwr( moduleNameLower.data() );
			filePath = S_Sprintf( "%s/%secs_%s.gen.%s", outputDir.c_str(), subDir.c_str(), moduleNameLower.c_str(), pCppFileExtension );
		}

		// Save buffer into the file
		TRefPtr<IStreamDataWriter> pFileWriter = g_pFileSystem->CreateFileWriter( filePath.c_str() );
		if ( pFileWriter )
		{
			pFileWriter->Write( (void*)buffer.data(), buffer.size() * sizeof( char ) );
			Msg( "EcsCompiler: C++ %s for '%s' saved to '%s'", pCppFileTypeName, pEcsStubModule->GetName(), filePath.c_str() );
		}
		else
		{
			Error( "EcsCompiler: Failed to save C++ %s for '%s' to '%s'", pCppFileTypeName, pEcsStubModule->GetName(), filePath.c_str() );
			return false;
		}
	}

	return true;
}

/*
==================
CEcsCompilerAppSystemGroup::PrintUsageHelp
==================
*/
void CEcsCompilerAppSystemGroup::PrintUsageHelp()
{
	Msg( "" );
	Msg( "ECS compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage: ecscompiler -file <path> -output <path_dir>" );
	Msg( "Usage: ecscompiler -dir <path> -output <path_dir>" );
	Msg( "Ex: ecscompiler -file ../src/public/libs/gameframework/ecs/ecs_movement.ecs -output ../src/public/libs/gameframework/ecs/" );
	Msg( "Ex: ecscompiler -dir ../src/ -output ../src/intermediate/generated/" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "file\tSource ECS file to compile. For syntax example see 'src/public/libs/gameframework/ecs/*.ecs'" );
	Msg( "dir\tRecursively compile all ECS file in the directory" );
	Msg( "output\tSpecify an output directory for a C++ file" );
	Msg( "" );
}

/*
==================
main
==================
*/
int main( int argc, char** argv )
{
	// Enable developer messages if we in debug configuration
#if DEBUG
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, true );
#endif	// DEBUG

	// Initialize the main thread
	Sys_InitMainThread();

	// Initialize OS console
	LogConsoleOS()->Show( true );

	// Setup application information for the crash dump
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ "ECS Compiler", __DATE__ " " __TIME__, NULL, NULL } );

	// Initialize a command line
	{
		eastl::string arguments;
		for ( uint32 index = 0; index < (uint32)argc; ++index )
		{
			if ( *argv[index] == '-' || *argv[index] == '/' )
			{
				arguments += argv[index];
				arguments += " ";
			}
			else
			{
				arguments += S_Sprintf( "\"%s\" ", argv[index] );
			}
		}
		CommandLine()->Init( arguments.c_str() );
	}

	// Run the application
	CEcsCompilerAppSystemGroup ecsCompilerSystems;
	CApplication			   application( &ecsCompilerSystems, "ecscompiler" );
	return application.Run();
}
