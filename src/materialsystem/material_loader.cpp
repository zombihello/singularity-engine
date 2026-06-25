#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "utils/smatdoc/smat_compiled_doc.h"
#include "materialsystem/material.h"
#include "materialsystem/material_loader.h"

/*
==================
CMaterialLoader::Load
==================
*/
bool CMaterialLoader::Load( const char* pPath, IResourceData* pData ) const
{
	Assert( pData );
	CSMATCompiledMaterialDoc smatCompiledMaterialDoc;
	if ( !smatCompiledMaterialDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smat_c", pPath ).c_str() ) )
	{
		return NULL;
	}

	CMaterial* pMaterial = (CMaterial*)pData;
	pMaterial->Init( smatCompiledMaterialDoc );
	return true;
}

/*
==================
CMaterialLoader::GetFormatName
==================
*/
const char* CMaterialLoader::GetFormatName() const
{
	return "Singularity Material";
}
