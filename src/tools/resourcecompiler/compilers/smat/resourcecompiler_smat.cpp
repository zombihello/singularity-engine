#include "utils/smatdoc/smat_source_doc.h"
#include "utils/smatdoc/smat_compiled_doc.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

//-----------------------------------------------------------------------------
// Resource compiler for 'smat'
//-----------------------------------------------------------------------------
class CResourceCompilerSMat : public IResourceCompiler
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

EXPOSE_SINGLE_INTERFACE( CResourceCompilerSMat, IResourceCompiler, RESOURCECOMPILER_INTERFACE_VERSION );

/*
==================
CResourceCompilerSMat::Init
==================
*/
bool CResourceCompilerSMat::Init( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CResourceCompilerSMat::Shutdown
==================
*/
void CResourceCompilerSMat::Shutdown()
{
	DisconnectTier1();
}

/*
==================
CResourceCompilerSMat::CompileMaterial
==================
*/
bool CResourceCompilerSMat::Compile( const char* pSrcPath, const char* pDestPath ) const
{
	// Load a source file
	CSMATSourceMaterialDoc smatSourceFile;
	if ( !smatSourceFile.LoadFromFile( pSrcPath ) )
	{
		Error( "ResourceCompilerSMat: Failed to load SMAT file '%s'", pSrcPath );
		return false;
	}

	// Convert the source file to a compiled
	const eastl::vector<CSMATMaterialVar>& smatSourceMaterialVars = smatSourceFile.GetVars();
	CSMATCompiledMaterialDoc			   smatCompiledFile;
	smatCompiledFile.SetShaderName( smatSourceFile.GetShaderName() );
	for ( uint32 varIdx = 0, numVars = (uint32)smatSourceMaterialVars.size(); varIdx < numVars; ++varIdx )
	{
		smatCompiledFile.AddVar( smatSourceMaterialVars[varIdx] );
	}

	// Save material to compiled SMAT format
	eastl::string destPath = S_Sprintf( "%s.smat_c", pDestPath );
	Msg( "ResourceCompilerSMat: Saving the material to '%s'...", destPath.c_str() );
	if ( !smatCompiledFile.SaveFile( destPath.c_str() ) )
	{
		Error( "ResourceCompilerSMat: Failed to save the material to '%s'", destPath.c_str() );
		return false;
	}
	Msg( "ResourceCompilerSMat: ..The material is saved" );
	return true;
}

/*
==================
CResourceCompilerSMat::GetNumSrcFormats
==================
*/
uint32 CResourceCompilerSMat::GetNumSrcFormats() const
{
	return 1;
}

/*
==================
CResourceCompilerSMat::GetNumSrcFormats
==================
*/
const char* CResourceCompilerSMat::GetSrcFormat( uint32 index ) const
{
	Assert( index < GetNumSrcFormats() );
	return "smat";
}

/*
==================
CResourceCompilerSMat::GetName
==================
*/
const char* CResourceCompilerSMat::GetName() const
{
	return "Singularity Material";
}
