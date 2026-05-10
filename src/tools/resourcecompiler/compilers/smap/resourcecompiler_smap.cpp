#include "utils/smapdoc/smap_source_doc.h"
#include "utils/smapdoc/smap_compiled_doc.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

//-----------------------------------------------------------------------------
// Resource compiler for 'smap'
//-----------------------------------------------------------------------------
class CResourceCompilerSMap : public IResourceCompiler
{
public:
	// IResourceCompiler interface
	// Initialize and shutdown
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	// Compile a resource
	// pSrcPath		Path to a source file (with file extension)
	// pDestPath	Path to a destination file (without file extension)
	virtual bool Compile( const char* pSrcPath, const char* pDestPath ) const override;

	// Get source formats and resource type name
	virtual uint32		GetNumSrcFormats() const override;
	virtual const char* GetSrcFormat( uint32 index ) const override;
	virtual const char* GetName() const override;
};

EXPOSE_SINGLE_INTERFACE( CResourceCompilerSMap, IResourceCompiler, RESOURCECOMPILER_INTERFACE_VERSION );

/*
==================
CResourceCompilerSMap::Init
==================
*/
bool CResourceCompilerSMap::Init( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CResourceCompilerSMap::Shutdown
==================
*/
void CResourceCompilerSMap::Shutdown()
{
	DisconnectTier1();
}

/*
==================
CResourceCompilerSMap::CompileMap
==================
*/
bool CResourceCompilerSMap::Compile( const char* pSrcPath, const char* pDestPath ) const
{
	// Load a source file
	CSMAPSourceMapDoc smapSourceFile;
	if ( !smapSourceFile.LoadFromFile( pSrcPath ) )
	{
		Error( "ResourceCompilerSMap: Failed to load SMAP file '%s'", pSrcPath );
		return false;
	}

	// Convert the source file to a compiled
	CSMAPCompiledMapDoc				  smapCompiledFile;
	const eastl::vector<CSMAPEntity>& smapEntities = smapSourceFile.GetEntities();
	for ( uint32 entityIdx = 0, numEntities = (uint32)smapEntities.size(); entityIdx < numEntities; ++entityIdx )
	{
		smapCompiledFile.AddEntity( smapEntities[entityIdx] );
	}

	// Save the map to compiled SMAP format
	eastl::string destPath = S_Sprintf( "%s.smap_c", pDestPath );
	Msg( "ResourceCompilerSMap: Saving the map to '%s'...", destPath.c_str() );
	if ( !smapCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "ResourceCompilerSMap: Failed to save the map to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "ResourceCompilerSMap: ..The map is saved" );
	return true;
}

/*
==================
CResourceCompilerSMap::GetNumSrcFormats
==================
*/
uint32 CResourceCompilerSMap::GetNumSrcFormats() const
{
	return 1;
}

/*
==================
CResourceCompilerSMap::GetNumSrcFormats
==================
*/
const char* CResourceCompilerSMap::GetSrcFormat( uint32 index ) const
{
	Assert( index < GetNumSrcFormats() );
	return "smap";
}

/*
==================
CResourceCompilerSMap::GetName
==================
*/
const char* CResourceCompilerSMap::GetName() const
{
	return "Singularity Map";
}
