#pragma once
#include <EASTL/vector.h>

#include "materialsystem/ishadermgr.h"
#include "materialsystem/shaderlib/shader_base.h"

//-----------------------------------------------------------------------------
// Shaders library
//-----------------------------------------------------------------------------
class CShaderLib : public IShaderLib
{
public:
	CShaderLib();

	// IShaderLib interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	virtual void Init( uint32 shaderLibIndex ) override;
	virtual void Shutdown() override;

	virtual uint32	 GetNumShaders() const override;
	virtual IShader* GetShader( uint32 index ) const override;
	virtual uint32	 GetIndex() const override;

	void InsertShader( CBaseShader* pShader );

private:
	uint32						index;
	eastl::vector<CBaseShader*> shaders;
};

CShaderLib& GetShaderLib();
#include "materialsystem/shaderlib/shaderlib.inl"
