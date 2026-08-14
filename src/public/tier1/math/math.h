#pragma once
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtx/compatibility.hpp>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier1/template.h"
#include "tier1/math/math_types.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CRotator;
class CTransform;

//-----------------------------------------------------------------------------
// Math constants
//-----------------------------------------------------------------------------
#undef PI
#undef SMALL_NUMBER
#undef KINDA_SMALL_NUMBER
#undef BIG_NUMBER
#undef EULERS_NUMBER

#define PI				   ( 3.1415926535897932 )
#define SMALL_NUMBER	   ( 1.e-8 )
#define KINDA_SMALL_NUMBER ( 1.e-4 )
#define BIG_NUMBER		   ( 3.4e+38f )
#define EULERS_NUMBER	   ( 2.71828182845904523536 )

extern const vector3_t	  g_vector000;
extern const vector3_t	  g_vector100;
extern const vector3_t	  g_vector010;
extern const vector3_t	  g_vector001;
extern const vector3_t	  g_vector111;
extern const vector3_t	  g_vectorForward;
extern const vector3_t	  g_vectorRight;
extern const vector3_t	  g_vectorUp;
extern const CRotator	  g_rotatorIdentity;
extern const CTransform	  g_transformIdentity;
extern const quaternion_t g_quaternionIdentity;
extern const matrix3x3_t  g_matrix33Identity;
extern const matrix3x4_t  g_matrix34Identity;
extern const matrix4x4_t  g_matrix44Identity;
extern const matrix4x3_t  g_matrix43Identity;

//-----------------------------------------------------------------------------
// General math functions
//-----------------------------------------------------------------------------
float	  S_RadiansToDegrees( float radians );
float	  S_DegreesToRadians( float degrees );
float	  S_Pow( float a, float b );
float	  S_Log2( float a );
float	  S_LogE( float a );
float	  S_LogX( float base, float value );
float	  S_Trunc( float a );
float	  S_Round( float a );
float	  S_Sin( float a );
float	  S_Cos( float a );
float	  S_Asin( float a );
float	  S_Atan( float a );
float	  S_Atan2( float y, float x );
float	  S_Sqrt( float a );
float	  S_Floor( float a );
float	  S_Abs( float x );
float	  S_Lerp( float x, float y, float a );
float	  S_Fmod( float x, float y );
bool	  S_IsNearlyZero( float value, float errorTolerance = SMALL_NUMBER );
bool	  S_IsNearlyZero( const vector2_t& value, float errorTolerance = SMALL_NUMBER );
bool	  S_IsNearlyZero( const vector3_t& value, float errorTolerance = SMALL_NUMBER );
bool	  S_IsNearlyZero( const vector4_t& value, float errorTolerance = SMALL_NUMBER );
float	  S_GridSnap( float value, float grid );
vector2_t S_GridSnap( const vector2_t& value, float grid );
void	  S_GridSnap( const vector2_t& value, float grid, vector2_t& result );
vector3_t S_GridSnap( const vector3_t& value, float grid );
void	  S_GridSnap( const vector3_t& value, float grid, vector3_t& result );
vector4_t S_GridSnap( const vector4_t& value, float grid );
void	  S_GridSnap( const vector4_t& value, float grid, vector4_t& result );

template<typename TType>
TType S_Max( const TType& a, const TType& b );
template<typename TType>
TType S_Min( const TType& a, const TType& b );
template<typename TType>
TType S_Align( const TType& value, uint64 alignment );
template<typename TType>
TType S_Clamp( const TType& x, const TType& min, const TType& max );
template<typename TType>
void S_Swap( TType& a, TType& b );
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
TVectorType S_VectorFromString( const char* pString );
template<typename TVectorType>
void S_VectorFromString( const char* pString, TVectorType& vector );
template<typename TVectorType>
eastl::string S_VectorToString( const TVectorType& vector );
template<typename TVectorType>
void S_VectorToString( const TVectorType& vector, eastl::string& result );

template<typename TVectorType>
float S_VectorDotProduct( const TVectorType& x, const TVectorType& y );
template<typename TVectorType>
TVectorType S_VectorNormalize( const TVectorType& vector );
template<typename TVectorType>
void S_VectorNormalize( const TVectorType& vector, TVectorType& result );
template<typename TVectorType>
float S_VectorLength( const TVectorType& vector );
template<typename TVectorType>
void S_VectorCrossProduct( const TVectorType& x, const TVectorType& y, TVectorType& result );
template<typename TVectorType>
TVectorType S_VectorCrossProduct( const TVectorType& x, const TVectorType& y );
template<typename TVectorType>
float S_VectorDistance( const TVectorType& x, const TVectorType& y );

template<typename TVectorBoolType>
bool S_VectorAll( const TVectorBoolType& x );
template<typename TVectorType>
void S_VectorLessThan( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result );
template<typename TVectorType>
typename vectorBoolType_t<TVectorType>::type_t S_VectorLessThan( const TVectorType& x, const TVectorType& y );
template<typename TVectorType>
void S_VectorLessThanEqual( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result );
template<typename TVectorType>
typename vectorBoolType_t<TVectorType>::type_t S_VectorLessThanEqual( const TVectorType& x, const TVectorType& y );
template<typename TVectorType>
void S_VectorGreaterThan( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result );
template<typename TVectorType>
typename vectorBoolType_t<TVectorType>::type_t S_VectorGreaterThan( const TVectorType& x, const TVectorType& y );
template<typename TVectorType>
void S_VectorGreaterThanEqual( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result );
template<typename TVectorType>
typename vectorBoolType_t<TVectorType>::type_t S_VectorGreaterThanEqual( const TVectorType& x, const TVectorType& y );

//-----------------------------------------------------------------------------
// Quaternion functions
//-----------------------------------------------------------------------------
quaternion_t  S_QuaternionFromString( const char* pString );
void		  S_QuaternionFromString( const char* pString, quaternion_t& quaternion );
eastl::string S_QuaternionToString( const quaternion_t& quaternion );
void		  S_QuaternionToString( const quaternion_t& quaternion, eastl::string& result );

void		 S_QuaternionIdentity( quaternion_t& quaternion );
quaternion_t S_QuaternionIdentity();
quaternion_t S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quaternion_t S_AnglesToQuaternionYZX( const vector3_t& eulerAngles );
void		 S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ, quaternion_t& result );
void		 S_AnglesToQuaternionYZX( const vector3_t& eulerAngles, quaternion_t& result );
quaternion_t S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ );
quaternion_t S_AnglesToQuaternionZYX( const vector3_t& eulerAngles );
void		 S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ, quaternion_t& result );
void		 S_AnglesToQuaternionZYX( const vector3_t& eulerAngles, quaternion_t& result );
vector3_t	 S_QuaternionToAngles( const quaternion_t& quaternion );
void		 S_QuaternionToAngles( const quaternion_t& quaternion, vector3_t& result );
template<typename TMatrixType>
TMatrixType S_QuaternionToMatrix( const quaternion_t& quaternion );
template<typename TMatrixType>
void		 S_QuaternionToMatrix( const quaternion_t& quaternion, TMatrixType& result );
quaternion_t S_QuaternionInverse( const quaternion_t& quaternion );
void		 S_QuaternionInverse( const quaternion_t& quaternion, quaternion_t& result );
quaternion_t S_QuatenrionLookAt( const vector3_t& lookFrom, const vector3_t& lookTo, const vector3_t& up, const vector3_t& globalUp );
void		 S_QuatenrionLookAt( const vector3_t& lookFrom, const vector3_t& lookTo, const vector3_t& up, const vector3_t& globalUp, quaternion_t& result );
quaternion_t S_QuaternionNormalize( const quaternion_t& quaternion );
void		 S_QuaternionNormalize( const quaternion_t& quaternion, quaternion_t& result );

//-----------------------------------------------------------------------------
// Matrix functions
//-----------------------------------------------------------------------------
template<typename TMatrixType>
TMatrixType S_MatrixFromString( const char* pString );
template<typename TMatrixType>
void S_MatrixFromString( const char* pString, TMatrixType& matrix );
template<typename TMatrixType>
eastl::string S_MatrixToString( const TMatrixType& matrix );
template<typename TMatrixType>
void S_MatrixToString( const TMatrixType& matrix, eastl::string& result );

template<typename TMatrixType>
TMatrixType S_MatrixIdentity();
template<typename TMatrixType>
void S_MatrixIdentity( TMatrixType& matrix );
template<typename TMatrixType>
void S_MatrixTranslate( const vector3_t& location, TMatrixType& matrix );
template<typename TMatrixType>
TMatrixType S_MatrixTranslate( const vector3_t& location );
template<typename TMatrixType>
void S_MatrixScale( const vector3_t& scale, TMatrixType& matrix );
template<typename TMatrixType>
TMatrixType S_MatrixScale( const vector3_t& scale );
template<typename TMatrixType>
void S_MatrixInverse( const TMatrixType& srcMatrix, TMatrixType& destMatrix );
template<typename TMatrixType>
TMatrixType S_MatrixInverse( const TMatrixType& matrix );
template<typename TMatrixType>
void S_MatrixTranspose( const TMatrixType& srcMatrix, typename matrixTransposeType_t<TMatrixType>::type_t& destMatrix );
template<typename TMatrixType>
typename matrixTransposeType_t<TMatrixType>::type_t S_MatrixTranspose( const TMatrixType& matrix );

matrix4x4_t S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane );
void		S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane, matrix4x4_t& matrix );
matrix4x4_t S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane );
void		S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane, matrix4x4_t& matrix );
matrix4x4_t S_MatrixLookAt( const vector3_t& location, const vector3_t& direction, const vector3_t& axisUp );
void		S_MatrixLookAt( const vector3_t& location, const vector3_t& direction, const vector3_t& axisUp, matrix4x4_t& matrix );

#include "tier1/math/math.inl"
