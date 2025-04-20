#include "core/crashdump.h"
#include "core/icommandline.h"
#include "stdlib/filetools.h"
#include "interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "engine/icvar.h"
#include "appframework/iappsystemgroup.h"
#include "tools/ecscompiler/ecsfileparser.h"
#include "tools/ecscompiler/ecscppgenerator.h"

//-----------------------------------------------------------------------------
// ECS compiler application
//-----------------------------------------------------------------------------
class CEcsCompilerApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

private:
	void PrintUsageHelp();
	bool ParseEcsFile( const achar* pPath, CEcsSystemStub& stubs );
	bool GenerateCppHeaders( const achar* pOutputDir, const CEcsSystemStub& stubs );
};


/*
==================
CEcsCompilerApp::Create
==================
*/
bool CEcsCompilerApp::Create()
{
	// Load application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "engine"			DLL_EXT_STRING,			CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem"		DLL_EXT_STRING,			FILESYSTEM_INTERFACE_VERSION		},
		{ "engine"			DLL_EXT_STRING,			CVAR_INTERFACE_VERSION				},
		{ "", "" }																			// Required to terminate the list
	};

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// We are done
	return true;
}

/*
==================
CEcsCompilerApp::PreInit
==================
*/
bool CEcsCompilerApp::PreInit()
{
	return ConnectStdLib( GetFactory() );
}

/*
==================
CEcsCompilerApp::Main
==================
*/
int32 CEcsCompilerApp::Main()
{
	// Is need to print help of usage
	bool			bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get a path to source ECS file
	const achar*	pEcsFile = CommandLine()->GetFirstValue( "file" );
	bool			bInvalidEcsFilePath = !pEcsFile || pEcsFile[0] == '\0';

	// Get an output directory for C++ file
	const achar*	pCppFileDir = CommandLine()->GetFirstValue( "output" );
	bool			bInvalidCppFileDir = !pCppFileDir || pCppFileDir[0] == '\0';

	// Print help of usage if it need or some parameters aren't set 
	if ( bPrintHelpUsage || bInvalidEcsFilePath || bInvalidCppFileDir )
	{
		PrintUsageHelp();
		return 0;
	}

	// Parse the ECS file
	CEcsSystemStub		ecsSystemStub;
	if ( !ParseEcsFile( pEcsFile, ecsSystemStub ) )
	{
		return 1;
	}

	// Generate C++ headers
	if ( !GenerateCppHeaders( pCppFileDir, ecsSystemStub ) )
	{
		return 2;
	}

	return 0;
}

/*
==================
CEcsCompilerApp::PostShutdown
==================
*/
void CEcsCompilerApp::PostShutdown()
{
	DisconnectStdLib();
}

/*
==================
CEcsCompilerApp::ParseEcsFile
==================
*/
bool CEcsCompilerApp::ParseEcsFile( const achar* pPath, CEcsSystemStub& stubs )
{
	// Read whole the file
	TRefPtr<IStreamDataReader>	pFileReader = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFileReader )
	{
		Error( "EcsCompiler: Failed to open ECS file '%s'", pPath );
		return false;
	}

	std::string		buffer;
	buffer.resize( pFileReader->GetSize() );
	pFileReader->Read( buffer.data(), buffer.size() );
	pFileReader		= NULL;

	// Parse the ECS file
	CEcsFileParser		ecsFileParser( stubs );
	if ( !ecsFileParser.ParseFile( pPath, buffer.c_str() ) )
	{
		Error( "EcsCompiler: Failed to parse ECS file '%s'", pPath );
		return false;
	}
	return true;
}

/*
==================
CEcsCompilerApp::GenerateCppHeader
==================
*/
bool CEcsCompilerApp::GenerateCppHeaders( const achar* pOutputDir, const CEcsSystemStub& stubs )
{
	bool											bResult = true;
	const std::vector<TRefPtr<CEcsStubModule>>&		ecsStubModules = stubs.GetModules();
	CEcsCppGenerator								ecsCppGenerator;

	for ( uint32 ecsStubModuleIdx = 0, numEcsStubModules = ( uint32 )ecsStubModules.size(); ecsStubModuleIdx < numEcsStubModules; ++ecsStubModuleIdx )
	{
		// Generate C++ header for this module
		CEcsStubModule*			pEcsStubModule = ecsStubModules[ecsStubModuleIdx];
		ecsCppGenerator.Reset();
		ecsCppGenerator.Generate( pEcsStubModule );
		const std::string&		buffer = ecsCppGenerator.GetBuffer();

		// Generate file path from the module name
		std::string				filePath;
		{
			std::string		moduleNameLower = pEcsStubModule->GetName();
			S_Strlwr( moduleNameLower.data() );
			filePath		= S_Sprintf( "%s/ecs_%s.h", pOutputDir, moduleNameLower.c_str() );
		}

		// Save buffer into the file
		TRefPtr<IStreamDataWriter>		pFileWriter = g_pFileSystem->CreateFileWriter( filePath.c_str() );
		if ( pFileWriter )
		{
			pFileWriter->Write( ( void* )buffer.data(), buffer.size() * sizeof( achar ) );
			Msg( "EcsCompiler: C++ header for '%s' saved to '%s'", pEcsStubModule->GetName(), filePath.c_str() );
		}
		else
		{
			Error( "EcsCompiler: Failed to save C++ header for '%s' to '%s'", pEcsStubModule->GetName(), filePath.c_str() );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CEcsCompilerApp::PrintUsageHelp
==================
*/
void CEcsCompilerApp::PrintUsageHelp()
{
	Msg( "" );
	Msg( "ECS compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage ecscompiler -file <path> -output <path_dir>" );
	Msg( "Ex: ecscompiler -file ../src/public/libs/gameframework/ecs/ecs_movement.ecs -output ../src/public/libs/gameframework/ecs/" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "file\tSource ECS file to compile. For syntax ecample see 'src/public/libs/gameframework/ecs/*.ecs'" );
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
	// Initialize the main thread
	Sys_InitMainThread();

	// Setup application information for the crash dump
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ "ECS Compiler", __DATE__ " " __TIME__, NULL, NULL } );

	// Initialize a command line
	{
		std::string		arguments;
		for ( uint32 index = 0; index < ( uint32 )argc; ++index )
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
	CEcsCompilerApp		ecsCompilerApp;
	return ecsCompilerApp.Run();
}