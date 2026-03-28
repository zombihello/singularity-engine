#include "pch_shaderlib.h"
#include "studiorender/studioapi/istudioapi.h"
#include "studiorender/istudiorender.h"
#include "materialsystem/ishader.h"
#include "materialsystem/ishadermgr.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/shaderlib/shaderlib.h"

/*
==================
CShaderLib::CShaderLib
==================
*/
CShaderLib::CShaderLib( const char* pName )
	: name( pName )
	, index( INVALID_INDEX )
{
}

/*
==================
CShaderLib::Connect
==================
*/
bool CShaderLib::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Connect Tier1
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	// Get StudioAPI
	g_pStudioAPI = (IStudioAPI*)pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	// Get StudioRender
	g_pStudioRender = (IStudioRender*)pFactory( STUDIORENDER_INTERFACE_VERSION );
	if ( !g_pStudioRender )
	{
		return false;
	}

	// Get shader manager
	g_pShaderMgr = (IShaderMgr*)pFactory( SHADERMGR_INTERFACE_VERSION );
	if ( !g_pShaderMgr )
	{
		return false;
	}

	// Get resource system
	g_pResourceSystem = (IResourceSystem*)pFactory( RESOURCESYSTEM_INTERFACE_VERSION );
	if ( !g_pResourceSystem )
	{
		return false;
	}

	return true;
}

/*
==================
CShaderLib::Disconnect
==================
*/
void CShaderLib::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Disconnect Tier1
	DisconnectTier1();
	g_pStudioAPI	= NULL;
	g_pStudioRender = NULL;
	g_pShaderMgr	= NULL;
}

/*
==================
CShaderLib::Init
==================
*/
void CShaderLib::Init( uint32 shaderLibIndex )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( index == INVALID_INDEX && shaderLibIndex != INVALID_INDEX );
	index = shaderLibIndex;
}

/*
==================
CShaderLib::Shutdown
==================
*/
void CShaderLib::Shutdown()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	for ( uint32 shaderIdx = 0, numShaders = (uint32)shaders.size(); shaderIdx < numShaders; ++shaderIdx )
	{
		shaders[shaderIdx]->Shutdown();
	}
	index = INVALID_INDEX;
}

/*
==================
CShaderLib::GetName
==================
*/
const char* CShaderLib::GetName() const
{
	return name.c_str();
}

/*
==================
CShaderLib::GetNumShaders
==================
*/
uint32 CShaderLib::GetNumShaders() const
{
	return (uint32)shaders.size();
}

/*
==================
CShaderLib::GetShader
==================
*/
IShader* CShaderLib::GetShader( uint32 index ) const
{
	Assert( index < (uint32)shaders.size() );
	return shaders[index];
}

/*
==================
CShaderLib::GetIndex
==================
*/
uint32 CShaderLib::GetIndex() const
{
	return index;
}
