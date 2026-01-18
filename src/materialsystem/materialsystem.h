#pragma once
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/texture_factory.h"
#include "materialsystem/material_factory.h"

//-----------------------------------------------------------------------------
// Material system
//-----------------------------------------------------------------------------
// Application systems factory. It used for connect materialsystem's submodules (e.g: stdshaders)
extern createInterfaceFn_t g_pAppSystemFactory;

class CMaterialSystem : public CBaseAppSystem<IMaterialSystem>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const char* pInterfaceName ) override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

private:
	CTextureFactory	 textureFactory;
	CMaterialFactory materialFactory;
};