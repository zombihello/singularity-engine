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
	return powf( a, b );
}

/*
==================
S_Log2
==================
*/
FORCEINLINE float S_Log2( float a )
{
	return log2( a );
}

/*
==================
S_Loge
==================
*/
FORCEINLINE float S_Loge( float a )
{
	return logf( a );
}

/*
==================
S_LogX
==================
*/
FORCEINLINE float S_LogX( float base, float value )
{
	return S_Loge( value ) / S_Loge( base );
}

/*
==================
S_Trunc
==================
*/
FORCEINLINE float S_Trunc( float a )
{
	return truncf( a );
}

/*
==================
S_Round
==================
*/
FORCEINLINE float S_Round( float a )
{
	return roundf( a );
}

/*
==================
S_Sin
==================
*/
FORCEINLINE float S_Sin( float a )
{
	return sinf( a );
}

/*
==================
S_Cos
==================
*/
FORCEINLINE float S_Cos( float a )
{
	return cosf( a );
}

/*
==================
S_ASin
==================
*/
FORCEINLINE float S_ASin( float a )
{
	return asinf( a );
}

/*
==================
S_ATan
==================
*/
FORCEINLINE float S_ATan( float a )
{
	return atanf( a );
}

/*
==================
S_ATan2
==================
*/
FORCEINLINE float S_ATan2( float y, float x )
{
	return atan2f( y, x );
}

/*
==================
S_Sqrt
==================
*/
FORCEINLINE float S_Sqrt( float a )
{
	return sqrtf( a );
}

/*
==================
S_Floor
==================
*/
FORCEINLINE float S_Floor( float a )
{
	return floorf( a );
}

/*
==================
S_Clamp
==================
*/
FORCEINLINE float S_Clamp( float x, float min, float max )
{
	return ::Clamp( x, min, max );
}

/*
==================
S_Abs
==================
*/
FORCEINLINE float S_Abs( float x )
{
	return abs( x );
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
FORCEINLINE bool S_IsNearlyZero( const vec2_t& value, float errorTolerance /*/*= SMALL_NUMBER*/ )
{
	return S_Abs( value.x ) < errorTolerance && S_Abs( value.y ) < errorTolerance;
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( const vec3_t& value, float errorTolerance /*= SMALL_NUMBER*/ )
{
	return S_Abs( value.x ) < errorTolerance && S_Abs( value.y ) < errorTolerance && S_Abs( value.z ) < errorTolerance;
}

/*
==================
S_IsNearlyZero
==================
*/
FORCEINLINE bool S_IsNearlyZero( const vec4_t& value, float errorTolerance /*= SMALL_NUMBER*/ )
{
	return S_Abs( value.x ) < errorTolerance && S_Abs( value.y ) < errorTolerance && S_Abs( value.z ) < errorTolerance && S_Abs( value.w ) < errorTolerance;
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
S_GridSnap
==================
*/
FORCEINLINE float S_GridSnap( float value, float grid )
{
	if ( grid == 0.f )
	{
		return value;
	}
	else
	{
		return S_Floor( ( value + 0.5f * grid ) / grid ) * grid;
	}
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vec2_t S_GridSnap( const vec2_t& value, float grid )
{
	vec2_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vec2_t& value, float grid, vec2_t& resultValue )
{
	resultValue = vec2_t( S_GridSnap( value.x, grid ), S_GridSnap( value.y, grid ) );
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vec3_t S_GridSnap( const vec3_t& value, float grid )
{
	vec3_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vec3_t& value, float grid, vec3_t& resultValue )
{
	resultValue = vec3_t( S_GridSnap( value.x, grid ), S_GridSnap( value.y, grid ), S_GridSnap( value.z, grid ) );
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE vec4_t S_GridSnap( const vec4_t& value, float grid )
{
	vec4_t retVec;
	S_GridSnap( value, grid, retVec );
	return retVec;
}

/*
==================
S_GridSnap
==================
*/
FORCEINLINE void S_GridSnap( const vec4_t& value, float grid, vec4_t& resultValue )
{
	resultValue = vec4_t( S_GridSnap( value.x, grid ), S_GridSnap( value.y, grid ), S_GridSnap( value.z, grid ), S_GridSnap( value.w, grid ) );
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
S_VectorCreate
==================
*/
template<typename TVectorType>
FORCEINLINE TVectorType S_VectorCreate( const char* pString )
{
	TVectorType result;
	S_VectorCreate( pString, result );
	return result;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( const char* pString, vec2_t& vector )
{
	S_Sscanf( pString, "%f %f", &vector.x, &vector.y );
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( const char* pString, vec3_t& vector )
{
	S_Sscanf( pString, "%f %f %f", &vector.x, &vector.y, &vector.z );
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( const char* pString, vec4_t& vector )
{
	S_Sscanf( pString, "%f %f %f %f", &vector.x, &vector.y, &vector.z, &vector.w );
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE vec2_t S_VectorCreate( float x, float y )
{
	vec2_t result;
	S_VectorCreate( x, y, result );
	return result;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE vec3_t S_VectorCreate( float x, float y, float z )
{
	vec3_t result;
	S_VectorCreate( x, y, z, result );
	return result;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE vec4_t S_VectorCreate( float x, float y, float z, float w )
{
	vec4_t result;
	S_VectorCreate( x, y, z, w, result );
	return result;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( float x, float y, vec2_t& vector )
{
	vector.x = x;
	vector.y = y;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( float x, float y, float z, vec3_t& vector )
{
	vector.x = x;
	vector.y = y;
	vector.z = z;
}

/*
==================
S_VectorCreate
==================
*/
FORCEINLINE void S_VectorCreate( float x, float y, float z, float w, vec4_t& vector )
{
	vector.x = x;
	vector.y = y;
	vector.z = z;
	vector.w = w;
}

/*
==================
S_VectorToString
==================
*/
FORCEINLINE eastl::string S_VectorToString( const vec2_t& vector )
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
FORCEINLINE eastl::string S_VectorToString( const vec3_t& vector )
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
FORCEINLINE eastl::string S_VectorToString( const vec4_t& vector )
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
FORCEINLINE void S_VectorToString( const vec2_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f", vector.x, vector.y );
}

/*
==================
S_VectorToString
==================
*/
FORCEINLINE void S_VectorToString( const vec3_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f", vector.x, vector.y, vector.z );
}

/*
==================
S_VectorToString
==================
*/
FORCEINLINE void S_VectorToString( const vec4_t& vector, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f %f", vector.x, vector.y, vector.z, vector.w );
}

/*
==================
S_VectorDotProduct
==================
*/
FORCEINLINE float S_VectorDotProduct( const vec3_t& x, const vec3_t& y )
{
	return glm::dot( x, y );
}

/*
==================
S_VectorNormalize
==================
*/
FORCEINLINE vec2_t S_VectorNormalize( const vec2_t& vector )
{
	return glm::normalize( vector );
}

/*
==================
S_VectorNormalize
==================
*/
FORCEINLINE vec3_t S_VectorNormalize( const vec3_t& vector )
{
	return glm::normalize( vector );
}

/*
==================
S_VectorNormalize
==================
*/
FORCEINLINE vec4_t S_VectorNormalize( const vec4_t& vector )
{
	return glm::normalize( vector );
}

/*
==================
S_VectorLength
==================
*/
FORCEINLINE float S_VectorLength( const vec2_t& vector )
{
	return glm::length( vector );
}

/*
==================
S_VectorLength
==================
*/
FORCEINLINE float S_VectorLength( const vec3_t& vector )
{
	return glm::length( vector );
}

/*
==================
S_VectorLength
==================
*/
FORCEINLINE float S_VectorLength( const vec4_t& vector )
{
	return glm::length( vector );
}

/*
==================
S_VectorCross
==================
*/
FORCEINLINE void S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB, vec3_t& resultVector )
{
	resultVector = glm::cross( vectorA, vectorB );
}

/*
==================
S_VectorCross
==================
*/
FORCEINLINE vec3_t S_VectorCross( const vec3_t& vectorA, const vec3_t& vectorB )
{
	vec3_t result;
	S_VectorCross( vectorA, vectorB, result );
	return result;
}

/*
==================
S_VectorDistance
==================
*/
FORCEINLINE float S_VectorDistance( const vec3_t& vectorA, const vec3_t& vectorB )
{
	return glm::distance( vectorA, vectorB );
}

/*
==================
S_QuaternionCreate
==================
*/
FORCEINLINE quat_t S_QuaternionCreate( const char* pString )
{
	quat_t result;
	S_QuaternionCreate( pString, result );
	return result;
}

/*
==================
S_QuaternionCreate
==================
*/
FORCEINLINE void S_QuaternionCreate( const char* pString, quat_t& quaternion )
{
	S_Sscanf( pString, "%f %f %f %f", &quaternion.x, &quaternion.y, &quaternion.z, &quaternion.w );
}

/*
==================
S_QuaternionCreate
==================
*/
FORCEINLINE quat_t S_QuaternionCreate( float x, float y, float z, float w )
{
	quat_t result;
	S_QuaternionCreate( x, y, z, w, result );
	return result;
}

/*
==================
S_QuaternionCreate
==================
*/
FORCEINLINE void S_QuaternionCreate( float x, float y, float z, float w, quat_t& quaternion )
{
	quaternion.x = x;
	quaternion.y = y;
	quaternion.z = z;
	quaternion.w = w;
}

/*
==================
S_QuaternionToString
==================
*/
FORCEINLINE eastl::string S_QuaternionToString( const quat_t& quaternion )
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
FORCEINLINE void S_QuaternionToString( const quat_t& quaternion, eastl::string& result )
{
	result = S_Sprintf( "%f %f %f %f", quaternion.x, quaternion.y, quaternion.z, quaternion.w );
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE quat_t S_AnglesToQuaternionYZX( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
{
	return glm::angleAxis( S_DegreesToRadians( eulerAngleY ), vec3_t( 0.f, 1.f, 0.f ) ) * glm::angleAxis( S_DegreesToRadians( eulerAngleZ ), vec3_t( 0.f, 0.f, 1.f ) ) * glm::angleAxis( S_DegreesToRadians( eulerAngleX ), vec3_t( 1.f, 0.f, 0.f ) );
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE quat_t S_AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
{
	return glm::angleAxis( S_DegreesToRadians( eulerAngleZ ), vec3_t( 0.f, 0.f, 1.f ) ) * glm::angleAxis( S_DegreesToRadians( eulerAngleY ), vec3_t( 0.f, 1.f, 0.f ) ) * glm::angleAxis( S_DegreesToRadians( eulerAngleX ), vec3_t( 1.f, 0.f, 0.f ) );
}

/*
==================
S_AnglesToQuaternionYZX
==================
*/
FORCEINLINE quat_t S_AnglesToQuaternionYZX( const vec3_t& eulerAngles )
{
	return S_AnglesToQuaternionYZX( eulerAngles.x, eulerAngles.y, eulerAngles.z );
}

/*
==================
S_AnglesToQuaternionZYX
==================
*/
FORCEINLINE quat_t S_AnglesToQuaternionZYX( const vec3_t& eulerAngles )
{
	return S_AnglesToQuaternionZYX( eulerAngles.x, eulerAngles.y, eulerAngles.z );
}

/*
==================
S_QuaternionToAngles
==================
*/
FORCEINLINE vec3_t S_QuaternionToAngles( const quat_t& quaternion )
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
	const float SINGULARITY_THRESHOLD = 0.4999995f;
	float		pitch, yaw, roll;

	// Singularity at north pole
	if ( singularityTest > SINGULARITY_THRESHOLD * normalizeFactor )
	{
		pitch = 0.f;
		yaw	  = 2.f * S_ATan2( quaternion.x, quaternion.w );
		roll  = PI / 2.f;
	}
	// Singularity at south pole
	else if ( singularityTest < -SINGULARITY_THRESHOLD * normalizeFactor )
	{
		pitch = 0.f;
		yaw	  = -2.f * S_ATan2( quaternion.x, quaternion.w );
		roll  = -PI / 2.f;
	}
	else
	{
		pitch = S_ATan2( 2.f * quaternion.x * quaternion.w - 2.f * quaternion.y * quaternion.z, -quatSquareX + quatSquareY - quatSquareZ + quatSquareW );
		yaw	  = S_ATan2( 2.f * quaternion.y * quaternion.w - 2.f * quaternion.x * quaternion.z, quatSquareX - quatSquareY - quatSquareZ + quatSquareW );
		roll  = S_ASin( 2.f * singularityTest / normalizeFactor );
	}

	return vec3_t( S_RadiansToDegrees( pitch ), S_RadiansToDegrees( yaw ), S_RadiansToDegrees( roll ) );
}

/*
==================
S_QuaternionToMatrix
==================
*/
FORCEINLINE mat4_t S_QuaternionToMatrix( const quat_t& quaternion )
{
	return glm::mat4_cast( quaternion );
}

/*
==================
S_QuaternionInverse
==================
*/
FORCEINLINE void S_QuaternionInverse( const quat_t& srcQuaternion, quat_t& destQuaternion )
{
	destQuaternion = glm::inverse( srcQuaternion );
}

/*
==================
S_QuaternionInverse
==================
*/
FORCEINLINE quat_t S_QuaternionInverse( const quat_t& quaternion )
{
	quat_t result;
	S_QuaternionInverse( quaternion, result );
	return result;
}

/*
==================
S_QuatenrionLookAt
==================
*/
FORCEINLINE quat_t S_QuatenrionLookAt( const vec3_t& lookFrom, const vec3_t& lookTo, const vec3_t& up, const vec3_t& globalUp )
{
	vec3_t direction	   = lookTo - lookFrom;
	float  directionLength = S_VectorLength( direction );
	direction			   = S_VectorNormalize( direction );

	// Check if the direction is valid; Also deals with NaN
	if ( directionLength <= 0.0001f )
	{
		return quat_t( 1.f, 0.f, 0.f, 0.f );  // Just return identity
	}

	// Is the normal up (nearly) parallel to direction?
	if ( S_Abs( S_VectorDotProduct( direction, up ) ) > 0.9999f )
	{
		// Use alternative up
		return glm::quatLookAt( direction, globalUp ) * glm::angleAxis( S_DegreesToRadians( 90.f ), vec3_t( 1.f, 0.f, 0.f ) );
	}
	else
	{
		return glm::quatLookAt( direction, up ) * glm::angleAxis( S_DegreesToRadians( 90.f ), vec3_t( 1.f, 0.f, 0.f ) );
	}
}

/*
==================
S_QuaternionNormalize
==================
*/
FORCEINLINE quat_t S_QuaternionNormalize( const quat_t& quaternion )
{
	return glm::normalize( quaternion );
}

/*
==================
S_MatrixCreate
==================
*/
FORCEINLINE mat4_t S_MatrixCreate( const char* pString )
{
	mat4_t result;
	S_MatrixCreate( pString, result );
	return result;
}

/*
==================
S_MatrixCreate
==================
*/
FORCEINLINE void S_MatrixCreate( const char* pString, mat4_t& matrix )
{
	S_Sscanf( pString, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
			  &matrix[0].x, &matrix[0].y, &matrix[0].z, &matrix[0].w,
			  &matrix[1].x, &matrix[1].y, &matrix[1].z, &matrix[1].w,
			  &matrix[2].x, &matrix[2].y, &matrix[2].z, &matrix[2].w,
			  &matrix[3].x, &matrix[3].y, &matrix[3].z, &matrix[3].w );
}

/*
==================
S_MatrixCreate
==================
*/
FORCEINLINE mat4_t S_MatrixCreate( const vec4_t& row0, const vec4_t& row1, const vec4_t& row2, const vec4_t& row3 )
{
	mat4_t result;
	S_MatrixCreate( row0, row1, row2, row3, result );
	return result;
}

/*
==================
S_MatrixCreate
==================
*/
FORCEINLINE void S_MatrixCreate( const vec4_t& row0, const vec4_t& row1, const vec4_t& row2, const vec4_t& row3, mat4_t& matrix )
{
	matrix[0] = row0;
	matrix[1] = row1;
	matrix[2] = row2;
	matrix[3] = row3;
}

/*
==================
S_MatrixToString
==================
*/
FORCEINLINE eastl::string S_MatrixToString( const mat4_t& matrix )
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
FORCEINLINE void S_MatrixToString( const mat4_t& matrix, eastl::string& result )
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
FORCEINLINE void S_MatrixIdentity( mat4_t& matrix )
{
	matrix = glm::identity<mat4_t>();
}

/*
==================
S_MatrixIdentity
==================
*/
FORCEINLINE mat4_t S_MatrixIdentity()
{
	return glm::identity<mat4_t>();
}

/*
==================
S_MatrixOrigin
==================
*/
FORCEINLINE void S_MatrixOrigin( const mat4_t& matrix, vec3_t& origin )
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
FORCEINLINE vec3_t S_MatrixOrigin( const mat4_t& matrix )
{
	vec3_t result;
	S_MatrixOrigin( matrix, result );
	return result;
}

/*
==================
S_MatrixTranslate
==================
*/
FORCEINLINE void S_MatrixTranslate( const vec3_t& location, mat4_t& matrix )
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
FORCEINLINE void S_MatrixTranslate( const vec3_t& location, const mat4_t& initMatrix, mat4_t& matrix )
{
	matrix = glm::translate( initMatrix, location );
}

/*
==================
S_MatrixTranslate
==================
*/
FORCEINLINE mat4_t S_MatrixTranslate( const vec3_t& location, const mat4_t& initMatrix /*= mat4_t( 1.f )*/ )
{
	return glm::translate( initMatrix, location );
}

/*
==================
S_MatrixScale
==================
*/
FORCEINLINE void S_MatrixScale( const vec3_t& scale, mat4_t& matrix )
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
FORCEINLINE void S_MatrixScale( const vec3_t& scale, const mat4_t& initMatrix, mat4_t& matrix )
{
	matrix = glm::scale( initMatrix, scale );
}

/*
==================
S_MatrixScale
==================
*/
FORCEINLINE mat4_t S_MatrixScale( const vec3_t& scale, const mat4_t& initMatrix /*= mat4_t( 1.f )*/ )
{
	return glm::scale( initMatrix, scale );
}

/*
==================
S_MatrixInverse
==================
*/
FORCEINLINE void S_MatrixInverse( const mat4_t& srcMatrix, mat4_t& destMatrix )
{
	destMatrix = glm::inverse( srcMatrix );
}

/*
==================
S_MatrixInverse
==================
*/
FORCEINLINE mat4_t S_MatrixInverse( const mat4_t& matrix )
{
	mat4_t result;
	S_MatrixInverse( matrix, result );
	return result;
}

/*
==================
S_MatrixPerspective
==================
*/
FORCEINLINE mat4_t S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane )
{
	mat4_t result;
	S_MatrixPerspective( fieldOfView, aspectRatio, nearClipPlane, farClipPlane, result );
	return result;
}

/*
==================
S_MatrixPerspective
==================
*/
FORCEINLINE void S_MatrixPerspective( float fieldOfView, float aspectRatio, float nearClipPlane, float farClipPlane, mat4_t& matrix )
{
	matrix = glm::perspective( S_DegreesToRadians( fieldOfView ), aspectRatio, nearClipPlane, farClipPlane );
}

/*
==================
S_MatrixOrtho
==================
*/
FORCEINLINE mat4_t S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane )
{
	mat4_t result;
	S_MatrixOrtho( left, right, bottom, top, nearClipPlane, farClipPlane, result );
	return result;
}

/*
==================
S_MatrixOrtho
==================
*/
FORCEINLINE void S_MatrixOrtho( float left, float right, float bottom, float top, float nearClipPlane, float farClipPlane, mat4_t& matrix )
{
	matrix = glm::ortho( left, right, bottom, top, nearClipPlane, farClipPlane );
}

/*
==================
S_MatrixLookAt
==================
*/
FORCEINLINE mat4_t S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp )
{
	mat4_t result;
	S_MatrixLookAt( location, direction, axisUp, result );
	return result;
}

/*
==================
S_MatrixLookAt
==================
*/
FORCEINLINE void S_MatrixLookAt( const vec3_t& location, const vec3_t& direction, const vec3_t& axisUp, mat4_t& matrix )
{
	matrix = glm::lookAt( location, direction, axisUp );
}
