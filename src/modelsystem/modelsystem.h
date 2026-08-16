#pragma once
#include "studiorender/istudiorender.h"
#include "modelsystem/imodelsystem.h"
#include "modelsystem/model.h"
#include "modelsystem/model_loader.h"

//-----------------------------------------------------------------------------
// Model system
//-----------------------------------------------------------------------------
class CModelSystem : public CBaseAppSystem<IModelSystem>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// Initialize and shutdown
	virtual bool Init() override;
	virtual void Shutdown() override;

	// IModelSystem interface
	virtual CRefPtr<IVertexFactory> CreateVertexFactory( modelVertexType_t vertexType, const char* pDebugName = "" ) const override;

	CModelSystem();

	// Add and remove a model whose studio resource has to be updated
	void AddPendingUpdateModel( CModel* pModel );
	void RemovePendingUpdateModel( CModel* pModel );

private:
	static void OnStudioRenderBeginFrame( void* pUserData );

	CResourceTypeFactory<CModel>		   modelFactory;
	CModelLoader						   modelLoader;
	eastl::vector<CModel*>				   pendingUpdateModels;
	IStudioRender::IOnBeginFrame::handle_t onStudioRenderBeginFrameHandle;
};

extern CModelSystem g_modelSystem;
#include "modelsystem/modelsystem.inl"
