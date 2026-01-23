#pragma once

//-----------------------------------------------------------------------------
// STEX cube face index
//-----------------------------------------------------------------------------
enum stexCubeFaceIndex_t
{
	STEX_CUBE_FACE_RIGHT,  // Positive X
	STEX_CUBE_FACE_LEFT,   // Negative X
	STEX_CUBE_FACE_UP,	   // Positive Y
	STEX_CUBE_FACE_DOWN,   // Negative Y
	STEX_CUBE_FACE_FRONT,  // Positive Z
	STEX_CUBE_FACE_BACK,   // Negative Z
	STEX_CUBE_FACE_COUNT   // Cube face count
};