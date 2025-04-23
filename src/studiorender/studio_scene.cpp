#include "pch_studiorender.h"
#include "studiorender/studio_scene.h"

/*
==================
CStudioScene::SetView
==================
*/
void CStudioScene::SetView( const studioSceneCameraView_t& cameraView )
{
	// Calculate a view matrix
	vec3_t		targetDirection = cameraView.rotation * g_vectorForward;
	vec3_t		axisUp			= cameraView.rotation * g_vectorUp;
	S_MatrixLookAt( cameraView.location, cameraView.location + targetDirection, axisUp, sceneView.viewMatrix );

	// Calculate a perspective matrix
	S_MatrixPerspective( cameraView.fieldOfView, cameraView.aspectRatio, cameraView.nearClipPlane, cameraView.farClipPlane, sceneView.projectionMatrix );
}