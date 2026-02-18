#include "pch_entitydesc_tool.h"
#include "utils/sentdoc/sent_compiled_doc.h"
#include "tools/resource_tools/ientitydesc_tool.h"

//-----------------------------------------------------------------------------
// Entity descriptor tool
//-----------------------------------------------------------------------------
class CEntityDescTool : public CBaseAppSystem<IEntityDescTool>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// IEntityDescTool interface
	virtual bool CompileEntityDesc( const resourceToolCompileEntityDescParams_t& compileParams ) const override;
};

EXPOSE_SINGLE_INTERFACE( CEntityDescTool, IEntityDescTool, ENTITYDESC_TOOL_INTERFACE_VERSION );

/*
==================
CEntityDescTool::Connect
==================
*/
bool CEntityDescTool::Connect( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CEntityDescTool::Disconnect
==================
*/
void CEntityDescTool::Disconnect()
{
	DisconnectTier1();
}

/*
==================
CEntityDescTool::CompileEntityDesc
==================
*/
bool CEntityDescTool::CompileEntityDesc( const resourceToolCompileEntityDescParams_t& compileParams ) const
{
	// Convert entity variables
	eastl::string destPath = S_Sprintf( "%s.sent_c", compileParams.pDestPath );
	Msg( "EntityDescTool: Saving the entity descriptor to '%s'...", destPath.c_str() );
	CSENTCompiledEntityDescDoc sentCompiledFile;

	bool bEntityDescComponentsAreValid = true;
	for ( uint32 componentIdx = 0; componentIdx < compileParams.numComponents; ++componentIdx )
	{
		CSENTEntityDescComponent				sentEntityDescComponent;
		const resourceToolEntityDescComponent_t resourceToolEntityDescComponent = compileParams.pComponents[componentIdx];
		sentEntityDescComponent.SetType( resourceToolEntityDescComponent.pType );
		for ( uint32 varIdx = 0; varIdx < resourceToolEntityDescComponent.numVars; ++varIdx )
		{
			CSENTEntityDescVar				  sentEntityDescVar;
			const resourceToolEntityDescVar_t resourceToolEntityDescVar = resourceToolEntityDescComponent.pVars[varIdx];
			sentEntityDescVar.SetName( resourceToolEntityDescVar.pName );
			switch ( resourceToolEntityDescVar.type )
			{
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_UNDEFINED: Warning( "EntityDescTool: Entity descriptor variable '%s' is undefined, skipped", resourceToolEntityDescVar.pName ); continue;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_BOOL: sentEntityDescVar.SetBoolValue( resourceToolEntityDescVar.boolValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_INT: sentEntityDescVar.SetIntValue( resourceToolEntityDescVar.intValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_FLOAT: sentEntityDescVar.SetFloatValue( resourceToolEntityDescVar.floatValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_2D: sentEntityDescVar.SetVec2Value( resourceToolEntityDescVar.vector2DValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_3D: sentEntityDescVar.SetVec3Value( resourceToolEntityDescVar.vector3DValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_4D: sentEntityDescVar.SetVec4Value( resourceToolEntityDescVar.vector4DValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_MATRIX: sentEntityDescVar.SetMatrixValue( resourceToolEntityDescVar.matrixValue ); break;
			case RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_STRING: sentEntityDescVar.SetStringValue( resourceToolEntityDescVar.pStringValue ); break;
			default:
				Error( "EntityDescTool: Unknown entity descriptor type 0x%X in variable '%s'", resourceToolEntityDescVar.type, resourceToolEntityDescVar.pName );
				bEntityDescComponentsAreValid = false;
				continue;
			}

			sentEntityDescComponent.AddVar( sentEntityDescVar );
		}

		sentCompiledFile.AddComponent( sentEntityDescComponent );
	}

	if ( !bEntityDescComponentsAreValid )
	{
		Error( "EntityDescTool: Some entity descriptor components are invalid" );
		return false;
	}

	// Save the entity descriptor to compiled SENT format
	if ( !sentCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "EntityDescTool: Failed to save the entity descriptor to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "EntityDescTool: ..The entity descriptor is saved" );
	return true;
}
