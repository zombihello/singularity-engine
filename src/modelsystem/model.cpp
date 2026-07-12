#include "pch_modelsystem.h"
#include "studiorender/studioapi/istudioapi.h"
#include "resourcesystem/iresourcesystem.h"
#include "resourcesystem/iresourcetypemgr.h"
#include "materialsystem/imaterial.h"
#include "modelsystem/vertexfactory_simple.h"
#include "modelsystem/vertexfactory_static.h"
#include "modelsystem/modelsystem.h"
#include "modelsystem/model.h"

static uint32 s_strideVertexType[] = {
	sizeof( modelSimpleVertex_t ),	// MODEL_VERTEXTYPE_SIMPLE
	sizeof( modelStaticVertex_t )	// MODEL_VERTEXTYPE_STATIC
};
static_assert( ARRAYSIZE( s_strideVertexType ) == MODEL_VERTEX_NUM_TYPES, "Array size 's_strideVertexType' must be equal to MODEL_VERTEX_NUM_TYPES" );

static uint32 s_strideIndexType[] = {
	sizeof( uint16 ),  // MODEL_INDEXTYPE_UINT16
	sizeof( uint32 )   // MODEL_INDEXTYPE_UINT32
};
static_assert( ARRAYSIZE( s_strideIndexType ) == MODEL_INDEX_NUM_TYPES, "Array size 's_strideIndexType' must be equal to MODEL_INDEX_NUM_TYPES" );

/*
==================
CModelResource::CModelResource
==================
*/
CModelResource::CModelResource()
	: indexType( MODEL_INDEX_NUM_TYPES )
{
}

/*
==================
CModelResource::Update
==================
*/
void CModelResource::Update( const modelInitialData_t& initialData )
{
	// Copy a new index type
	PROFILER_SCOPE_FUNC();
	Assert( initialData.vertexType < MODEL_VERTEX_NUM_TYPES );
	Assert( initialData.indexType < MODEL_INDEX_NUM_TYPES );
	indexType = initialData.indexType;

	// Create a new vertex factory if the resource didn't have one previously, or
	// if the new vertex type doesn't match the factory
	bool bDirtyVertexFactory = false;
	if ( !pVertexFactory || pVertexFactory->GetVertexType() != initialData.vertexType )
	{
		// If the old vertex factory is valid begin release one in the render thread
		bDirtyVertexFactory = true;
		if ( pVertexFactory )
		{
			Studio_BeginReleaseResource( pVertexFactory );
		}

		// Create a new vertex factory for the vertex type
		switch ( initialData.vertexType )
		{
		case MODEL_VERTEXTYPE_SIMPLE: pVertexFactory = new CVertexFactorySimple(); break;
		case MODEL_VERTEXTYPE_STATIC: pVertexFactory = new CVertexFactoryStatic(); break;
		default:
			AssertMsg( false, "Unknown vertex type 0x%X", initialData.vertexType );
			break;
		}
	}
	pVertexFactory->ClearStreams();

	// Copy vertices, indices and surfaces
	vertices.resize( initialData.sizeVertices );
	indices.resize( initialData.sizeIndices );
	surfaces.resize( initialData.numSurfaces );
	Mem_Memcpy( vertices.data(), initialData.pVertices, initialData.sizeVertices );
	Mem_Memcpy( indices.data(), initialData.pIndices, initialData.sizeIndices );
	Mem_Memcpy( surfaces.data(), initialData.pSurfaces, initialData.numSurfaces * sizeof( modelSurface_t ) );

	// Copy materials
	UpdateMaterials( initialData.pMaterials, initialData.numMaterials );

	// Begin update the vertex factory (if it need) and the resource in the render thread
	if ( bDirtyVertexFactory )
	{
		Studio_BeginUpdateResource( pVertexFactory );
	}
	Studio_BeginUpdateResource( this );
}

/*
==================
CModelResource::UpdateMaterials
==================
*/
void CModelResource::UpdateMaterials( const CResourcePtr<IMaterial>* pMaterials, uint32 numMaterials )
{
	PROFILER_SCOPE_FUNC();
	materials.resize( numMaterials );
	IResourceTypeMgr*		pMaterialsMgr	 = g_pResourceSystem->GetResourceManagerForType<IMaterial>();
	CResourcePtr<IMaterial> pDefaultMaterial = pMaterialsMgr->GetDefaultResource();
	for ( uint32 materialId = 0; materialId < numMaterials; ++materialId )
	{
		CResourcePtr<IMaterial> pMaterial = pMaterials[materialId];
		materials[materialId]			  = pMaterial.IsCached() ? pMaterial->GetStudioResource() : pDefaultMaterial->GetStudioResource();
	}
}

/*
==================
CModelResource::Clear
==================
*/
void CModelResource::Clear()
{
	// Reset all fields
	PROFILER_SCOPE_FUNC();
	indexType = MODEL_INDEX_NUM_TYPES;
	vertices.clear();
	indices.clear();
	materials.clear();
	surfaces.clear();

	// Begin release the resource and the vertex factory (if it exists) in the render thread
	Studio_BeginReleaseResource( this );
	if ( pVertexFactory )
	{
		Studio_BeginReleaseResource( pVertexFactory );
		pVertexFactory = NULL;
	}
}

/*
==================
CModelResource::InitStudioAPI
==================
*/
void CModelResource::InitStudioAPI()
{
	// Create a GPU vertex buffer and add all vertex streams into the vertex factory
	Assert( pVertexFactory );
	Assert( !vertices.empty() );
	modelVertexType_t vertexType = pVertexFactory->GetVertexType();
	pStudioAPIVertexBuffer		 = g_pStudioAPI->CreateBuffer( vertices.data(), (uint32)vertices.size(), s_strideVertexType[vertexType], STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_VERTEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
	pVertexFactory->AddVertexStream( vertexFactoryStream_t{ pStudioAPIVertexBuffer, 0 } );
	vertices.clear();

	// Create a GPU index buffer and set a index stream into the vertex factory
	Assert( !indices.empty() );
	pStudioAPIIndexBuffer = g_pStudioAPI->CreateBuffer( indices.data(), (uint32)indices.size(), s_strideIndexType[indexType], STUDIOAPI_BUFFER_USAGE_FLAG_STATIC | STUDIOAPI_BUFFER_USAGE_FLAG_INDEX_BUFFER | STUDIOAPI_BUFFER_USAGE_FLAG_TRANSFER_DST );
	pVertexFactory->SetIndexStream( vertexFactoryStream_t{ pStudioAPIIndexBuffer, 0 } );
	indices.clear();
}

/*
==================
CModelResource::ReleaseStudioAPI
==================
*/
void CModelResource::ReleaseStudioAPI()
{
	pStudioAPIVertexBuffer = NULL;
	pStudioAPIIndexBuffer  = NULL;
}

/*
==================
CModelResource::FinalRelease
==================
*/
void CModelResource::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CModelResource::GetNumMaterials
==================
*/
uint32 CModelResource::GetNumMaterials() const
{
	return (uint32)materials.size();
}

/*
==================
CModelResource::GetMaterials
==================
*/
const CRefPtr<IMaterialResource>* CModelResource::GetMaterials() const
{
	return materials.data();
}

/*
==================
CModelResource::GetNumSurfaces
==================
*/
uint32 CModelResource::GetNumSurfaces() const
{
	return (uint32)surfaces.size();
}

/*
==================
CModelResource::GetSurfaces
==================
*/
const modelSurface_t* CModelResource::GetSurfaces() const
{
	return surfaces.data();
}

/*
==================
CModelResource::GetVertexFactory
==================
*/
IVertexFactory* CModelResource::GetVertexFactory() const
{
	return pVertexFactory;
}

/*
==================
CModel::CModel
==================
*/
CModel::CModel( IResource* pResource )
	: CResourceData<IModel>( pResource )
	, bDirtyMaterials( false )
	, pStudioResource( new CModelResource() )
{
}

/*
==================
CModel::~CModel
==================
*/
CModel::~CModel()
{
	Destroy();
}

/*
==================
CModel::Init
==================
*/
void CModel::Init( const modelInitialData_t& initialData )
{
	// Insert a fence to make sure that the render thread not using the studio resource
	PROFILER_SCOPE_FUNC();
	CStudioRenderCmdFence& renderCmdFence = pStudioResource->GetRenderCmdFence();
	renderCmdFence.InsertFence();

	// Unsubscribe from material events
	UnsubscribeMaterialEvents();

	// Copy a new materials
	materials.resize( initialData.numMaterials );
	for ( uint32 materialId = 0; materialId < initialData.numMaterials; ++materialId )
	{
		materials[materialId] = initialData.pMaterials[materialId];
	}
	bDirtyMaterials = false;

	// Subscribe on material events
	SubscribeMaterialEvents();

	// Wait the render command fence
	renderCmdFence.Wait();

	// Update the studio resource
	pStudioResource->Update( initialData );
}

/*
==================
CModel::Destroy
==================
*/
void CModel::Destroy()
{
	// Insert a fence to make sure that the render thread not using the studio resource
	PROFILER_SCOPE_FUNC();
	CStudioRenderCmdFence& renderCmdFence = pStudioResource->GetRenderCmdFence();
	renderCmdFence.InsertFence();

	// Unsubscribe from material events and clear material array
	UnsubscribeMaterialEvents();
	materials.clear();
	bDirtyMaterials = false;

	// Wait the render command fence
	renderCmdFence.Wait();

	// Clear the studio resource
	pStudioResource->Clear();
}

/*
==================
CModel::MarkUsedDependencies
==================
*/
void CModel::MarkUsedDependencies()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)materials.size(); index < count; ++index )
	{
		IResource* pResource = materials[index];
		if ( pResource )
		{
			pResource->MarkUsed();
		}
	}
}

/*
==================
CModel::MakePermanentDependencies
==================
*/
void CModel::MakePermanentDependencies()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)materials.size(); index < count; ++index )
	{
		IResource* pResource = materials[index];
		if ( pResource )
		{
			pResource->MakePermanent();
		}
	}
}

/*
==================
CModel::ClearPermanentDependencies
==================
*/
void CModel::ClearPermanentDependencies()
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)materials.size(); index < count; ++index )
	{
		IResource* pResource = materials[index];
		if ( pResource )
		{
			pResource->ClearPermanent();
		}
	}
}

/*
==================
CModel::OnMaterialCachedUncached
==================
*/
void CModel::OnMaterialCachedUncached( void* pUserData, IResource* pResource )
{
	// Insert a fence to the render thread and mark materials as dirty
	CModel* pModel = (CModel*)pUserData;
	pModel->pStudioResource->GetRenderCmdFence().InsertFence();
	pModel->bDirtyMaterials = true;
}

/*
==================
CModel::SubscribeMaterialEvents
==================
*/
void CModel::SubscribeMaterialEvents()
{
	PROFILER_SCOPE_FUNC();
	Assert( materialEventHandles.empty() );

	uint32 numMaterials = (uint32)materials.size();
	materialEventHandles.resize( numMaterials );
	for ( uint32 materialId = 0; materialId < numMaterials; ++materialId )
	{
		IResource*				pResource	 = materials[materialId].GetResource();
		materialEventHandles_t& eventHandles = materialEventHandles[materialId];
		eventHandles.materialId				 = materialId;
		eventHandles.onChachedHandle		 = pResource->OnCached()->Subscribe( &CModel::OnMaterialCachedUncached, this );
		eventHandles.onUncachedHandle		 = pResource->OnUncached()->Subscribe( &CModel::OnMaterialCachedUncached, this );
	}
}

/*
==================
CModel::UnsubscribeMaterialEvents
==================
*/
void CModel::UnsubscribeMaterialEvents()
{
	PROFILER_SCOPE_FUNC();
	Assert( materialEventHandles.size() == materials.size() || materialEventHandles.empty() );
	for ( uint32 eventHandlesId = 0, numEventHandles = (uint32)materialEventHandles.size(); eventHandlesId < numEventHandles; ++eventHandlesId )
	{
		materialEventHandles_t& eventHandles = materialEventHandles[eventHandlesId];
		IResource*				pResource	 = materials[eventHandles.materialId].GetResource();
		pResource->OnCached()->Unsubscribe( eventHandles.onChachedHandle );
		pResource->OnUncached()->Unsubscribe( eventHandles.onUncachedHandle );
	}

	materialEventHandles.clear();
}

/*
==================
CModel::GetNumMaterials
==================
*/
uint32 CModel::GetNumMaterials() const
{
	return (uint32)materials.size();
}

/*
==================
CModel::GetMaterials
==================
*/
const CResourcePtr<IMaterial>* CModel::GetMaterials() const
{
	return materials.data();
}

/*
==================
CModel::GetStudioResource
==================
*/
IModelResource* CModel::GetStudioResource() const
{
	// TODO BS yehor.pohuliaka - Remove it when will be done CIT-43
	if ( bDirtyMaterials )
	{
		// Wait fences to make sure that the render thread not using the studio resource
		pStudioResource->GetRenderCmdFence().Wait();

		// Update materials in the studio resource
		pStudioResource->UpdateMaterials( materials.data(), (uint32)materials.size() );

		// Reset the dirty materials flag
		( (CModel*)this )->bDirtyMaterials = false;
	}
	return pStudioResource;
}
