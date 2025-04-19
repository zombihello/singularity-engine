#include "pch_material_tool.h"
#include "smatdoc/smat_compiled_doc.h"
#include "tools/resource_tools/imaterial_tool.h"

//-----------------------------------------------------------------------------
// Material tool
//-----------------------------------------------------------------------------
class CMaterialTool : public CBaseAppSystem<IMaterialTool>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// IMaterialTool interface
	virtual bool CompileMaterial( const resourceToolCompileMaterialParams_t& compileParams ) const override;
};

EXPOSE_SINGLE_INTERFACE( CMaterialTool, IMaterialTool, MATERIAL_TOOL_INTERFACE_VERSION );


/*
==================
CMaterialTool::Connect
==================
*/
bool CMaterialTool::Connect( createInterfaceFn_t pFactory )
{
	return ConnectStdLib( pFactory );
}

/*
==================
CMaterialTool::Disconnect
==================
*/
void CMaterialTool::Disconnect()
{
	DisconnectStdLib();
}

/*
==================
CMaterialTool::CompileMaterial
==================
*/
bool CMaterialTool::CompileMaterial( const resourceToolCompileMaterialParams_t& compileParams ) const
{
	// Convert material variables
	std::string			destPath = S_Sprintf( "%s.smat_c", compileParams.pDestPath );
	Msg( "MaterialTool: Saving the material to '%s'...", destPath.c_str() );
	CSMATCompiledMaterialDoc		smatCompiledFile;
	smatCompiledFile.SetShaderName( compileParams.pShaderName );
	
	bool	bMaterialVarsAreVaild = true;
	for ( uint32 varIdx = 0; varIdx < compileParams.numVars; ++varIdx )
	{
		CSMATMaterialVar					smatMaterialVar;
		const resourceToolMaterialVar_t&	resourceToolMaterialVar = compileParams.pVars[varIdx];
		smatMaterialVar.SetName( resourceToolMaterialVar.pName );
		switch ( resourceToolMaterialVar.type )
		{
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_BOOL:		smatMaterialVar.SetBoolValue( resourceToolMaterialVar.boolValue );			break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_INT:		smatMaterialVar.SetIntValue( resourceToolMaterialVar.intValue );			break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_FLOAT:		smatMaterialVar.SetFloatValue( resourceToolMaterialVar.floatValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_2D:	smatMaterialVar.SetVecValue( resourceToolMaterialVar.vector2DValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_3D:	smatMaterialVar.SetVecValue( resourceToolMaterialVar.vector3DValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_4D:	smatMaterialVar.SetVecValue( resourceToolMaterialVar.vector4DValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATRIX:	smatMaterialVar.SetMatrixValue( resourceToolMaterialVar.matrixValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_STRING:	smatMaterialVar.SetStringValue( resourceToolMaterialVar.pStringValue );		break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_TEXTURE:	smatMaterialVar.SetTextureValue( resourceToolMaterialVar.pTextureValue );	break;
		case RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATERIAL:	smatMaterialVar.SetMaterialValue( resourceToolMaterialVar.pMaterialValue );	break;
		default:
			Error( "MaterialTool: Unknown material type 0x%X in variable '%s'", resourceToolMaterialVar.type, resourceToolMaterialVar.pName );
			bMaterialVarsAreVaild = false;
			continue;
		}

		smatCompiledFile.AddVar( smatMaterialVar );
	}

	if ( !bMaterialVarsAreVaild )
	{
		Error( "MaterialTool: Some material variables are invalid" );
		return false;
	}

	// Save material to compiled SMAT format
	if ( !smatCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "MaterialTool: Failed to save the material to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "MaterialTool: ..The material is saved" );
	return true;
}