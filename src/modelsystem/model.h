#pragma once
#include "studiorender/studioapi/istudioapi_buffer.h"
#include "studiorender/istudio_renderresource.h"
#include "modelsystem/imodel.h"
#include "modelsystem/vertexfactory_base.h"

//-----------------------------------------------------------------------------
// A model resource which is owned by the render thread
//-----------------------------------------------------------------------------
class CModelResource : public CDebugNamed<CRefCounted<IModelResource>>, public CStudioRenderResource<IStudioRenderResource>
{
public:
	// IModelResource interface
	virtual uint32							  GetNumMaterials() const override;
	virtual const CRefPtr<IMaterialResource>* GetMaterials() const override;
	virtual uint32							  GetNumSurfaces() const override;
	virtual const modelSurface_t*			  GetSurfaces() const override;
	virtual IVertexFactory*					  GetVertexFactory() const override;

	CModelResource( const char* pDebugName = "" );

	void				   Update( const modelInitialData_t& initialData );
	void				   UpdateMaterials( const CResourcePtr<IMaterial>* pMaterials, uint32 numMaterials );
	void				   Clear();
	CStudioRenderCmdFence& GetRenderCmdFence();

protected:
	// IRefCounted interface
	virtual void FinalRelease() override;

private:
	// IStudioRenderResource interface
	virtual void InitStudioAPI() override;
	virtual void ReleaseStudioAPI() override;

	modelIndexType_t						  indexType;
	CStudioRenderCmdFence					  renderCmdFence;
	CRefPtr<IVertexFactory>					  pVertexFactory;
	CRefPtr<IStudioAPIBuffer>				  pStudioAPIVertexBuffer;
	CRefPtr<IStudioAPIBuffer>				  pStudioAPIIndexBuffer;
	eastl::vector<byte>						  vertices;
	eastl::vector<byte>						  indices;
	eastl::vector<CRefPtr<IMaterialResource>> materials;
	eastl::vector<modelSurface_t>			  surfaces;
};

//-----------------------------------------------------------------------------
// A model
//-----------------------------------------------------------------------------
class CModel : public CResourceData<IModel>
{
public:
	// IResourceData interface
	// Collect all resources this data depends on
	virtual void CollectDependencies( IResourceDependencyCollector* pCollector ) const override;

	// IModel interface
	virtual void Init( const modelInitialData_t& initialData ) override;
	virtual void Destroy() override;

	virtual uint32						   GetNumMaterials() const override;
	virtual const CResourcePtr<IMaterial>* GetMaterials() const override;
	virtual IModelResource*				   GetStudioResource() const override;

	CModel( IResource* pResource );
	~CModel();

	// Applies the pending changes to the studio resource
	// NOTE: Called by `CModelSystem` while it drains its pending list on `IStudioRender::OnBeginFrame`
	void UpdateStudioResource();

private:
	struct materialEventHandles_t
	{
		uint32							 materialId;
		IResource::IOnCached::handle_t	 onChachedHandle;
		IResource::IOnUncached::handle_t onUncachedHandle;
	};

	static void OnMaterialCachedUncached( void* pUserData, IResource* pResource );
	void		ScheduleUpdateStudioResource();
	void		CancelUpdateStudioResource();
	void		SubscribeMaterialEvents();
	void		UnsubscribeMaterialEvents();

	bool								   bPendingUpdateStudioResource;
	CRefPtr<CModelResource>				   pStudioResource;
	eastl::vector<CResourcePtr<IMaterial>> materials;
	eastl::vector<materialEventHandles_t>  materialEventHandles;
};

DECLARE_RESOURCE_TYPE( CModel, RESOURCE_TYPE_MODEL );
#include "modelsystem\model.inl"
