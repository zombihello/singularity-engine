#include "pch_modelsystem.h"
#include "tier1/filetools.h"
#include "utils/smdldoc/smdl_compiled_doc.h"
#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/iresourcetypemgr.h"
#include "materialsystem/imaterial.h"
#include "modelsystem/model.h"
#include "modelsystem/model_loader.h"

/*
==================
CModelLoader::Load
==================
*/
bool CModelLoader::Load( const char* pPath, IResourceData* pData ) const
{
	PROFILER_SCOPE_FUNC();
	Assert( pData );
	CSMDLCompiledModelDoc smdlCompiledModelDoc;
	if ( !smdlCompiledModelDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smdl_c", pPath ).c_str() ) )
	{
		return false;
	}

	// Load materials
	IResourceTypeMgr*					   pMaterialsMgr  = g_pResourceSystem->GetResourceManagerForType<IMaterial>();
	const char**						   pMaterialsRefs = smdlCompiledModelDoc.GetMaterials();
	eastl::vector<CResourcePtr<IMaterial>> materials;
	for ( uint32 index = 0, count = smdlCompiledModelDoc.GetNumMaterials(); index < count; ++index )
	{
		materials.emplace_back( pMaterialsMgr->LoadResource( pMaterialsRefs[index] ) );
	}

	// Initialize the model
	CModel*			   pModel	   = (CModel*)pData;
	modelInitialData_t initialData = {};
	initialData.vertexType		   = smdlCompiledModelDoc.GetVertexType();
	initialData.indexType		   = smdlCompiledModelDoc.GetIndexType();
	initialData.sizeVertices	   = smdlCompiledModelDoc.GetSizeVertices();
	initialData.sizeIndices		   = smdlCompiledModelDoc.GetSizeIndices();
	initialData.numMaterials	   = smdlCompiledModelDoc.GetNumMaterials();
	initialData.numSurfaces		   = smdlCompiledModelDoc.GetNumSurfaces();
	initialData.pVertices		   = smdlCompiledModelDoc.GetVertices();
	initialData.pIndices		   = smdlCompiledModelDoc.GetIndices();
	initialData.pMaterials		   = materials.data();
	initialData.pSurfaces		   = smdlCompiledModelDoc.GetSurfaces();
	pModel->Init( initialData );
	return true;
}

/*
==================
CModelLoader::GetFormatName
==================
*/
const char* CModelLoader::GetFormatName() const
{
	return "Singularity Model";
}
