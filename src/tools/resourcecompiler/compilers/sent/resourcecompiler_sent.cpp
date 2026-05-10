#include "utils/sentdoc/sent_source_doc.h"
#include "utils/sentdoc/sent_compiled_doc.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

//-----------------------------------------------------------------------------
// Resource compiler for 'sent'
//-----------------------------------------------------------------------------
class CResourceCompilerSEnt : public IResourceCompiler
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

EXPOSE_SINGLE_INTERFACE( CResourceCompilerSEnt, IResourceCompiler, RESOURCECOMPILER_INTERFACE_VERSION );

/*
==================
CResourceCompilerSEnt::Init
==================
*/
bool CResourceCompilerSEnt::Init( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CResourceCompilerSEnt::Shutdown
==================
*/
void CResourceCompilerSEnt::Shutdown()
{
	DisconnectTier1();
}

/*
==================
CResourceCompilerSEnt::Compile
==================
*/
bool CResourceCompilerSEnt::Compile( const char* pSrcPath, const char* pDestPath ) const
{
	// Load a source file
	CSENTSourceEntityDescDoc sentSourceFile;
	if ( !sentSourceFile.LoadFromFile( pSrcPath ) )
	{
		Error( "ResourceCompilerSEnt: Failed to load SENT file '%s'", pSrcPath );
		return false;
	}

	// Convert the source file to a compiled
	CSENTCompiledEntityDescDoc					   sentCompiledFile;
	const eastl::vector<CSENTEntityDescComponent>& sentSourceEntityComponents = sentSourceFile.GetComponents();
	for ( uint32 componentIdx = 0, numComponents = (uint32)sentSourceEntityComponents.size(); componentIdx < numComponents; ++componentIdx )
	{
		sentCompiledFile.AddComponent( sentSourceEntityComponents[componentIdx] );
	}

	// Save the entity descriptor to compiled SENT format
	eastl::string destPath = S_Sprintf( "%s.sent_c", pDestPath );
	Msg( "ResourceCompilerSEnt: Saving the entity descriptor to '%s'...", destPath.c_str() );
	if ( !sentCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "ResourceCompilerSEnt: Failed to save the entity descriptor to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "ResourceCompilerSEnt: ..The entity descriptor is saved" );
	return true;
}

/*
==================
CResourceCompilerSEnt::GetNumSrcFormats
==================
*/
uint32 CResourceCompilerSEnt::GetNumSrcFormats() const
{
	return 1;
}

/*
==================
CResourceCompilerSEnt::GetNumSrcFormats
==================
*/
const char* CResourceCompilerSEnt::GetSrcFormat( uint32 index ) const
{
	Assert( index < GetNumSrcFormats() );
	return "sent";
}

/*
==================
CResourceCompilerSEnt::GetName
==================
*/
const char* CResourceCompilerSEnt::GetName() const
{
	return "Singularity Entity Descriptor";
}
