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
// AHTUNG
#if 0
	if ( camera.bAutoViewData )
	{
		// TODO BS yehor.pohuliaka - Implement handle resize the window
		uint32 windowWidth;
		uint32 windowHeight;
		windowMgr.pWindowMgr->GetSize( windowWidth, windowHeight );
		camera.aspectRatio = (float)windowWidth / windowHeight;
	}

	ecsWorld.AddComponent<ecsComponentCameraInited_t>( entity );
#endif	// 0
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
