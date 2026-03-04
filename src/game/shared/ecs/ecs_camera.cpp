#include "pch_game_shared.h"
#include "appframework/iwindowmgr.h"
#include "studiorender/istudiorender.h"
#include "game/shared/ecs/ecs_core.h"
#include "game/shared/ecs/ecs_camera.gen.h"

/*
==================
CEcsSystemCameraInit::OnUpdate
==================
*/
void CEcsSystemCameraInit::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsResourceWindowMgr_t& windowMgr, ecsComponentCamera_t& camera )
{
	if ( camera.bAutoViewData )
	{
		// TODO BS yehor.pohuliaka - Implement handle resize the window
		ivec2_t	 size;
		IWindow* pMainWindow = windowMgr.pWindowMgr->GetOrCreateMainWindow();
		pMainWindow->GetSize( size );
		camera.aspectRatio = (float)size.x / size.y;
	}

	ecsWorld.AddComponent<ecsComponentCameraInited_t>( entity );
}

/*
==================
CEcsSystemUpdateCameraView::OnUpdate
==================
*/
void CEcsSystemUpdateCameraView::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentTransform_t& transform, const ecsComponentCamera_t& camera, const ecsResourceStudioRender_t& studioRender )
{
	studioCameraView_t studioCameraView = {};
	studioCameraView.location			= transform.transform.GetLocation();
	studioCameraView.rotation			= transform.transform.GetRotation();
	studioCameraView.fieldOfView		= camera.fieldOfView;
	studioCameraView.nearClipPlane		= camera.nearClipPlane;
	studioCameraView.farClipPlane		= camera.farClipPlane;
	studioCameraView.aspectRatio		= camera.aspectRatio;
	studioRender.pStudioRender->SetCameraView( studioCameraView );
}
