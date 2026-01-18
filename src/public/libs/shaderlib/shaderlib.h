#pragma once
#include <vector>

#include "materialsystem/ishadermgr.h"
#include "shaderlib/shader_base.h"

//-----------------------------------------------------------------------------
// Shaders library
//-----------------------------------------------------------------------------
class CShaderLib : public IShaderLib
{
public:
	CShaderLib( const char* pName );

	// IShaderLib interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	virtual void Init( uint32 shaderLibIndex ) override;
	virtual void Shutdown() override;

	virtual const char* GetName() const override;
	virtual uint32		 GetNumShaders() const override;
	virtual IShader*	 GetShader( uint32 index ) const override;
	virtual uint32		 GetIndex() const override;

	void InsertShader( CBaseShader* pShader );

private:
	std::string				  name;
	uint32					  index;
	std::vector<CBaseShader*> shaders;
};
extern CShaderLib g_ShaderLib;

#include "shaderlib/shaderlib.inl"