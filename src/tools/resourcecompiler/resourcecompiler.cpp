#include "pch_resourcecompiler.h"
#include "tier0/icommandline.h"
#include "appframework/application.h"
#include "utils/gameinfo/gameinfo.h"
#include "utils/toolsinfo/toolsinfo.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

//-----------------------------------------------------------------------------
// Resource compiler app
//-----------------------------------------------------------------------------
class CResourceCompilerApp : public CApplication
{
	typedef CApplication BaseClass;

public:
	// CApplication interface
	virtual void  Init() override;
	virtual int32 Main() override;
	virtual void  Shutdown() override;

	virtual const appInfo_t&		  GetAppInfo() const override;
	virtual const crashDumpAppInfo_t& GetCrashDumpInfo() const override;

	CResourceCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance = NULL );

private:
	struct resourceCompiler_t
	{
		resourceCompiler_t()
			: handle( NULL )
			, pCompiler( NULL )
		{
		}

		dllHandle_t		   handle;
		IResourceCompiler* pCompiler;
	};

	struct resourceFile_t
	{
		eastl::string srcPath;
		eastl::string destPath;
	};

	void			   PrintUsageHelp();
	bool			   LoadFileList( const char* pPath );
	bool			   AddFileToCompile( const char* pPath, const char* pWorkDir );
	bool			   Compile( const resourceFile_t& resourceFile ) const;
	IResourceCompiler* FindResourceCompiler( const char* pSrcFormat ) const;

	bool LoadGameInfo();
	bool LoadToolsInfo();
	bool LoadResourceCompiler( const char* pPath );
	void UnloadGameInfo();
	void UnloadToolsInfo();
	void UnloadResourceCompiler( resourceCompiler_t& resourceCompiler );

	eastl::string																						 outputPath;
	eastl::string																						 gameDir;
	eastl::string																						 workDir;
	CGameInfoDoc																						 gameInfo;
	CToolsInfoDoc																						 toolsInfo;
	eastl::list<resourceFile_t>																			 files;
	eastl::vector<resourceCompiler_t>																	 resourceCompilers;
	eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> resourceCompilerDict;
};

/*
==================
CResourceCompilerApp::CResourceCompilerApp
==================
*/
CResourceCompilerApp::CResourceCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance /* = NULL */ )
	: CApplication( pCommandLine, hInstance )
{
}

/*
==================
CResourceCompilerApp::Init
==================
*/
void CResourceCompilerApp::Init()
{
	BaseClass::Init();
}

/*
==================
CResourceCompilerApp::Main
==================
*/
int32 CResourceCompilerApp::Main()
{
	// Is need to print help of usage
	bool bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get a path to a file or file list and a game directory
	const char* pFile			 = CommandLine()->GetFirstValue( "file" );
	const char* pFileList		 = CommandLine()->GetFirstValue( "filelist" );
	bool		bInvalidFile	 = !pFile || pFile[0] == '\0';
	bool		bInvalidFileList = !pFileList || pFileList[0] == '\0';

	// Get an game directory
	gameDir				 = CommandLine()->GetFirstValue( "game" );
	bool bInvalidGameDir = gameDir.empty();

	// Get an output directory
	outputPath = CommandLine()->GetFirstValue( "output" );
	if ( outputPath.empty() )
	{
		outputPath = gameDir;
	}
	bool bInvalidOutputPath = outputPath.empty();

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || ( bInvalidFile && bInvalidFileList ) || bInvalidGameDir || bInvalidOutputPath )
	{
		PrintUsageHelp();
		return 0;
	}

	// Make sure that set only one flag
	if ( !( bInvalidFile ^ bInvalidFileList ) )
	{
		Error( "ResourceCompiler: Must be set only 'file' or 'filelist'" );
		return 1;
	}

	// Get directory with the file list
	{
		eastl::string tmpBuffer;
		S_GetFilePath( bInvalidFileList ? pFile : pFileList, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, workDir, "" );
	}

	// Append a path separator to the output path and convert it into an absolute path
	S_AppendPathSeparator( outputPath );
	if ( !S_IsAbsolutePath( outputPath ) )
	{
		eastl::string tempBuffer = eastl::move( outputPath );
		S_MakeAbsolutePath( tempBuffer, outputPath );
	}

	// Append a path separator to the game directory path and convert it into an absolute path
	S_AppendPathSeparator( gameDir );
	if ( !S_IsAbsolutePath( gameDir ) )
	{
		eastl::string tempBuffer = eastl::move( gameDir );
		S_MakeAbsolutePath( tempBuffer, gameDir );
	}

	// Load a gameinfo.txt
	if ( !LoadGameInfo() )
	{
		Error( "ResourceCompiler: Failed to load %sgameinfo.txt", gameDir.c_str() );
		return 1;
	}

	// Load a toolsinfo.txt
	if ( !LoadToolsInfo() )
	{
		Error( "ResourceCompiler: Failed to load %stoolsinfo.txt", gameDir.c_str() );
		return 1;
	}

	// Add the file/filelist to the compile list
	if ( bInvalidFileList && !AddFileToCompile( pFile, workDir.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to add file '%s' to compile", pFile );
		return 1;
	}
	else if ( bInvalidFile && !LoadFileList( pFileList ) )
	{
		Error( "ResourceCompiler: Failed to load file list '%s'", pFileList );
		return 1;
	}

	// Compile each file by tools
	bool bResult = true;
	for ( auto it = files.begin(), itEnd = files.end(); it != itEnd; ++it )
	{
		const resourceFile_t& resourceFile = *it;
		Msg( "ResourceCompiler: Compiling '%s' ", resourceFile.srcPath.c_str() );
		bool bResourceCompiled = Compile( resourceFile );
		if ( !bResourceCompiled )
		{
			Error( "ResourceCompiler: Failed to compile '%s'", resourceFile.srcPath.c_str() );
			bResult = false;
		}
		else
		{
			Msg( "ResourceCompiler: Compiled '%s'", resourceFile.srcPath.c_str() );
		}
	}

	return bResult ? 0 : 1;
}

/*
==================
CResourceCompilerApp::Shutdown
==================
*/
void CResourceCompilerApp::Shutdown()
{
	UnloadToolsInfo();
	UnloadGameInfo();
	BaseClass::Shutdown();
}

/*
==================
CResourceCompilerApp::PrintUsageHelp
==================
*/
void CResourceCompilerApp::PrintUsageHelp()
{
	Msg( "" );
	Msg( "Resource compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage resourcecompiler -game <dir> -file <path>" );
	Msg( "Usage resourcecompiler -game <dir> -filelist <path>" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "game <dir>\tSpecifies the path to the directory with gameinfo.txt and toolsinfo.txt (which game to build for)" );
	Msg( "file <path>\tSpecify a JSON file containing settings to compile the resource" );
	Msg( "filelist <path>\tSpecify a JSON file containing a list of files to be processed as inputs. For syntax example see 'content/core/resourcelist.txt'" );
	Msg( "output <dir>\tOutput directory path. If this is not specified, it is derived from the game directory" );
	Msg( "" );
}

/*
==================
CResourceCompilerApp::LoadGameInfo
==================
*/
bool CResourceCompilerApp::LoadGameInfo()
{
	// Try load a gameinfo
	if ( !gameInfo.LoadFromFile( S_Sprintf( "%sgameinfo.txt", gameDir.c_str() ).c_str() ) )
	{
		return false;
	}

	// Print some information about the game
	Msg( "ResourceCompiler: Game '%s' (version: '%s')", gameInfo.GetGame().c_str(), gameInfo.GetVersion().c_str() );

	// Add search paths from the gameinfo
	const eastl::vector<gameInfoSearchPath_t>& searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t& searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}

	// We are done
	return true;
}

/*
==================
CResourceCompilerApp::LoadToolsInfo
==================
*/
bool CResourceCompilerApp::LoadToolsInfo()
{
	// Try load a toolsinfo
	if ( !toolsInfo.LoadFromFile( S_Sprintf( "%stoolsinfo.txt", gameDir.c_str() ).c_str() ) )
	{
		return false;
	}

	// Add search paths from the toolsinfo
	const eastl::vector<toolsInfoSearchPath_t>& searchPaths = toolsInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const toolsInfoSearchPath_t& searchPath = searchPaths[index];
		g_pFileSystem->AddSearchPath( searchPath.path.c_str(), searchPath.id.c_str() );
	}

	// Load resource compilers from the toolsinfo
	const eastl::vector<eastl::string> toolsInfoResourceCompilers = toolsInfo.GetResourceCompilers();
	for ( uint32 index = 0, count = (uint32)toolsInfoResourceCompilers.size(); index < count; ++index )
	{
		const eastl::string& resourceCompiler = toolsInfoResourceCompilers[index];
		if ( !resourceCompiler.empty() && !LoadResourceCompiler( S_Sprintf( "%s" DLL_EXT_STRING, resourceCompiler.c_str() ).c_str() ) )
		{
			Error( "ResourceCompiler: Failed to load resource compiler '%s" DLL_EXT_STRING "'", resourceCompiler.c_str() );
			return false;
		}
	}

	// We are done
	return true;
}

/*
==================
CResourceCompilerApp::LoadResourceCompiler
==================
*/
bool CResourceCompilerApp::LoadResourceCompiler( const char* pPath )
{
	// Load a resource compiler dll
	resourceCompiler_t resourceCompiler;
	resourceCompiler.handle = g_pFileSystem->LoadModule( pPath );
	if ( resourceCompiler.handle == INVALID_DLL_HANDLE )
	{
		return false;
	}

	// Get an interface factory of the module
	createInterfaceFn_t pFactory = Sys_GetFactory( resourceCompiler.handle );
	if ( !pFactory )
	{
		Warning( "ResourceCompiler: Could not find " CREATEINTERFACE_FUNCNAME " from '%s'", pPath );
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		return false;
	}

	// Get an resource compiler from the module
	resourceCompiler.pCompiler = (IResourceCompiler*)pFactory( RESOURCECOMPILER_INTERFACE_VERSION );
	if ( !resourceCompiler.pCompiler )
	{
		Warning( "ResourceCompiler: Could not get " RESOURCECOMPILER_INTERFACE_VERSION " interface from '%s'", pPath );
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		return false;
	}

	// Make sure that the resource compiler use any source format
	if ( resourceCompiler.pCompiler->GetNumSrcFormats() <= 0 )
	{
		Warning( "ResourceCompiler: Resource compiler '%s' hasn't any source format", pPath );
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		return false;
	}

	// We make sure that there are no collisions in the source formats
	bool   bSrcFormatsAreFree = true;
	uint32 srcFormatIndex	  = 0;
	for ( uint32 count = resourceCompiler.pCompiler->GetNumSrcFormats(), srcFormatIndex = 0; srcFormatIndex < count && bSrcFormatsAreFree; ++srcFormatIndex )
	{
		bSrcFormatsAreFree &= !FindResourceCompiler( resourceCompiler.pCompiler->GetSrcFormat( srcFormatIndex ) );
	}
	if ( !bSrcFormatsAreFree )
	{
		Warning( "ResourceCompiler: Source format '%s' already used by another resource compiler", resourceCompiler.pCompiler->GetSrcFormat( srcFormatIndex ) );
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		return false;
	}

	// Initialize the resource compiler
	if ( !resourceCompiler.pCompiler->Init( GetFactory() ) )
	{
		Warning( "ResourceCompiler: Failed to initialize resource compiler '%s'", pPath );
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		return false;
	}

	// Add the resource compiler into arrays
	uint32 resourceCompilerId = (uint32)resourceCompilers.size();
	resourceCompilers.emplace_back( resourceCompiler );
	for ( uint32 index = 0, count = resourceCompiler.pCompiler->GetNumSrcFormats(); index < count; ++index )
	{
		resourceCompilerDict[resourceCompiler.pCompiler->GetSrcFormat( index )] = resourceCompilerId;
	}

	// We are done
	Msg( "ResourceCompiler: Loaded resource compiler '%s' (path: '%s')", resourceCompiler.pCompiler->GetName(), pPath );
	return true;
}

/*
==================
CResourceCompilerApp::UnloadGameInfo
==================
*/
void CResourceCompilerApp::UnloadGameInfo()
{
	// We look for search paths in gameinfo.txt and remove they from the file system
	eastl::unordered_set<eastl::string>		   pathIDSet;
	const eastl::vector<gameInfoSearchPath_t>& searchPaths = gameInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const gameInfoSearchPath_t& searchPath = searchPaths[index];
		pathIDSet.insert( searchPath.id );
	}

	for ( auto it = pathIDSet.begin(), itEnd = pathIDSet.end(); it != itEnd; ++it )
	{
		g_pFileSystem->RemoveSearchPath( ( *it ).c_str() );
	}

	// Reset the gameinfo
	gameInfo.Clear();
}

/*
==================
CResourceCompilerApp::UnloadToolsInfo
==================
*/
void CResourceCompilerApp::UnloadToolsInfo()
{
	// Unload all resource compilers
	for ( int32 index = (int32)resourceCompilers.size(); --index >= 0; )
	{
		UnloadResourceCompiler( resourceCompilers[index] );
	}
	resourceCompilers.clear();
	resourceCompilerDict.clear();

	// We look for search paths in toolsinfo.txt and remove they from the file system
	eastl::unordered_set<eastl::string>			pathIDSet;
	const eastl::vector<toolsInfoSearchPath_t>& searchPaths = toolsInfo.GetSearchPaths();
	for ( uint32 index = 0, count = (uint32)searchPaths.size(); index < count; ++index )
	{
		const toolsInfoSearchPath_t& searchPath = searchPaths[index];
		pathIDSet.insert( searchPath.id );
	}

	for ( auto it = pathIDSet.begin(), itEnd = pathIDSet.end(); it != itEnd; ++it )
	{
		g_pFileSystem->RemoveSearchPath( ( *it ).c_str() );
	}

	// Reset the toolsinfo
	toolsInfo.Clear();
}

/*
==================
CResourceCompilerApp::UnloadResourceCompiler
==================
*/
void CResourceCompilerApp::UnloadResourceCompiler( resourceCompiler_t& resourceCompiler )
{
	Assert( resourceCompiler.pCompiler );
	Msg( "ResourceCompiler: Unloaded resource compiler '%s'", resourceCompiler.pCompiler->GetName() );

	resourceCompiler.pCompiler->Shutdown();
	if ( resourceCompiler.handle != INVALID_DLL_HANDLE )
	{
		g_pFileSystem->UnloadModule( resourceCompiler.handle );
		resourceCompiler.handle = INVALID_DLL_HANDLE;
	}
}

/*
==================
CResourceCompilerApp::LoadFileList
==================
*/
bool CResourceCompilerApp::LoadFileList( const char* pPath )
{
	Msg( "ResourceCompiler: Load file list '%s'", pPath );

	// Load key values file
	CKeyValues keyValues( "filelist" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}

	// Get resources
	bool bResult = true;
	for ( CKeyValuesSubKeysIterator it( &keyValues, "file" ); it; ++it )
	{
		// Make sure that a file in work directory
		eastl::string absoluteFilePath;
		S_MakeAbsolutePath( it->GetString( NULL ), absoluteFilePath, workDir );
		if ( S_Strnicmp( absoluteFilePath.c_str(), workDir.c_str(), (uint32)workDir.size() ) )
		{
			Warning( "ResourceCompiler: Resource file '%s' must be in '%s'", absoluteFilePath.c_str(), workDir.c_str() );
			bResult = false;
			continue;
		}

		// Add the file to compile list
		if ( !AddFileToCompile( absoluteFilePath.c_str(), workDir.c_str() ) )
		{
			bResult = false;
			continue;
		}
	}

	// We are done
	return bResult;
}

/*
==================
CResourceCompilerApp::AddFileToCompile
==================
*/
bool CResourceCompilerApp::AddFileToCompile( const char* pPath, const char* pWorkDir )
{
	// Convert of the file path to absolute
	eastl::string absoluteSourceFilePath;
	S_MakeAbsolutePath( pPath, absoluteSourceFilePath, pWorkDir );

	// Make sure that the file is exists
	if ( !g_pFileSystem->IsFileExists( absoluteSourceFilePath.c_str() ) )
	{
		Warning( "ResourceCompiler: File not found '%s'", absoluteSourceFilePath.c_str() );
		return false;
	}

	// Get an absolute destination path
	eastl::string tempBuffer;
	eastl::string sourceFileDir;
	eastl::string baseFileName;
	S_MakeRelativePath( absoluteSourceFilePath, pWorkDir, tempBuffer, false );
	S_GetFilePath( tempBuffer, sourceFileDir, false );
	S_GetFileBaseName( absoluteSourceFilePath, baseFileName, false );
	S_AppendPathSeparator( sourceFileDir );

	// Add into a list the file
	resourceFile_t resourceFile = {};
	resourceFile.srcPath		= absoluteSourceFilePath;
	resourceFile.destPath		= S_Sprintf( "%s%s%s", outputPath.c_str(), sourceFileDir.c_str(), baseFileName.c_str() );

	Msg( "ResourceCompiler: File '%s' added to compile", absoluteSourceFilePath.c_str() );
	files.emplace_back( resourceFile );
	return true;
}

/*
==================
CResourceCompilerApp::Compile
==================
*/
FORCEINLINE bool CResourceCompilerApp::Compile( const resourceFile_t& resourceFile ) const
{
	const char*		   pFileExtension	 = S_GetFileExtension( resourceFile.srcPath.c_str() );
	IResourceCompiler* pResourceCompiler = FindResourceCompiler( pFileExtension );
	if ( !pResourceCompiler )
	{
		Warning( "ResourceCompiler: Not found resource compiler for '%s'", pFileExtension );
		return false;
	}

	return pResourceCompiler->Compile( resourceFile.srcPath.c_str(), resourceFile.destPath.c_str() );
}

/*
==================
CResourceCompilerApp::FindResourceCompiler
==================
*/
FORCEINLINE IResourceCompiler* CResourceCompilerApp::FindResourceCompiler( const char* pSrcFormat ) const
{
	auto it = resourceCompilerDict.find( pSrcFormat );
	if ( it == resourceCompilerDict.end() )
	{
		return NULL;
	}

	Assert( it->second < (uint32)resourceCompilers.size() );
	return resourceCompilers[it->second].pCompiler;
}

/*
==================
CResourceCompilerApp::GetAppInfo
==================
*/
const appInfo_t& CResourceCompilerApp::GetAppInfo() const
{
	static appInfo_t s_appInfo{ "resourcecompiler", APPLICATION_TYPE_CONSOLE, CMD_FLAG_NONE, FCVAR_NONE, NULL, NULL };
	return s_appInfo;
}

/*
==================
CResourceCompilerApp::GetCrashDumpInfo
==================
*/
const crashDumpAppInfo_t& CResourceCompilerApp::GetCrashDumpInfo() const
{
	static crashDumpAppInfo_t s_crashDumpAppInfo{ "Resource Compiler", __DATE__ " " __TIME__, NULL, NULL };
	return s_crashDumpAppInfo;
}

/*
==================
main
==================
*/
int main( int argc, char** argv )
{
	// Prepare a command line
	eastl::string commandLine;
	for ( uint32 index = 0; index < (uint32)argc; ++index )
	{
		if ( *argv[index] == '-' || *argv[index] == '/' )
		{
			commandLine += argv[index];
			commandLine += " ";
		}
		else
		{
			commandLine += S_Sprintf( "\"%s\" ", argv[index] );
		}
	}

	return CResourceCompilerApp( commandLine.c_str() ).Run();
}
