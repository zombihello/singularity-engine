#ifndef ISTUDIO_SCENE_H
#define ISTUDIO_SCENE_H

#include "stdlib/math/math.h"
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Studio scene view
//-----------------------------------------------------------------------------
struct studioSceneCameraView_t
{
	vec3_t	location;
	quat_t	rotation;
	float	fieldOfView;
	float	nearClipPlane;
	float	farClipPlane;
	float	aspectRatio;
};


//-----------------------------------------------------------------------------
// Studio scene interface
//-----------------------------------------------------------------------------
class IStudioScene : public IRefCounted
{
public:
	virtual void SetView( const studioSceneCameraView_t& cameraView ) = 0;
};

#endif // !ISTUDIO_SCENE_H