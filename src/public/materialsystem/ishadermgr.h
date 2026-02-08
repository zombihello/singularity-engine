#pragma once
#include "tier1/interface.h"
#include "studiorender/studioapi/istudioapi_shader.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IShader;

//-----------------------------------------------------------------------------
// Shader manager interface
//-----------------------------------------------------------------------------
#define SHADERMGR_INTERFACE_VERSION "SShaderMgr001"
class IShaderMgr
{
public:
	// Initialize and shutdown the manager
	// NOTE: Those functions loads/unloads all engine shader DLLs
	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	// Initialize and shutdown the manager for the game
	// NOTE: Those functions loads/unloads all game specific shader DLLs
	virtual void GameInit()		= 0;
	virtual void GameShutdown() = 0;

	virtual IShader*		  FindShader( const char* pShaderName ) const															  = 0;
	virtual IStudioAPIShader* GetStudioAPIShader( uint32 shaderLibIndex, studioAPIShaderType_t shaderType, uint64 shaderIndex ) const = 0;
};

//-----------------------------------------------------------------------------
// Shader library interface
//-----------------------------------------------------------------------------
#define SHADERLIB_INTERFACE_VERSION "SShaderLib001"
class IShaderLib
{
public:
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;
	virtual void Disconnect()							 = 0;

	virtual void Init( uint32 shaderLibIndex ) = 0;
	virtual void Shutdown()					   = 0;

	virtual const char* GetName() const					= 0;
	virtual uint32		GetNumShaders() const			= 0;
	virtual IShader*	GetShader( uint32 index ) const = 0;
	virtual uint32		GetIndex() const				= 0;
};
