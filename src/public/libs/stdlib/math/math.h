#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtx/compatibility.hpp>

#include "stdlib/template.h"
#include "stdlib/math/axis.h"
#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Math constants
//-----------------------------------------------------------------------------
#undef PI
#undef SMALL_NUMBER
#undef KINDA_SMALL_NUMBER
#undef BIG_NUMBER
#undef EULERS_NUMBER

#define PI 					(3.1415926535897932)
#define SMALL_NUMBER		(1.e-8)
#define KINDA_SMALL_NUMBER	(1.e-4)
#define BIG_NUMBER			(3.4e+38f)
#define EULERS_NUMBER       (2.71828182845904523536)


//-----------------------------------------------------------------------------
// Math types
//-----------------------------------------------------------------------------
typedef glm::vec2		vec2_t;
typedef glm::ivec2		ivec2_t;
typedef glm::vec3		vec3_t;
typedef glm::ivec3		ivec3_t;
typedef glm::vec4		vec4_t;
typedef glm::ivec4		ivec4_t;
typedef glm::mat4		matrix_t;
typedef glm::quat		quat_t;


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern const vec3_t				g_vectorZero;
extern const vec3_t				g_vectorOne;
extern const quat_t				g_quaternionZero;
extern const class CRotator		g_rotatorZero;
extern const matrix_t			g_matrixIdentity;
extern const class CTransform	g_transformZero;
extern const vec3_t				g_vectorForward;
extern const vec3_t				g_vectorRight;
extern const vec3_t				g_vectorUp;


//-----------------------------------------------------------------------------
// General math functions
//-----------------------------------------------------------------------------
float S_RadiansToDegrees( float radians );
float S_DegreesToRadians( float degrees );
float S_Pow( float a, float b );
float S_Log2( float a );
float S_Loge( float a );
float S_LogX( float base, float value );
float S_Trunc( float a );
float S_Round( float a );
float S_Sin( float a );
float S_Cos( float a );
float S_ASin( float a );
float S_ATan( float a );
float S_ATan2( float y, float x );
float S_Sqrt( float a );
float S_Floor( float a );
float S_Clamp( float x, float min, float max );
float S_Abs( float x );
float S_Lerp( float x, float y, float a );
bool S_IsNearlyZero( float value, float errorTolerance = SMALL_NUMBER );
bool S_IsNearlyZero( const vec2_t& value, float errorTolerance = SMALL_NUMBER );
bool S_IsNearlyZero( const vec3_t& value, float errorTolerance = SMALL_NUMBER );
bool S_IsNearlyZero( const vec4_t& value, float errorTolerance = SMALL_NUMBER );
float S_Fmod( float x, float y );
float S_GridSnap( float value, float grid );
vec2_t S_GridSnap( const vec2_t& value, float grid );
void S_GridSnap( const vec2_t& value, float grid, vec2_t& resultValue );
vec3_t S_GridSnap( const vec3_t& value, float grid );
void S_GridSnap( const vec3_t& value, float grid, vec3_t& resultValue );
vec4_t S_GridSnap( const vec4_t& value, float grid );
void S_GridSnap( const vec4_t& value, float grid, vec4_t& resultValue );


//-----------------------------------------------------------------------------
// Vector functions
//-----------------------------------------------------------------------------
float S_VectorDotProduct( const vec3_t& x, const vec3_t& y );
vec2_t S_VectorNormalize( const vec2_t& vector );
vec3_t S_VectorNormalize( const vec3_t& vector );
vec4_t S_VectorNormalize( const vec4_t& vector );
float S_VectorLength( const vec2_t& vector );
float S_VectorLength( const vec3_t& vector );
float S_VectorLength( const vec4_t& vector );
void S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB, vec3_t& resultVector );
vec3_t S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB );
float S_VectorDistance( const vec3_t& vectorA, const vec3_t& vectorB );


//-----------------------------------------------------------------------------
// Quaternion functions
//-----------------------------------------------------------------------------
quat_t S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quat_t S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quat_t S_AnglesToQuaternionYZX( const vec3_t& eulerAngles );
quat_t S_AnglesToQuaternionZYX( const vec3_t& eulerAngles );
vec3_t S_QuaternionToAngles( const quat_t& quaternion );
matrix_t S_QuaternionToMatrix( const quat_t& quaternion );
void S_QuaternionInverse( const quat_t& srcQuaternion, quat_t& destQuaternion );
quat_t S_QuaternionInverse( const quat_t& quaternion );
quat_t S_QuatenrionLookAt( const vec3_t& lookFrom, const vec3_t& lookTo, const vec3_t& up, const vec3_t& globalUp );
quat_t S_QuaternionNormalize( const quat_t& quaternion );


//-----------------------------------------------------------------------------
// Matrix functions
//-----------------------------------------------------------------------------
void S_MatrixIdentity( matrix_t& matrix );
matrix_t S_MatrixIdentity();
void S_MatrixOrigin( const matrix_t& matrix, vec3_t& origin );
vec3_t S_MatrixOrigin( const matrix_t& matrix );
void S_MatrixTranslate( const vec3_t& location, matrix_t& matrix );
void S_MatrixTranslate( const vec3_t& location, const matrix_t& initMatrix, matrix_t& matrix );
matrix_t S_MatrixTranslate( const vec3_t& location, const matrix_t& initMatrix = matrix_t( 1.f ) );
void S_MatrixScale( const vec3_t& scale, matrix_t& matrix );
void S_MatrixScale( const vec3_t& scale, const matrix_t& initMatrix, matrix_t& matrix );
matrix_t S_MatrixScale( const vec3_t& scale, const matrix_t& initMatrix = matrix_t( 1.f ) );
void S_MatrixInverse( const matrix_t& srcMatrix, matrix_t& destMatrix );
matrix_t S_MatrixInverse( const matrix_t& matrix );
matrix_t S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane );
void S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane, matrix_t& matrix );
matrix_t S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane );
void S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane, matrix_t& matrix );
matrix_t S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp );
void S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp, matrix_t& matrix );

#include "stdlib/math/math.inl"