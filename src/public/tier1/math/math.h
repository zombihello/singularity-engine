#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/template.h"
#include "tier1/math/axis.h"
#include "tier1/math/types.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
extern const vec3_t			  g_vectorZero;
extern const vec3_t			  g_vectorOne;
extern const quat_t			  g_quaternionZero;
extern const class CRotator	  g_rotatorZero;
extern const mat4_t			  g_matrixIdentity;
extern const class CTransform g_transformZero;
extern const vec3_t			  g_vectorForward;
extern const vec3_t			  g_vectorRight;
extern const vec3_t			  g_vectorUp;

//-----------------------------------------------------------------------------
// General math functions
//-----------------------------------------------------------------------------
float  S_RadiansToDegrees( float radians );
float  S_DegreesToRadians( float degrees );
float  S_Pow( float a, float b );
float  S_Log2( float a );
float  S_Loge( float a );
float  S_LogX( float base, float value );
float  S_Trunc( float a );
float  S_Round( float a );
float  S_Sin( float a );
float  S_Cos( float a );
float  S_ASin( float a );
float  S_ATan( float a );
float  S_ATan2( float y, float x );
float  S_Sqrt( float a );
float  S_Floor( float a );
float  S_Clamp( float x, float min, float max );
float  S_Abs( float x );
float  S_Lerp( float x, float y, float a );
bool   S_IsNearlyZero( float value, float errorTolerance = SMALL_NUMBER );
bool   S_IsNearlyZero( const vec2_t& value, float errorTolerance = SMALL_NUMBER );
bool   S_IsNearlyZero( const vec3_t& value, float errorTolerance = SMALL_NUMBER );
bool   S_IsNearlyZero( const vec4_t& value, float errorTolerance = SMALL_NUMBER );
float  S_Fmod( float x, float y );
float  S_GridSnap( float value, float grid );
vec2_t S_GridSnap( const vec2_t& value, float grid );
void   S_GridSnap( const vec2_t& value, float grid, vec2_t& resultValue );
vec3_t S_GridSnap( const vec3_t& value, float grid );
void   S_GridSnap( const vec3_t& value, float grid, vec3_t& resultValue );
vec4_t S_GridSnap( const vec4_t& value, float grid );
void   S_GridSnap( const vec4_t& value, float grid, vec4_t& resultValue );

template<typename TType>
constexpr TType S_MinValue();
template<typename TType>
constexpr TType S_MaxValue();
template<typename TType>
constexpr int32 S_Digits10();
template<typename TType>
constexpr int32 S_MaxDigits10();
template<typename TType>
constexpr bool S_IsFinite( const TType& value );

//-----------------------------------------------------------------------------
// Vector functions
//-----------------------------------------------------------------------------
template<typename TVectorType>
TVectorType	  S_VectorCreate( const char* pString );
void		  S_VectorCreate( const char* pString, vec2_t& vector );
void		  S_VectorCreate( const char* pString, vec3_t& vector );
void		  S_VectorCreate( const char* pString, vec4_t& vector );
vec2_t		  S_VectorCreate( float x, float y );
vec3_t		  S_VectorCreate( float x, float y, float z );
vec4_t		  S_VectorCreate( float x, float y, float z, float w );
void		  S_VectorCreate( float x, float y, vec2_t& vector );
void		  S_VectorCreate( float x, float y, float z, vec3_t& vector );
void		  S_VectorCreate( float x, float y, float z, float w, vec4_t& vector );
eastl::string S_VectorToString( const vec2_t& vector );
eastl::string S_VectorToString( const vec3_t& vector );
eastl::string S_VectorToString( const vec4_t& vector );
void		  S_VectorToString( const vec2_t& vector, eastl::string& result );
void		  S_VectorToString( const vec3_t& vector, eastl::string& result );
void		  S_VectorToString( const vec4_t& vector, eastl::string& result );
float		  S_VectorDotProduct( const vec3_t& x, const vec3_t& y );
vec2_t		  S_VectorNormalize( const vec2_t& vector );
vec3_t		  S_VectorNormalize( const vec3_t& vector );
vec4_t		  S_VectorNormalize( const vec4_t& vector );
float		  S_VectorLength( const vec2_t& vector );
float		  S_VectorLength( const vec3_t& vector );
float		  S_VectorLength( const vec4_t& vector );
void		  S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB, vec3_t& resultVector );
vec3_t		  S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB );
float		  S_VectorDistance( const vec3_t& vectorA, const vec3_t& vectorB );

//-----------------------------------------------------------------------------
// Quaternion functions
//-----------------------------------------------------------------------------
quat_t		  S_QuaternionCreate( const char* pString );
void		  S_QuaternionCreate( const char* pString, quat_t& quaternion );
quat_t		  S_QuaternionCreate( float x, float y, float z, float w );
void		  S_QuaternionCreate( float x, float y, float z, float w, quat_t& quaternion );
eastl::string S_QuaternionToString( const quat_t& quaternion );
void		  S_QuaternionToString( const quat_t& quaternion, eastl::string& result );
quat_t		  S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quat_t		  S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quat_t		  S_AnglesToQuaternionYZX( const vec3_t& eulerAngles );
quat_t		  S_AnglesToQuaternionZYX( const vec3_t& eulerAngles );
vec3_t		  S_QuaternionToAngles( const quat_t& quaternion );
mat4_t		  S_QuaternionToMatrix( const quat_t& quaternion );
void		  S_QuaternionInverse( const quat_t& srcQuaternion, quat_t& destQuaternion );
quat_t		  S_QuaternionInverse( const quat_t& quaternion );
quat_t		  S_QuatenrionLookAt( const vec3_t& lookFrom, const vec3_t& lookTo, const vec3_t& up, const vec3_t& globalUp );
quat_t		  S_QuaternionNormalize( const quat_t& quaternion );

//-----------------------------------------------------------------------------
// Matrix functions
//-----------------------------------------------------------------------------
mat4_t		  S_MatrixCreate( const char* pString );
void		  S_MatrixCreate( const char* pString, mat4_t& matrix );
mat4_t		  S_MatrixCreate( const vec4_t& row0, const vec4_t& row1, const vec4_t& row2, const vec4_t& row3 );
void		  S_MatrixCreate( const vec4_t& row0, const vec4_t& row1, const vec4_t& row2, const vec4_t& row3, mat4_t& matrix );
eastl::string S_MatrixToString( const mat4_t& matrix );
void		  S_MatrixToString( const mat4_t& matrix, eastl::string& result );
void		  S_MatrixIdentity( mat4_t& matrix );
mat4_t		  S_MatrixIdentity();
void		  S_MatrixOrigin( const mat4_t& matrix, vec3_t& origin );
vec3_t		  S_MatrixOrigin( const mat4_t& matrix );
void		  S_MatrixTranslate( const vec3_t& location, mat4_t& matrix );
void		  S_MatrixTranslate( const vec3_t& location, const mat4_t& initMatrix, mat4_t& matrix );
mat4_t		  S_MatrixTranslate( const vec3_t& location, const mat4_t& initMatrix = mat4_t( 1.f ) );
void		  S_MatrixScale( const vec3_t& scale, mat4_t& matrix );
void		  S_MatrixScale( const vec3_t& scale, const mat4_t& initMatrix, mat4_t& matrix );
mat4_t		  S_MatrixScale( const vec3_t& scale, const mat4_t& initMatrix = mat4_t( 1.f ) );
void		  S_MatrixInverse( const mat4_t& srcMatrix, mat4_t& destMatrix );
mat4_t		  S_MatrixInverse( const mat4_t& matrix );
mat4_t		  S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane );
void		  S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane, mat4_t& matrix );
mat4_t		  S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane );
void		  S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane, mat4_t& matrix );
mat4_t		  S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp );
void		  S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp, mat4_t& matrix );

#include "tier1/math/math.inl"
