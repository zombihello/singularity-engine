#include "pch_materialsystem.h"
#include "tier1/debugname.h"
#include "filesystem/ifilesystem.h"
#include "utils/shadercache/shadercache.h"
#include "materialsystem/shaderlib/shaderlib.h"
#include "materialsystem/ishadermgr.h"
#include "materialsystem/ishader.h"
#include "materialsystem/materialsystem.h"

//-----------------------------------------------------------------------------
// Information about a shader library
//-----------------------------------------------------------------------------
struct shaderLibInfo_t
{
	typedef eastl::unordered_map<const char*, IShader*, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> shadersDict_t;

	eastl::string							 fileName;
	dllHandle_t								 moduleHandle;
	IShaderLib*								 pShaderLib;
	bool									 bGameShaderLib;  // TRUE if this is a game's shader library, in which case it's not allowed to override any existing shader names
	shadersDict_t							 shadersDict;
	eastl::vector<CRefPtr<IStudioAPIShader>> shaderCaches[STUDIOAPI_SHADER_NUM_TYPES];
};

//-----------------------------------------------------------------------------
// Shader manager
//-----------------------------------------------------------------------------
class CShaderMgr : public IShaderMgr
{
public:
	// IShaderMgr interface
	// Initialize and shutdown the manager
	// NOTE: Those functions loads/unloads all engine shader DLLs
	virtual void Init() override;
	virtual void Shutdown() override;

	// Initialize and shutdown the manager for the game
	// NOTE: Those functions loads/unloads all game specific shader DLLs
	virtual void GameInit() override;
	virtual void GameShutdown() override;

	virtual IShader*		  FindShader( const char* pShaderName ) const override;
	virtual IStudioAPIShader* GetStudioAPIShader( uint32 shaderLibIndex, studioAPIShaderType_t shaderType, uint64 shaderIndex ) const override;

private:
	bool   LoadShaderLib( const char* pPath, bool bGameShaderLib = false );
	void   UnloadShaderLib( uint32 index );
	uint32 FindShaderLib( const char* pPath ) const;
	bool   InitShaderLibInfo( const shaderLibInfo_t& shaderLibInfo );
	void   SetupShaderDictionary( uint32 index );

	bool LoadShaderCaches( uint32 index );
	void UnloadShaderCaches( uint32 index );

	void LoadShaderLibs();
	void UnloadShaderLibs();
	void LoadGameShaderLibs();
	void UnloadGameShaderLibs();

	eastl::vector<shaderLibInfo_t> shaderLibs;
};

EXPOSE_SINGLE_INTERFACE( CShaderMgr, IShaderMgr, SHADERMGR_INTERFACE_VERSION );

/*
==================
CShaderMgr::Init
==================
*/
void CShaderMgr::Init()
{
	// Load all engine's shader libraries
	LoadShaderLibs();
}

/*
==================
CShaderMgr::Shutdown
==================
*/
void CShaderMgr::Shutdown()
{
	// Unload all engine's shader libraries
	UnloadShaderLibs();
}

/*
==================
CShaderMgr::GameInit
==================
*/
void CShaderMgr::GameInit()
{
	// Load game's shader libraries
	LoadGameShaderLibs();
}

/*
==================
CShaderMgr::GameShutdown
==================
*/
void CShaderMgr::GameShutdown()
{
	// Unload game's shader libraries
	UnloadGameShaderLibs();
}

/*
==================
CShaderMgr::LoadShaderLib
==================
*/
bool CShaderMgr::LoadShaderLib( const char* pPath, bool bGameShaderLib /* = false */ )
{
	// Load shader module
	dllHandle_t shaderModule = g_pFileSystem->LoadModule( pPath );
	if ( !shaderModule )
	{
		Warning( "ShaderMgr: Failed to load shader library '%s'", pPath );
		return false;
	}

	// Get at the shader library interface
	createInterfaceFn_t pFactory = Sys_GetFactory( shaderModule );
	if ( !pFactory )
	{
		Warning( "ShaderMgr: Could not find factory interface in '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	IShaderLib* pShaderLib = (IShaderLib*)pFactory( SHADERLIB_INTERFACE_VERSION );
	if ( !pShaderLib )
	{
		Warning( "ShaderMgr: Could not get IShaderLib interface from '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	// Allow the library try to connect to other interfaces
	if ( !pShaderLib->Connect( g_pAppSystemFactory ) )
	{
		Warning( "ShaderMgr: Failed to initialize shader library '%s'", pPath );
		g_pFileSystem->UnloadModule( shaderModule );
		return false;
	}

	// Initialize a shader library information
	shaderLibInfo_t shaderLibInfo = {};
	shaderLibInfo.fileName		  = pPath;
	shaderLibInfo.moduleHandle	  = shaderModule;
	shaderLibInfo.pShaderLib	  = pShaderLib;
	shaderLibInfo.bGameShaderLib  = bGameShaderLib;
	return InitShaderLibInfo( shaderLibInfo );
}

/*
==================
CShaderMgr::InitShaderLibInfo
==================
*/
bool CShaderMgr::InitShaderLibInfo( const shaderLibInfo_t& shaderLibInfo )
{
	// Now replace any existing shader
	uint32			 shaderLibIndex = FindShaderLib( shaderLibInfo.fileName.c_str() );
	shaderLibInfo_t* pShaderLibInfo = NULL;
	if ( shaderLibIndex != INVALID_INDEX )
	{
		UnloadShaderLib( shaderLibIndex );
		pShaderLibInfo = &shaderLibs[shaderLibIndex];
	}
	else
	{
		shaderLibIndex = (uint32)shaderLibs.size();
		pShaderLibInfo = &shaderLibs.emplace_back();
	}

	// Ok, the shader library good!
	pShaderLibInfo->fileName	   = shaderLibInfo.fileName;
	pShaderLibInfo->moduleHandle   = shaderLibInfo.moduleHandle;
	pShaderLibInfo->pShaderLib	   = shaderLibInfo.pShaderLib;
	pShaderLibInfo->bGameShaderLib = shaderLibInfo.bGameShaderLib;

	// Add the shaders to the dictionary of shaders and initialize the library
	SetupShaderDictionary( shaderLibIndex );
	shaderLibInfo.pShaderLib->Init( shaderLibIndex );
	Msg( "ShaderMgr: Shader library '%s' initialized", shaderLibInfo.fileName.c_str() );

	// Load shader caches for the shader library
	if ( !LoadShaderCaches( shaderLibIndex ) )
	{
		UnloadShaderLib( shaderLibIndex );
		shaderLibs.erase( shaderLibs.begin() + shaderLibIndex );
		return false;
	}
	return true;
}

/*
==================
CShaderMgr::UnloadShaderLib
==================
*/
void CShaderMgr::UnloadShaderLib( uint32 index )
{
	// Get a shader library info by the index
	Assert( index != INVALID_INDEX );
	shaderLibInfo_t& info = shaderLibs[index];

	// Unload all shader caches
	UnloadShaderCaches( index );

	// Shutdown the shader library
	if ( info.pShaderLib )
	{
		info.pShaderLib->Shutdown();
	}

	// Unload the shader module
	if ( info.moduleHandle )
	{
		info.pShaderLib->Disconnect();
		g_pFileSystem->UnloadModule( info.moduleHandle );
	}

	Msg( "ShaderMgr: Shader library '%s' unloaded", info.fileName.c_str() );
	info.fileName.clear();
	info.shadersDict.clear();
	info.pShaderLib	  = NULL;
	info.moduleHandle = NULL;
}

/*
==================
CShaderMgr::LoadShaderCaches
==================
*/
bool CShaderMgr::LoadShaderCaches( uint32 index )
{
	// Get a shader library info and shader platform name
	Assert( index != INVALID_INDEX );
	shaderLibInfo_t& info			 = shaderLibs[index];
	const char*		 pShaderPlatform = g_pStudioAPI->GetInfo().pShaderPlatform;
	Assert( pShaderPlatform && info.pShaderLib );
	eastl::string shaderCacheDir = S_Sprintf( "//%s/shaders/%s", info.bGameShaderLib ? "game" : "core", pShaderPlatform );

	// Load shader caches for each shader
	uint32 numShaders		= info.pShaderLib->GetNumShaders();
	uint32 numLoadedShaders = 0;
	for ( uint32 shaderIdx = 0; shaderIdx < numShaders; ++shaderIdx )
	{
		// Load shader caches for a shader
		shaderInitParams_t shaderInitParams = {};
		IShader*		   pShader			= info.pShaderLib->GetShader( shaderIdx );
		uint32			   numCacheNames	= pShader->GetNumCacheNames();
		bool			   bOk				= true;
		for ( uint32 cacheNameIdx = 0; cacheNameIdx < numCacheNames && bOk; ++cacheNameIdx )
		{
			// Load shader cache file for the shader
			const char*		pCacheName = pShader->GetCacheName( cacheNameIdx );
			CShaderCacheDoc shaderCacheDoc;
			if ( !shaderCacheDoc.LoadFromFile( S_Sprintf( "%s/%s.ssc", shaderCacheDir.c_str(), pCacheName ).c_str() ) )
			{
				Warning( "ShaderMgr: Failed to load shader cache '%s/%s.ssc' for '%s'", shaderCacheDir.c_str(), pCacheName, pShader->GetName() );
				bOk = false;
				break;
			}

			// Load all shader caches
			studioAPIShaderType_t shaderType	  = shaderCacheDoc.GetType();
			shaderCacheInfo_t&	  shaderCacheInfo = shaderInitParams.cacheInfos[shaderType];
			shaderCacheInfo.numCaches			  = shaderCacheDoc.GetNumCaches();
			shaderCacheInfo.indexOffset			  = (uint64)info.shaderCaches[shaderType].size();
			for ( uint64 cacheIdx = 0; cacheIdx < shaderCacheInfo.numCaches && bOk; ++cacheIdx )
			{
				// Load the shader cache for the vertex type
				const CShaderCacheDoc::shaderCache_t& shaderCache = shaderCacheDoc.GetCache( cacheIdx );
				CRefPtr<IStudioAPIShader>			  pStudioAPIShader;
				switch ( shaderType )
				{
				case STUDIOAPI_SHADER_TYPE_VERTEX: pStudioAPIShader = g_pStudioAPI->CreateVertexShader( shaderCache.entryPointName.c_str(), shaderCache.bytecode.data(), shaderCache.bytecode.size(), shaderCache.reflectionData.data(), shaderCache.reflectionData.size(), DEBUGNAME( pCacheName ) ); break;
				case STUDIOAPI_SHADER_TYPE_HULL: pStudioAPIShader = g_pStudioAPI->CreateHullShader( shaderCache.entryPointName.c_str(), shaderCache.bytecode.data(), shaderCache.bytecode.size(), shaderCache.reflectionData.data(), shaderCache.reflectionData.size(), DEBUGNAME( pCacheName ) ); break;
				case STUDIOAPI_SHADER_TYPE_DOMAIN: pStudioAPIShader = g_pStudioAPI->CreateDomainShader( shaderCache.entryPointName.c_str(), shaderCache.bytecode.data(), shaderCache.bytecode.size(), shaderCache.reflectionData.data(), shaderCache.reflectionData.size(), DEBUGNAME( pCacheName ) ); break;
				case STUDIOAPI_SHADER_TYPE_GEOMETRY: pStudioAPIShader = g_pStudioAPI->CreateGeometryShader( shaderCache.entryPointName.c_str(), shaderCache.bytecode.data(), shaderCache.bytecode.size(), shaderCache.reflectionData.data(), shaderCache.reflectionData.size(), DEBUGNAME( pCacheName ) ); break;
				case STUDIOAPI_SHADER_TYPE_PIXEL: pStudioAPIShader = g_pStudioAPI->CreatePixelShader( shaderCache.entryPointName.c_str(), shaderCache.bytecode.data(), shaderCache.bytecode.size(), shaderCache.reflectionData.data(), shaderCache.reflectionData.size(), DEBUGNAME( pCacheName ) ); break;
				default:
					Warning( "ShaderMgr: Unsupported shader type 0x%X (cache: %i, path: '%s/%s.ssc')", shaderType, cacheIdx + 1, shaderCacheDir.c_str(), pCacheName );
					bOk = false;
					break;
				}

				// Remember the loaded shader cache if all good
				if ( bOk )
				{
					if ( pStudioAPIShader )
					{
						Msg( "ShaderMgr: Shader cache %i/%i loaded (path: '%s/%s.ssc')", cacheIdx + 1, shaderCacheInfo.numCaches, shaderCacheDir.c_str(), pCacheName );
						info.shaderCaches[shaderType].emplace_back( pStudioAPIShader );
					}
					else
					{
						Warning( "ShaderMgr: Failed to load shader cache %i/%i (path: '%s/%s.ssc')", cacheIdx + 1, shaderCacheInfo.numCaches, shaderCacheDir.c_str(), pCacheName );
						bOk = false;
						break;
					}
				}
			}
		}

		if ( bOk )
		{
			Msg( "ShaderMgr: Shader caches for '%s' loaded", pShader->GetName() );
			pShader->Init( shaderInitParams );
			++numLoadedShaders;
		}
		else
		{
			Warning( "ShaderMgr: Shader caches for '%s' not loaded", pShader->GetName() );
		}
	}

	Msg( "ShaderMgr: Loaded caches for shaders %i/%i (library: '%s', platform: '%s')", numLoadedShaders, numShaders, info.fileName.c_str(), pShaderPlatform );
	return numLoadedShaders == numShaders;
}

/*
==================
CShaderMgr::UnloadShaderCaches
==================
*/
void CShaderMgr::UnloadShaderCaches( uint32 index )
{
	// Get a shader library info by the index
	Assert( index != INVALID_INDEX );
	shaderLibInfo_t& info = shaderLibs[index];

	// Unload all shader caches
	for ( uint32 shaderTypeIdx = 0; shaderTypeIdx < STUDIOAPI_SHADER_NUM_TYPES; ++shaderTypeIdx )
	{
		info.shaderCaches[shaderTypeIdx].clear();
	}
	Msg( "ShaderMgr: Removed all shader caches for '%s'", info.fileName.c_str() );
}

/*
==================
CShaderMgr::SetupShaderDictionary
==================
*/
void CShaderMgr::SetupShaderDictionary( uint32 index )
{
	shaderLibInfo_t& info		= shaderLibs[index];
	uint32			 numShaders = info.pShaderLib->GetNumShaders();
	for ( uint32 index = 0; index < numShaders; ++index )
	{
		// Make sure it doesn't try to override another shader library's names
		IShader* pShader = info.pShaderLib->GetShader( index );
		if ( info.bGameShaderLib )
		{
			for ( uint32 testIdx = 0, countShaderLibs = (uint32)shaderLibs.size(); testIdx < countShaderLibs; ++testIdx )
			{
				const shaderLibInfo_t& testInfo = shaderLibs[testIdx];
				if ( !testInfo.bGameShaderLib )
				{
					if ( testInfo.shadersDict.find( pShader->GetName() ) != testInfo.shadersDict.end() )
					{
						Error( "ShaderMgr: Game shaders '%s' trying to override base shader '%s'", info.fileName.c_str(), pShader->GetName() );
					}
				}
			}
		}

		info.shadersDict.insert( eastl::make_pair( pShader->GetName(), pShader ) );
	}
}

/*
==================
CShaderMgr::FindShaderLib
==================
*/
uint32 CShaderMgr::FindShaderLib( const char* pPath ) const
{
	PROFILER_SCOPE_FUNC();
	for ( int32 index = (int32)shaderLibs.size(); --index >= 0; )
	{
		const shaderLibInfo_t& info = shaderLibs[index];
		if ( !S_Stricmp( pPath, info.fileName.c_str() ) )
		{
			return index;
		}
	}

	return INVALID_INDEX;
}

/*
==================
CShaderMgr::LoadShaderLibs
==================
*/
void CShaderMgr::LoadShaderLibs()
{
	// Unload old shader libs
	UnloadShaderLibs();

	// Initialize default shader library
	shaderLibInfo_t defaultShaderLibInfo = {};
	defaultShaderLibInfo.fileName		 = "default";
	defaultShaderLibInfo.moduleHandle	 = INVALID_DLL_HANDLE;
	defaultShaderLibInfo.pShaderLib		 = &GetShaderLib();
	defaultShaderLibInfo.bGameShaderLib	 = false;
	if ( !InitShaderLibInfo( defaultShaderLibInfo ) )
	{
		Sys_Error( "Failed to initialize shader library 'default'" );
	}

	// Load standard shader library
	if ( !LoadShaderLib( "//enginebin/stdshaders" DLL_EXT_STRING ) )
	{
		Sys_Error( "Failed to load //enginebin/stdshaders" DLL_EXT_STRING );
	}
}

/*
==================
CShaderMgr::UnloadShaderLibs
==================
*/
void CShaderMgr::UnloadShaderLibs()
{
	if ( !shaderLibs.empty() )
	{
		for ( int32 index = (int32)shaderLibs.size(); --index >= 0; )
		{
			UnloadShaderLib( index );
		}

		shaderLibs.clear();
	}
}

/*
==================
CShaderMgr::LoadGameShaderLibs
==================
*/
void CShaderMgr::LoadGameShaderLibs()
{
	// Unload old game's shader libraries
	UnloadGameShaderLibs();

	// Load game's shader lib
	LoadShaderLib( "//gamebin/game_shaders" DLL_EXT_STRING, true );
}

/*
==================
CShaderMgr::UnloadGameShaderLibs
==================
*/
void CShaderMgr::UnloadGameShaderLibs()
{
	if ( !shaderLibs.empty() )
	{
		for ( int32 index = (int32)shaderLibs.size(); --index >= 0; )
		{
			const shaderLibInfo_t& info = shaderLibs[index];
			if ( info.bGameShaderLib )
			{
				UnloadShaderLib( index );
				shaderLibs.erase( shaderLibs.begin() + index );
			}
		}
	}
}

/*
==================
CShaderMgr::GetStudioAPIShader
==================
*/
IStudioAPIShader* CShaderMgr::GetStudioAPIShader( uint32 shaderLibIndex, studioAPIShaderType_t shaderType, uint64 shaderIndex ) const
{
	PROFILER_SCOPE_FUNC();
	Assert( shaderLibIndex < (uint32)shaderLibs.size() );
	Assert( shaderType < STUDIOAPI_SHADER_NUM_TYPES );

	const shaderLibInfo_t& info = shaderLibs[shaderLibIndex];
	Assert( shaderIndex < (uint64)info.shaderCaches[shaderType].size() );
	return info.shaderCaches[shaderType][shaderIndex];
}

/*
==================
CShaderMgr::FindShader
==================
*/
IShader* CShaderMgr::FindShader( const char* pShaderName ) const
{
	PROFILER_SCOPE_FUNC();
	for ( int32 shaderLibIdx = (int32)shaderLibs.size(); --shaderLibIdx >= 0; )
	{
		const shaderLibInfo_t& info	  = shaderLibs[shaderLibIdx];
		auto				   itFind = info.shadersDict.find( pShaderName );
		if ( itFind == info.shadersDict.end() )
		{
			continue;
		}
		return itFind->second;
	}

	return NULL;
}
