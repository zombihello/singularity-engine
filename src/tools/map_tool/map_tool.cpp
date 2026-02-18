#include "pch_map_tool.h"
#include "utils/smapdoc/smap_compiled_doc.h"
#include "tools/resource_tools/imap_tool.h"

//-----------------------------------------------------------------------------
// Map tool
//-----------------------------------------------------------------------------
class CMapTool : public CBaseAppSystem<IMapTool>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// IMapTool interface
	virtual bool CompileMap( const resourceToolCompileMapParams_t& compileParams ) const override;
};

EXPOSE_SINGLE_INTERFACE( CMapTool, IMapTool, MAP_TOOL_INTERFACE_VERSION );

/*
==================
CMapTool::Connect
==================
*/
bool CMapTool::Connect( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CMapTool::Disconnect
==================
*/
void CMapTool::Disconnect()
{
	DisconnectTier1();
}

/*
==================
CMapTool::CompileMap
==================
*/
bool CMapTool::CompileMap( const resourceToolCompileMapParams_t& compileParams ) const
{
	// Convert entities
	eastl::string destPath = S_Sprintf( "%s.smap_c", compileParams.pDestPath );
	Msg( "MapTool: Saving the map to '%s'...", destPath.c_str() );
	CSMAPCompiledMapDoc smapCompiledFile;

	for ( uint32 entityIdx = 0; entityIdx < compileParams.numEntities; ++entityIdx )
	{
		CSMAPEntity					   smapEntity;
		const resourceToolMapEntity_t* pResourceToolMapEntity = &compileParams.pEntities[entityIdx];
		smapEntity.SetClassName( pResourceToolMapEntity->pClassName );
		smapEntity.SetName( pResourceToolMapEntity->pName );
		smapCompiledFile.AddEntity( smapEntity );
	}

	// Save the map to compiled SMAP format
	if ( !smapCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "MapTool: Failed to save the map to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "MapTool: ..The map is saved" );
	return true;
}
