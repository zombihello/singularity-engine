#pragma once

/*
==================
S_RadiansToDegrees
==================
*/
FORCEINLINE float S_RadiansToDegrees( float radians )
{
	return glm::degrees( radians );
}

/*
==================
S_DegreesToRadians
==================
*/
FORCEINLINE float S_DegreesToRadians( float degrees )
{
	return glm::radians( degrees );
}

/*
==================
S_Pow
==================
*/
FORCEINLINE float S_Pow( float a, float b )
{
	return glm::pow( a, b );
}

/*
==================
S_Log2
==================
*/
FORCEINLINE float S_Log2( float a )
{
	return glm::log2( a );
}

/*
==================
S_LogE
==================
*/
FORCEINLINE float S_LogE( float a )
{
	return glm::log( a );
}

/*
==================
S_LogX
==================
*/
FORCEINLINE float S_LogX( float base, float value )
{
	return S_LogE( value ) / S_LogE( base );
}

/*
==================
S_Trunc
==================
*/
FORCEINLINE float S_Trunc( float a )
{
	return glm::trunc( a );
}

/*
==================
S_Round
==================
*/
FORCEINLINE float S_Round( float a )
{
	return glm::round( a );
}

/*
==================
S_Sin
==================
*/
FORCEINLINE float S_Sin( float a )
{
	return glm::sin( a );
}

/*
==================
S_Cos
==================
*/
FORCEINLINE float S_Cos( float a )
{
	return glm::cos( a );
}

/*
==================
S_Asin
==================
*/
FORCEINLINE float S_Asin( float a )
{
	return glm::asin( a );
}

/*
==================
S_Atan
==================
*/
FORCEINLINE float S_Atan( float a )
{
	return glm::atan( a );
}

/*
==================
S_Atan2
==================
*/
FORCEINLINE float S_Atan2( float y, float x )
{
	return glm::atan( y, x );
}

/*
==================
S_Sqrt
==================
*/
FORCEINLINE float S_Sqrt( float a )
{
	return glm::sqrt( a );
}

/*
==================
S_Floor
==================
*/
FORCEINLINE float S_Floor( float a )
{
	return glm::floor( a );
}

/*
==================
S_Abs
==================
*/
FORCEINLINE float S_Abs( float x )
{
	return glm::abs( x );
}

/*
==================
S_Lerp
==================
*/
FORCEINLINE float S_Lerp( float x, float y, float a )
{
	return glm::lerp( x, y, a );
}

/*
==================
S_Fmod
==================
*/
FORCEINLINE float S_Fmod( float x, float y )
{
	return fmod( x, y );
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( float value, float errorTolerance /*/*= SMALL_NUMBER*/ )
{
	return S_Abs( value ) < errorTolerance;
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( const vector2_t& value, float errorTolerance /*/*= SMALL_NUMBER*/ )
{
	return S_IsNearlyZero( value.x, errorTolerance ) && S_IsNearlyZero( value.y, errorTolerance );
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( const vector3_t& value, float errorTolerance /*= SMALL_NUMBER*/ )
{
	return S_IsNearlyZero( value.x, errorTolerance ) && S_IsNearlyZero( value.y, errorTolerance ) && S_IsNearlyZero( value.z, errorTolerance );
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( const vector4_t& value, float errorTolerance /*= SMALL_NUMBER*/ )
{
	return S_IsNearlyZero( value.x, errorTolerance ) && S_IsNearlyZero( value.y, errorTolerance ) && S_IsNearlyZero( value.z, errorTolerance ) && S_IsNearlyZero( value.w, errorTolerance );
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE float S_GridSnap( float value, float grid )
{
	return grid != 0.f ? S_Floor( ( value + 0.5f * grid ) / grid ) * grid : value;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vector2_t S_GridSnap( const vector2_t& value, float grid )
{
	vector2_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vector2_t& value, float grid, vector2_t& result )
{
	result.x = S_GridSnap( value.x, grid );
	result.y = S_GridSnap( value.y, grid );
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vector3_t S_GridSnap( const vector3_t& value, float grid )
{
	vector3_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vector3_t& value, float grid, vector3_t& result )
{
	result.x = S_GridSnap( value.x, grid );
	result.y = S_GridSnap( value.y, grid );
	result.z = S_GridSnap( value.z, grid );
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vector4_t S_GridSnap( const vector4_t& value, float grid )
{
	vector4_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vector4_t& value, float grid, vector4_t& result )
{
	result.x = S_GridSnap( value.x, grid );
	result.y = S_GridSnap( value.y, grid );
	result.z = S_GridSnap( value.z, grid );
	result.w = S_GridSnap( value.w, grid );
}

/*
==================
S_Max
==================
*/
template<typename TType>
FORCEINLINE TType S_Max( const TType& a, const TType& b )
{
	return ( a >= b ) ? a : b;
}

/*
==================
S_Min
==================
*/
template<typename TType>
FORCEINLINE TType S_Min( const TType& a, const TType& b )
{
	return ( a <= b ) ? a : b;
}

/*
==================
S_Align
==================
*/
template<typename TType>
FORCEINLINE TType S_Align( const TType& value, uint64 alignment )
{
	return (TType)( ( (ptrint)value + alignment - 1 ) & ~( (ptrint)alignment - 1 ) );
}

/*
==================
S_Clamp
==================
*/
template<typename TType>
FORCEINLINE TType S_Clamp( const TType& x, const TType& min, const TType& max )
{
	return S_Min( S_Max( x, min ), max );
}

/*
==================
S_Swap
==================
*/
template<typename TType>
FORCEINLINE void S_Swap( TType& a, TType& b )
{
	TType tmp = eastl::move( a );
	a		  = eastl::move( b );
	b		  = eastl::move( tmp );
}

/*
==================
S_MinValue
==================
*/
template<typename TType>
constexpr FORCEINLINE TType S_MinValue()
{
	return eastl::numeric_limits<TType>::min();
}

/*
==================
S_MaxValue
==================
*/
template<typename TType>
constexpr FORCEINLINE TType S_MaxValue()
{
	return eastl::numeric_limits<TType>::max();
}

/*
==================
S_Digits10
==================
*/
template<typename TType>
constexpr FORCEINLINE int32 S_Digits10()
{
	return eastl::numeric_limits<TType>::digits10;
}

/*
==================
S_MaxDigits10
==================
*/
template<typename TType>
constexpr FORCEINLINE int32 S_MaxDigits10()
{
	return eastl::numeric_limits<TType>::max_digits10;
}

/*
==================
S_IsFinite
==================
*/
template<typename TType>
constexpr FORCEINLINE bool S_IsFinite( const TType& value )
{
	return glm::isfinite( value );
}

/*
==================
S_VectorFromString
==================
*/
template<typename TVectorType>
FORCEINLINE TVectorType S_VectorFromString( const char* pString )
{
	TVectorType result;
	S_VectorFromString( pString, result );
	return result;
}

/*
==================
S_VectorFromString
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorFromString( const char* pString, TVectorType& vector )
{
	static_assert( false, "Unknown type to create a vector from a string" );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector2_t>( const char* pString, vector2_t& vector )
{
	S_Sscanf( pString, "%f %f", &vector.x, &vector.y );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector2i_t>( const char* pString, vector2i_t& vector )
{
	S_Sscanf( pString, "%i %i", &vector.x, &vector.y );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector2b_t>( const char* pString, vector2b_t& vector )
{
	int32 x = 0;
	int32 y = 0;
	S_Sscanf( pString, "%i %i", &x, &y );
	vector.x = x > 0;
	vector.y = y > 0;
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector3_t>( const char* pString, vector3_t& vector )
{
	S_Sscanf( pString, "%f %f %f", &vector.x, &vector.y, &vector.z );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector3i_t>( const char* pString, vector3i_t& vector )
{
	S_Sscanf( pString, "%i %i %i", &vector.x, &vector.y, &vector.z );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector3b_t>( const char* pString, vector3b_t& vector )
{
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	S_Sscanf( pString, "%i %i %i", &x, &y, &z );
	vector.x = x > 0;
	vector.y = y > 0;
	vector.z = z > 0;
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector4_t>( const char* pString, vector4_t& vector )
{
	S_Sscanf( pString, "%f %f %f %f", &vector.x, &vector.y, &vector.z, &vector.w );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector4i_t>( const char* pString, vector4i_t& vector )
{
	S_Sscanf( pString, "%i %i %i %i", &vector.x, &vector.y, &vector.z, &vector.w );
}

/*
==================
S_VectorFromString
==================
*/
template<>
FORCEINLINE void S_VectorFromString<vector4b_t>( const char* pString, vector4b_t& vector )
{
	int32 x = 0;
	int32 y = 0;
	int32 z = 0;
	int32 w = 0;
	S_Sscanf( pString, "%i %i %i %i", &x, &y, &z, &w );
	vector.x = x > 0;
	vector.y = y > 0;
	vector.z = z > 0;
	vector.w = w > 0;
}

/*
==================
S_VectorToString
==================
*/
template<typename TVectorType>
FORCEINLINE eastl::string S_VectorToString( const TVectorType& vector )
{
	eastl::string result;
	S_VectorToString( vector, result );
	return result;
}

/*
==================
S_VectorToString
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorToString( const TVectorType& vector, eastl::string& result )
{
	static_assert( false, "Unknown type to create a string from a vector" );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector2_t>( const vector2_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f", vector.x, vector.y );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector2i_t>( const vector2i_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i", vector.x, vector.y );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector2b_t>( const vector2b_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i", vector.x, vector.y );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector3_t>( const vector3_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f", vector.x, vector.y, vector.z );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector3i_t>( const vector3i_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i %i", vector.x, vector.y, vector.z );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector3b_t>( const vector3b_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i %i", vector.x, vector.y, vector.z );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector4_t>( const vector4_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f %f", vector.x, vector.y, vector.z, vector.w );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector4i_t>( const vector4i_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i %i %i", vector.x, vector.y, vector.z, vector.w );
}

/*
==================
S_VectorToString
==================
*/
template<>
FORCEINLINE void S_VectorToString<vector4b_t>( const vector4b_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%i %i %i %i", vector.x, vector.y, vector.z, vector.w );
}

/*
==================
S_VectorDotProduct
==================
*/
template<typename TVectorType>
FORCEINLINE float S_VectorDotProduct( const TVectorType& x, const TVectorType& y )
{
	return glm::dot( x, y );
}

/*
==================
S_VectorNormalize
==================
*/
template<typename TVectorType>
FORCEINLINE TVectorType S_VectorNormalize( const TVectorType& vector )
{
	return glm::normalize( vector );
}

/*
==================
S_VectorNormalize
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorNormalize( const TVectorType& vector, TVectorType& result )
{
	result = S_VectorNormalize( vector );
}

/*
==================
S_VectorLength
==================
*/
template<typename TVectorType>
FORCEINLINE float S_VectorLength( const TVectorType& vector )
{
	return glm::length( vector );
}

/*
==================
S_VectorCrossProduct
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorCrossProduct( const TVectorType& x, const TVectorType& y, TVectorType& result )
{
	result = glm::cross( x, y );
}

/*
==================
S_VectorCrossProduct
==================
*/
template<typename TVectorType>
FORCEINLINE TVectorType S_VectorCrossProduct( const TVectorType& x, const TVectorType& y )
{
	TVectorType result;
	S_VectorCrossProduct( x, y, result );
	return result;
}

/*
==================
S_VectorDistance
==================
*/
template<typename TVectorType>
FORCEINLINE float S_VectorDistance( const TVectorType& x, const TVectorType& y )
{
	return glm::distance( x, y );
}

/*
==================
S_VectorAll
==================
*/
template<typename TVectorBoolType>
FORCEINLINE bool S_VectorAll( const TVectorBoolType& x )
{
	return glm::all( x );
}

/*
==================
S_VectorLessThan
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorLessThan( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result )
{
	result = glm::lessThan( x, y );
}

/*
==================
S_VectorLessThan
==================
*/
template<typename TVectorType>
FORCEINLINE typename vectorBoolType_t<TVectorType>::type_t S_VectorLessThan( const TVectorType& x, const TVectorType& y )
{
	typename vectorBoolType_t<TVectorType>::type_t result;
	S_VectorLessThan( x, y, result );
	return result;
}

/*
==================
S_VectorLessThanEqual
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorLessThanEqual( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result )
{
	result = glm::lessThanEqual( x, y );
}

/*
==================
S_VectorLessThanEqual
==================
*/
template<typename TVectorType>
FORCEINLINE typename vectorBoolType_t<TVectorType>::type_t S_VectorLessThanEqual( const TVectorType& x, const TVectorType& y )
{
	typename vectorBoolType_t<TVectorType>::type_t result;
	S_VectorLessThanEqual( x, y, result );
	return result;
}

/*
==================
S_VectorGreaterThan
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorGreaterThan( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result )
{
	result = glm::greaterThan( x, y );
}

/*
==================
S_VectorGreaterThan
==================
*/
template<typename TVectorType>
FORCEINLINE typename vectorBoolType_t<TVectorType>::type_t S_VectorGreaterThan( const TVectorType& x, const TVectorType& y )
{
	typename vectorBoolType_t<TVectorType>::type_t result;
	S_VectorGreaterThan( x, y, result );
	return result;
}

/*
==================
S_VectorGreaterThanEqual
==================
*/
template<typename TVectorType>
FORCEINLINE void S_VectorGreaterThanEqual( const TVectorType& x, const TVectorType& y, typename vectorBoolType_t<TVectorType>::type_t& result )
{
	result = glm::greaterThanEqual( x, y );
}

/*
==================
S_VectorGreaterThanEqual
==================
*/
template<typename TVectorType>
FORCEINLINE typename vectorBoolType_t<TVectorType>::type_t S_VectorGreaterThanEqual( const TVectorType& x, const TVectorType& y )
{
	typename vectorBoolType_t<TVectorType>::type_t result;
	S_VectorGreaterThanEqual( x, y, result );
	return result;
}

/*
==================
S_QuaternionFromString
==================
*/
FORCEINLINE quaternion_t S_QuaternionFromString( const char* pString )
{
	quaternion_t result;
	S_QuaternionFromString( pString, result );
	return result;
}

/*
==================
S_QuaternionFromString
==================
*/
FORCEINLINE void S_QuaternionFromString( const char* pString, quaternion_t& quaternion )
{
	S_Sscanf( pString, "%f %f %f %f", &quaternion.x, &quaternion.y, &quaternion.z, &quaternion.w );
}

/*
==================
S_QuaternionToString
==================
*/
FORCEINLINE eastl::string S_QuaternionToString( const quaternion_t& quaternion )
{
	eastl::string result;
	S_QuaternionToString( quaternion, result );
	return result;
}

/*
==================
S_QuaternionToString
==================
*/
FORCEINLINE void S_QuaternionToString( const quaternion_t& quaternion, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f %f", quaternion.x, quaternion.y, quaternion.z, quaternion.w );
}

/*
==================
S_QuaternionIdentity
==================
*/
FORCEINLINE void S_QuaternionIdentity( quaternion_t& quaternion )
{
	quaternion = glm::identity<quaternion_t>();
}

/*
==================
S_QuaternionIdentity
==================
*/
FORCEINLINE quaternion_t S_QuaternionIdentity()
{
	return glm::identity<quaternion_t>();
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE quaternion_t S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
{
	quaternion_t result;
	S_AnglesToQuaternionYZX( eulerAngleX, eulerAngleY, eulerAngleZ, result );
	return result;
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE quaternion_t S_AnglesToQuaternionYZX( const vector3_t& eulerAngles )
{
	quaternion_t result;
	S_AnglesToQuaternionYZX( eulerAngles.x, eulerAngles.y, eulerAngles.z, result );
	return result;
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE void S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ, quaternion_t& result )
{
	result =
		glm::angleAxis( S_DegreesToRadians( eulerAngleY ), g_vector010 ) * glm::angleAxis( S_DegreesToRadians( eulerAngleZ ), g_vector001 ) * glm::angleAxis( S_DegreesToRadians( eulerAngleX ), g_vector100 );
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE void S_AnglesToQuaternionYZX( const vector3_t& eulerAngles, quaternion_t& result )
{
	S_AnglesToQuaternionYZX( eulerAngles.x, eulerAngles.y, eulerAngles.z, result );
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE quaternion_t S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
{
	quaternion_t result;
	S_AnglesToQuaternionZYX( eulerAngleX, eulerAngleY, eulerAngleZ, result );
	return result;
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE quaternion_t S_AnglesToQuaternionZYX( const vector3_t& eulerAngles )
{
	quaternion_t result;
	S_AnglesToQuaternionZYX( eulerAngles.x, eulerAngles.y, eulerAngles.z, result );
	return result;
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE void S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ, quaternion_t& result )
{
	result =
		glm::angleAxis( S_DegreesToRadians( eulerAngleZ ), g_vector001 ) * glm::angleAxis( S_DegreesToRadians( eulerAngleY ), g_vector010 ) * glm::angleAxis( S_DegreesToRadians( eulerAngleX ), g_vector100 );
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE void S_AnglesToQuaternionZYX( const vector3_t& eulerAngles, quaternion_t& result )
{
	S_AnglesToQuaternionZYX( eulerAngles.x, eulerAngles.y, eulerAngles.z, result );
}

/*
==================
S_QuaternionToAngles
==================
*/
FORCEINLINE vector3_t S_QuaternionToAngles( const quaternion_t& quaternion )
{
	vector3_t result;
	S_QuaternionToAngles( quaternion, result );
	return result;
}

/*
==================
S_QuaternionToAngles
==================
*/
FORCEINLINE void S_QuaternionToAngles( const quaternion_t& quaternion, vector3_t& result )
{
	float quatSquareW	  = S_Pow( quaternion.w, 2.f );
	float quatSquareX	  = S_Pow( quaternion.x, 2.f );
	float quatSquareY	  = S_Pow( quaternion.y, 2.f );
	float quatSquareZ	  = S_Pow( quaternion.z, 2.f );
	float normalizeFactor = quatSquareX + quatSquareY + quatSquareZ + quatSquareW;	// Normalize factor. If quaternion already is normalized it's will be one, otherwise it's correct factor
	float singularityTest = quaternion.x * quaternion.y + quaternion.z * quaternion.w;

	// Reference
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	constexpr float singularityThreshold = 0.4999995f;
	float			pitch, yaw, roll;

	// Singularity at north pole
	if ( singularityTest > singularityThreshold * normalizeFactor )
	{
		pitch = 0.f;
		yaw	  = 2.f * S_Atan2( quaternion.x, quaternion.w );
		roll  = PI / 2.f;
	}
	// Singularity at south pole
	else if ( singularityTest < -singularityThreshold * normalizeFactor )
	{
		pitch = 0.f;
		yaw	  = -2.f * S_Atan2( quaternion.x, quaternion.w );
		roll  = -PI / 2.f;
	}
	else
	{
		pitch = S_Atan2( 2.f * quaternion.x * quaternion.w - 2.f * quaternion.y * quaternion.z, -quatSquareX + quatSquareY - quatSquareZ + quatSquareW );
		yaw	  = S_Atan2( 2.f * quaternion.y * quaternion.w - 2.f * quaternion.x * quaternion.z, quatSquareX - quatSquareY - quatSquareZ + quatSquareW );
		roll  = S_Asin( 2.f * singularityTest / normalizeFactor );
	}

	result.x = S_RadiansToDegrees( pitch );
	result.y = S_RadiansToDegrees( yaw );
	result.z = S_RadiansToDegrees( roll );
}

/*
==================
S_QuaternionToMatrix
==================
*/
FORCEINLINE matrix4x4_t S_QuaternionToMatrix( const quaternion_t& quaternion )
{
	matrix4x4_t result;
	S_QuaternionToMatrix( quaternion, result );
	return result;
}

/*
==================
S_QuaternionToMatrix
==================
*/
FORCEINLINE void S_QuaternionToMatrix( const quaternion_t& quaternion, matrix4x4_t& result )
{
	result = glm::mat4_cast( quaternion );
}

/*
==================
S_QuaternionInverse
==================
*/
FORCEINLINE quaternion_t S_QuaternionInverse( const quaternion_t& quaternion )
{
	quaternion_t result;
	S_QuaternionInverse( quaternion, result );
	return result;
}

/*
==================
S_QuaternionInverse
==================
*/
FORCEINLINE void S_QuaternionInverse( const quaternion_t& quaternion, quaternion_t& result )
{
	result = glm::inverse( quaternion );
}

/*
==================
S_QuatenrionLookAt
==================
*/
FORCEINLINE quaternion_t S_QuatenrionLookAt( const vector3_t& lookFrom, const vector3_t& lookTo, const vector3_t& up, const vector3_t& globalUp )
{
	quaternion_t result;
	S_QuatenrionLookAt( lookFrom, lookTo, up, globalUp, result );
	return result;
}

/*
==================
S_QuatenrionLookAt
==================
*/
FORCEINLINE void S_QuatenrionLookAt( const vector3_t& lookFrom, const vector3_t& lookTo, const vector3_t& up, const vector3_t& globalUp, quaternion_t& result )
{
	vector3_t direction		  = lookTo - lookFrom;
	float	  directionLength = S_VectorLength( direction );
	S_VectorNormalize( direction, direction );

	// Check if the direction is valid; Also deals with NaN
	if ( directionLength <= 0.0001f )
	{
		result = g_quaternionIdentity;	// Just return identity
		return;
	}

	// NOTE: Use global up if is the normal up (nearly) parallel to direction
	result = glm::quatLookAt( direction, S_Abs( S_VectorDotProduct( direction, up ) ) > 0.9999f ? globalUp : up ) * glm::angleAxis( S_DegreesToRadians( 90.f ), g_vector100 );
}

/*
==================
S_QuaternionNormalize
==================
*/
FORCEINLINE quaternion_t S_QuaternionNormalize( const quaternion_t& quaternion )
{
	quaternion_t result;
	S_QuaternionNormalize( quaternion, result );
	return result;
}

/*
==================
S_QuaternionNormalize
==================
*/
FORCEINLINE void S_QuaternionNormalize( const quaternion_t& quaternion, quaternion_t& result )
{
	result = glm::normalize( quaternion );
}

/*
==================
S_MatrixFromString
==================
*/
FORCEINLINE matrix4x4_t S_MatrixFromString( const char* pString )
{
	matrix4x4_t result;
	S_MatrixFromString( pString, result );
	return result;
}

/*
==================
S_MatrixFromString
==================
*/
FORCEINLINE void S_MatrixFromString( const char* pString, matrix4x4_t& matrix )
{
	S_Sscanf( pString, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
			  &matrix[0].x, &matrix[0].y, &matrix[0].z, &matrix[0].w,
			  &matrix[1].x, &matrix[1].y, &matrix[1].z, &matrix[1].w,
			  &matrix[2].x, &matrix[2].y, &matrix[2].z, &matrix[2].w,
			  &matrix[3].x, &matrix[3].y, &matrix[3].z, &matrix[3].w );
}

/*
==================
S_MatrixToString
==================
*/
FORCEINLINE eastl::string S_MatrixToString( const matrix4x4_t& matrix )
{
	eastl::string result;
	S_MatrixToString( matrix, result );
	return result;
}

/*
==================
S_MatrixToString
==================
*/
FORCEINLINE void S_MatrixToString( const matrix4x4_t& matrix, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
						matrix[0].x, matrix[0].y, matrix[0].z, matrix[0].w,
						matrix[1].x, matrix[1].y, matrix[1].z, matrix[1].w,
						matrix[2].x, matrix[2].y, matrix[2].z, matrix[2].w,
						matrix[3].x, matrix[3].y, matrix[3].z, matrix[3].w );
}

/*
==================
S_MatrixIdentity
==================
*/
FORCEINLINE void S_MatrixIdentity( matrix4x4_t& matrix )
{
	matrix = glm::identity<matrix4x4_t>();
}

/*
==================
S_MatrixIdentity
==================
*/
FORCEINLINE matrix4x4_t S_MatrixIdentity()
{
	return glm::identity<matrix4x4_t>();
}

/*
==================
S_MatrixOrigin
==================
*/
FORCEINLINE void S_MatrixOrigin( const matrix4x4_t& matrix, vector3_t& origin )
{
	origin.x = matrix[3].x;
	origin.y = matrix[3].y;
	origin.z = matrix[3].z;
}

/*
==================
S_MatrixOrigin
==================
*/
FORCEINLINE vector3_t S_MatrixOrigin( const matrix4x4_t& matrix )
{
	vector3_t result;
	S_MatrixOrigin( matrix, result );
	return result;
}

/*
==================
S_MatrixTranslate
==================
*/
FORCEINLINE void S_MatrixTranslate( const vector3_t& location, matrix4x4_t& matrix )
{
	matrix[3].x = location.x;
	matrix[3].y = location.y;
	matrix[3].z = location.z;
}

/*
==================
S_MatrixTranslate
==================
*/
FORCEINLINE matrix4x4_t S_MatrixTranslate( const vector3_t& location )
{
	return glm::translate( g_matrixIdentity, location );
}

/*
==================
S_MatrixScale
==================
*/
FORCEINLINE void S_MatrixScale( const vector3_t& scale, matrix4x4_t& matrix )
{
	matrix[0].x = scale.x;
	matrix[1].y = scale.y;
	matrix[2].z = scale.z;
}

/*
==================
S_MatrixScale
==================
*/
FORCEINLINE matrix4x4_t S_MatrixScale( const vector3_t& scale )
{
	return glm::scale( g_matrixIdentity, scale );
}

/*
==================
S_MatrixInverse
==================
*/
FORCEINLINE void S_MatrixInverse( const matrix4x4_t& srcMatrix, matrix4x4_t& destMatrix )
{
	destMatrix = glm::inverse( srcMatrix );
}

/*
==================
S_MatrixInverse
==================
*/
FORCEINLINE matrix4x4_t S_MatrixInverse( const matrix4x4_t& matrix )
{
	matrix4x4_t result;
	S_MatrixInverse( matrix, result );
	return result;
}

/*
==================
S_MatrixPerspective
==================
*/
FORCEINLINE matrix4x4_t S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane )
{
	matrix4x4_t result;
	S_MatrixPerspective( fieldOfView, aspectRatio, nearClipPlane, farClipPlane, result );
	return result;
}

/*
==================
S_MatrixPerspective
==================
*/
FORCEINLINE void S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane, matrix4x4_t& matrix )
{
	matrix = glm::perspective( S_DegreesToRadians( fieldOfView ), aspectRatio, nearClipPlane, farClipPlane );
}

/*
==================
S_MatrixOrtho
==================
*/
FORCEINLINE matrix4x4_t S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane )
{
	matrix4x4_t result;
	S_MatrixOrtho( left, right, bottom, top, nearClipPlane, farClipPlane, result );
	return result;
}

/*
==================
S_MatrixOrtho
==================
*/
FORCEINLINE void S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane, matrix4x4_t& matrix )
{
	matrix = glm::ortho( left, right, bottom, top, nearClipPlane, farClipPlane );
}

/*
==================
S_MatrixLookAt
==================
*/
FORCEINLINE matrix4x4_t S_MatrixLookAt( const vector3_t& location, const vector3_t& direction, const vector3_t& axisUp )
{
	matrix4x4_t result;
	S_MatrixLookAt( location, direction, axisUp, result );
	return result;
}

/*
==================
S_MatrixLookAt
==================
*/
FORCEINLINE void S_MatrixLookAt( const vector3_t& location, const vector3_t& direction, const vector3_t& axisUp, matrix4x4_t& matrix )
{
	matrix = glm::lookAt( location, direction, axisUp );
}
