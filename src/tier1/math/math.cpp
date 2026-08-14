#include "pch_tier1.h"
#include "tier1/math/rotator.h"
#include "tier1/math/transform.h"
#include "tier1/math/math.h"

const vector3_t	   g_vector000( 0.f, 0.f, 0.f );
const vector3_t	   g_vector100( 1.f, 0.f, 0.f );
const vector3_t	   g_vector010( 0.f, 1.f, 0.f );
const vector3_t	   g_vector001( 0.f, 0.f, 1.f );
const vector3_t	   g_vector111( 1.f, 1.f, 1.f );
const vector3_t	   g_vectorForward( 0.f, 0.f, 1.f );
const vector3_t	   g_vectorRight( 1.f, 0.f, 0.f );
const vector3_t	   g_vectorUp( 0.f, 1.f, 0.f );
const CRotator	   g_rotatorIdentity( 0.f, 0.f, 0.f );
const CTransform   g_transformIdentity;
const quaternion_t g_quaternionIdentity( 1.f, 0.f, 0.f, 0.f );
const matrix3x3_t  g_matrix33Identity( 1.f );
const matrix3x4_t  g_matrix34Identity( 1.f );
const matrix4x4_t  g_matrix44Identity( 1.f );
const matrix4x3_t  g_matrix43Identity( 1.f );
