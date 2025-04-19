#include "pch_stdlib.h"
#include "stdlib/math/rotator.h"
#include "stdlib/math/transform.h"
#include "stdlib/math/math.h"

const vec3_t			g_vectorZero( 0.f, 0.f, 0.f );
const vec3_t			g_vectorOne( 1.f, 1.f, 1.f );
const quat_t			g_quaternionZero( S_AnglesToQuaternionYZX( 0.f, 0.f, 0.f ) );
const CRotator			g_rotatorZero( 0.f, 0.f, 0.f );
const matrix_t			g_matrixIdentity( 1.f );
const CTransform		g_transformZero( g_quaternionZero, g_vectorOne, g_vectorOne );
const vec3_t			g_vectorForward( 0.f, 0.f, 1.f );
const vec3_t			g_vectorRight( 1.f, 0.f, 0.f );
const vec3_t			g_vectorUp( 0.f, 1.f, 0.f );