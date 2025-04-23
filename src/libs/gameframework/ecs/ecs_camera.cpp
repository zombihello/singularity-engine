#include "pch_gameframework.h"
#include "studiorender/studio_scene.h"
#include "studiorender/istudiorender.h"
#include "gameframework/ecs/ecs.h"
#include "gameframework/ecs/ecs_camera.gen.h"

/*
==================
CEcsSystemCameraMovement::OnUpdate
==================
*/
void CEcsSystemCameraMovement::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, ecsComponentTransform_t& transform )
{}

/*
==================
CEcsSystemCameraSendToRender::OnUpdate
==================
*/
void CEcsSystemCameraSendToRender::OnUpdate( CEcsWorld ecsWorld, ecsEntity_t entity, const ecsComponentTransform_t& transform, const ecsComponentCamera_t& camera )
{
	studioSceneCameraView_t				studioSceneCameraView = {};
	studioSceneCameraView.location		= transform.transform.GetLocation();
	studioSceneCameraView.rotation		= transform.transform.GetRotation();
	studioSceneCameraView.fieldOfView	= camera.fieldOfView;
	studioSceneCameraView.nearClipPlane = camera.nearClipPlane;
	studioSceneCameraView.farClipPlane	= camera.farClipPlane;
	studioSceneCameraView.aspectRatio	= camera.aspectRatio;
}