#ifndef STUDIO_SCENE_H
#define STUDIO_SCENE_H

#include "studiorender/istudio_scene.h"

//-----------------------------------------------------------------------------
// Studio scene view
//-----------------------------------------------------------------------------
struct studioSceneView_t
{
	matrix_t	viewMatrix;
	matrix_t	projectionMatrix;
};


//-----------------------------------------------------------------------------
// Studio scene
//-----------------------------------------------------------------------------
class CStudioScene : public TRefCounted<IStudioScene>
{
public:
	virtual void SetView( const studioSceneCameraView_t& cameraView ) override;

private:
	studioSceneView_t		sceneView;
};

#endif // !STUDIO_SCENE_H