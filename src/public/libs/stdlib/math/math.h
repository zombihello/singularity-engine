/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef MATH_H
#define MATH_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>
#include <gtx/compatibility.hpp>

#include "stdlib/template.h"
#include "stdlib/math/axis.h"
#include "core/types.h"
#include "core/platform.h"

// Constants
#undef PI
#undef SMALL_NUMBER
#undef KINDA_SMALL_NUMBER
#undef BIG_NUMBER
#undef EULERS_NUMBER

/**
 * @ingroup stdlib
 * @brief Macro of PI value
 */
#define PI 					(3.1415926535897932)

/**
 * @ingroup stdlib
 * @brief Macro of small number
 */
#define SMALL_NUMBER		(1.e-8)

/**
 * @ingroup stdlib
 * @brief Macro of kinda small number
 */
#define KINDA_SMALL_NUMBER	(1.e-4)

/**
 * @ingroup stdlib
 * @brief Macro of big number
 */
#define BIG_NUMBER			(3.4e+38f)

/**
 * @ingroup stdlib
 * @brief Macro of Euler's number
 */
#define EULERS_NUMBER       (2.71828182845904523536)

/**
 * @ingroup stdlib
 * @brief Typedef glm::vec2
 */
typedef glm::vec2		vec2_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::ivec2
 */
typedef glm::ivec2		ivec2_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::vec3
 */
typedef glm::vec3		vec3_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::ivec3
 */
typedef glm::ivec3		ivec3_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::vec4
 */
typedef glm::vec4		vec4_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::ivec4
 */
typedef glm::ivec4		ivec4_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::mat4
 */
typedef glm::mat4		matrix_t;

/**
 * @ingroup stdlib
 * @brief Typedef glm::quat
 */
typedef glm::quat		quat_t;

/**
 * @ingroup stdlib
 * @brief Structure for all math helper functions
 */
struct math_t
{
	/**
	 * @ingroup stdlib
	 * @brief Convert from euler angles to quaternion YZX (default)
	 *
	 * @param eulerAngleX		Euler angle by X
	 * @param eulerAngleY		Euler angle by Y
	 * @param eulerAngleZ		Euler angle by Z
	 */
	static FORCEINLINE quat_t AnglesToQuaternion( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
	{
		return
			glm::angleAxis( DegreesToRadians( eulerAngleY ), vec3_t( 0.f, 1.f, 0.f ) ) *
			glm::angleAxis( DegreesToRadians( eulerAngleZ ), vec3_t( 0.f, 0.f, 1.f ) ) *
			glm::angleAxis( DegreesToRadians( eulerAngleX ), vec3_t( 1.f, 0.f, 0.f ) );
	}

	/**
	 * @ingroup stdlib
	 * @brief Convert from euler angles to quaternion ZYX
	 *
	 * @param eulerAngleX		Euler angle by X
	 * @param eulerAngleY		Euler angle by Y
	 * @param eulerAngleZ		Euler angle by Z
	 */
	static FORCEINLINE quat_t AnglesToQuaternionZYX( float eulerAngleX, float eulerAngleY, float eulerAngleZ )
	{
		return
			glm::angleAxis( DegreesToRadians( eulerAngleZ ), vec3_t( 0.f, 0.f, 1.f ) ) *
			glm::angleAxis( DegreesToRadians( eulerAngleY ), vec3_t( 0.f, 1.f, 0.f ) ) *
			glm::angleAxis( DegreesToRadians( eulerAngleX ), vec3_t( 1.f, 0.f, 0.f ) );
	}
	/**
	 * @ingroup stdlib
	 * @brief Convert from euler angles to quaternion YZX (default)
	 *
	 * @param eulerAngles	Euler angles
	 */
	static FORCEINLINE quat_t AnglesToQuaternion( const vec3_t& eulerAngles )
	{
		return AnglesToQuaternion( eulerAngles.x, eulerAngles.y, eulerAngles.z );
	}

	/**
	 * @ingroup stdlib
	 * @brief Convert from euler angles to quaternion ZYX
	 *
	 * @param eulerAngles	Euler angles
	 */
	static FORCEINLINE quat_t AnglesToQuaternionZYX( const vec3_t& eulerAngles )
	{
		return AnglesToQuaternionZYX( eulerAngles.x, eulerAngles.y, eulerAngles.z );
	}

	/**
	 * @ingroup stdlib
	 * @brief Convert from quaternion to euler angles
	 * 
	 * @param quaternion	Quaternion
	 * @return Return euler angles in radians
	 */
	static FORCEINLINE vec3_t QuaternionToAngles( const quat_t& quaternion )
	{
		float	quatSquareW			= Pow( quaternion.w, 2.f );
		float	quatSquareX			= Pow( quaternion.x, 2.f );
		float	quatSquareY			= Pow( quaternion.y, 2.f );
		float	quatSquareZ			= Pow( quaternion.z, 2.f );
		float	normalizeFactor		= quatSquareX + quatSquareY + quatSquareZ + quatSquareW;				// Normalize factor. If quaternion already is normalized it's will be one, otherwise it's correct factor
		float	singularityTest		= quaternion.x * quaternion.y + quaternion.z * quaternion.w;

		// Reference
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

		const float	SINGULARITY_THRESHOLD = 0.4999995f;
		float		pitch, yaw, roll;

		// Singularity at north pole
		if ( singularityTest > SINGULARITY_THRESHOLD * normalizeFactor )
		{
			pitch	= 0.f;
			yaw		= 2.f * ATan2( quaternion.x, quaternion.w );
			roll	= PI / 2.f;
		}
		// Singularity at south pole
		else if ( singularityTest < -SINGULARITY_THRESHOLD * normalizeFactor )
		{
			pitch	= 0.f;
			yaw		= -2.f * ATan2( quaternion.x, quaternion.w );
			roll	= -PI / 2.f;
		}
		else
		{
			pitch	= ATan2( 2.f * quaternion.x * quaternion.w - 2.f * quaternion.y * quaternion.z, -quatSquareX + quatSquareY - quatSquareZ + quatSquareW );
			yaw		= ATan2( 2.f * quaternion.y * quaternion.w - 2.f * quaternion.x * quaternion.z, quatSquareX - quatSquareY - quatSquareZ + quatSquareW );
			roll	= ASin( 2.f * singularityTest / normalizeFactor );
		}

		return vec3_t( RadiansToDegrees( pitch ), RadiansToDegrees( yaw ), RadiansToDegrees( roll ) );
	}

    /**
     * @brief Convert from quaternion to matrix
     *
     * @param quaternion	quat_t
     * @return Return matrix of ratation
     */
    static FORCEINLINE matrix_t QuaternionToMatrix( const quat_t& quaternion )
    {
        return glm::mat4_cast( quaternion );
    }

	/**
	 * @ingroup stdlib
	 * @brief Convert radians to degrees
	 * 
	 * @param radians	Radians
	 */
	static FORCEINLINE float RadiansToDegrees( float radians )
	{
		return glm::degrees( radians );
	}

	/**
	 * @ingroup stdlib
	 * @brief Convert degrees to radians
	 * 
	 * @param degrees	Degrees
	 */
	static FORCEINLINE float DegreesToRadians( float degrees )
	{
		return glm::radians( degrees );
	}

	/**
	 * @brief Create identity matrix
	 * @param matrix	Output created identity matrix
	 */
	static FORCEINLINE void IdentityMatrix( matrix_t& matrix )
	{
		matrix = glm::identity<matrix_t>();
	}

	/**
	 * @brief Create identity matrix
	 * @return Return created identity matrix
	 */
	static FORCEINLINE matrix_t IdentityMatrix()
	{
		return glm::identity<matrix_t>();
	}

	/**
	 * @brief Create translate matrix
	 *
	 * @param location	Location
	 * @param matrix	Output created translate matrix
	 */
	static FORCEINLINE void TranslateMatrix( const vec3_t& location, matrix_t& matrix )
	{
		matrix[3].x = location.x;
		matrix[3].y = location.y;
		matrix[3].z = location.z;
	}

	/**
	 * @brief Translate matrix
	 *
	 * @param location		Location
	 * @param initMatrix	Initial matrix
	 * @param matrix		Output result translated matrix
	 */
	static FORCEINLINE void TranslateMatrix( const vec3_t& location, const matrix_t& initMatrix, matrix_t& matrix )
	{
		matrix = glm::translate( initMatrix, location );
	}

    /**
     * @brief Translate matrix
     *
     * @param location		Location
	 * @param initMatrix	Initial matrix
     * @return Return translated matrix
     */
    static FORCEINLINE matrix_t TranslateMatrix( const vec3_t& location, const matrix_t& initMatrix = matrix_t( 1.f ) )
    {
		return glm::translate( initMatrix, location );
    }

	/**
	 * @brief Create scale matrix
	 *
	 * @param scale		Scale
	 * @param matrix	Output created scale matrix
	 */
	static FORCEINLINE void ScaleMatrix( const vec3_t& scale, matrix_t& matrix )
	{
		matrix[0].x = scale.x;
		matrix[1].y = scale.y;
		matrix[2].z = scale.z;
	}

	/**
	 * @brief Scale matrix
	 *
	 * @param scale			Scale
	 * @param initMatrix	Initial matrix
	 * @param matrix		Output result scaled matrix
	 */
	static FORCEINLINE void ScaleMatrix( const vec3_t& scale, const matrix_t& initMatrix, matrix_t& matrix )
	{
		matrix = glm::scale( initMatrix, scale );
	}

    /**
     * @brief Scale matrix
     *
     * @param scale			Scale
	 * @param initMatrix	Initial matrix
     * @return Return scaled matrix
     */
    static FORCEINLINE matrix_t ScaleMatrix( const vec3_t& scale, const matrix_t& initMatrix = matrix_t( 1.f ) )
    {
        return glm::scale( initMatrix, scale );
    }

	/**
	 * @brief Inverse matrix
	 * 
	 * @param srcMatrix		Input matrix
	 * @param destMatrix	Output matrix
	 */
	static FORCEINLINE void InverseMatrix( const matrix_t& srcMatrix, matrix_t& destMatrix )
	{
		destMatrix = glm::inverse( srcMatrix );
	}

	/**
	 * @brief Inverse matrix
	 *
	 * @param matrix	Input matrix
	 * @return Return inverted matrix
	 */
	static FORCEINLINE matrix_t InverseMatrix( const matrix_t& matrix )
	{
		matrix_t		result;
		InverseMatrix( matrix, result );
		return result;
	}

	/**
	 * @brief Inverse quaternion
	 *
	 * @param srcQuaternion		Input quaternion
	 * @param destQuaternion	Output quaternion
	 */
	static FORCEINLINE void InverseQuaternion( const quat_t& srcQuaternion, quat_t& destQuaternion )
	{
		destQuaternion = glm::inverse( srcQuaternion );
	}

	/**
	 * @brief Inverse quaternion
	 *
	 * @param quaternion	Input quaternion
	 * @return Return inverted quaternion
	 */
	static FORCEINLINE quat_t InverseQuaternion( const quat_t& quaternion )
	{
		quat_t		result;
		InverseQuaternion( quaternion, result );
		return result;
	}

	/**
	 * @brief Dot product
	 * 
	 * @param x		Vector X
	 * @param y		Vector Y
	 * @return Returns the dot product of x and y.
	 */
	static FORCEINLINE float DotProduct( const vec3_t& x, const vec3_t& y )
	{
		return glm::dot( x, y );
	}

	/**
	 * @brief LookAt quaternion
	 * 
	 * @param lookFrom		Look from position
	 * @param lookTo		Look to position
	 * @param up			Up vector
	 * @param globalUp		Global up vector
	 */
	static FORCEINLINE quat_t LookAtQuatenrion( const vec3_t& lookFrom, const vec3_t& lookTo, const vec3_t& up, const vec3_t& globalUp )
	{
		vec3_t		direction		= lookTo - lookFrom;
		float		directionLength	= math_t::LengthVector( direction );
		direction = math_t::NormalizeVector( direction );

		// Check if the direction is valid; Also deals with NaN
		if ( directionLength <= 0.0001f )
		{
			return quat_t( 1.f, 0.f, 0.f, 0.f );	// Just return identity
		}

		// Is the normal up (nearly) parallel to direction?
		if ( math_t::Abs( math_t::DotProduct( direction, up ) ) > 0.9999f )
		{
			// Use alternative up
			return glm::quatLookAt( direction, globalUp ) * glm::angleAxis( math_t::DegreesToRadians( 90.f ), vec3_t( 1.f, 0.f, 0.f ) );
		}
		else
		{
			return glm::quatLookAt( direction, up ) * glm::angleAxis( math_t::DegreesToRadians( 90.f ), vec3_t( 1.f, 0.f, 0.f ) );
		}
	}

	/**
	 * @brief Get origin from matrix
	 * 
	 * @param matrix	Input matrix
	 * @param origin	Output origin
	 */
	static FORCEINLINE void GetOriginMatrix( const matrix_t& matrix, vec3_t& origin )
	{
		origin.x = matrix[3].x;
		origin.y = matrix[3].y;
		origin.z = matrix[3].z;
	}

	/**
	 * @brief Get origin from matrix
	 *
	 * @param matrix	Input matrix
	 * @return Return origin from matrix
	 */
	static FORCEINLINE vec3_t GetOriginMatrix( const matrix_t& matrix )
	{
		vec3_t		result;
		GetOriginMatrix( matrix, result );
		return result;
	}

	/**
	 * @brief Normalize vector
	 * 
	 * @param vector	Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE vec2_t NormalizeVector( const vec2_t& vector )
	{
		return glm::normalize( vector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param vector	Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE vec3_t NormalizeVector( const vec3_t& vector )
	{
		return glm::normalize( vector );
	}

	/**
	 * @brief Normalize vector
	 *
	 * @param vector	Vector
	 * @return Return normalized vector
	 */
	static FORCEINLINE vec4_t NormalizeVector( const vec4_t& vector )
	{
		return glm::normalize( vector );
	}

	/**
	 * @brief Normalize quaternion
	 *
	 * @param quaternion	Quaternion
	 * @return Return normalized quaternion
	 */
	static FORCEINLINE quat_t NormalizeQuaternion( const quat_t& quaternion )
	{
		return glm::normalize( quaternion );
	}

	/**
	 * @brie Length vector
	 * 
	 * @param vector	Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const vec2_t& vector )
	{
		return glm::length( vector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param vector	Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const vec3_t& vector )
	{
		return glm::length( vector );
	}

	/**
	 * @brie Length vector
	 *
	 * @param vector	Vector
	 * @return Return length vector
	 */
	static FORCEINLINE float LengthVector( const vec4_t& vector )
	{
		return glm::length( vector );
	}

	/**
	 * @brief Cross vector
	 *
	 * @param vectorA		Vector A
	 * @param vectorB		Vector B
	 * @param resultVector	Output vector
	 */
	static FORCEINLINE void CrossVector( const vec3_t& vectorA, const vec3_t& vectorB, vec3_t& resultVector )
	{
		resultVector = glm::cross( vectorA, vectorB );
	}

	/**
	 * @brief Cross vector
	 *
	 * @param vectorA		Vector A
	 * @param vectorB		Vector B
	 * @return Return cross vector
	 */
	static FORCEINLINE vec3_t CrossVector( const vec3_t& vectorA, const vec3_t& vectorB )
	{
		vec3_t		result;
		CrossVector( vectorA, vectorB, result );
		return result;
	}

	/**
	 * @brief Pow
	 * 
	 * @param a		Value A
	 * @param b		Value B
	 * @return Return A to the extent  B
	 */
	static FORCEINLINE float Pow( float a, float b )
	{
		return powf( a, b );
	}

	/**
	 * @brief Log2
	 *
	 * @param a		Value whose logarithm is calculated
	 * @return Return the binary logarithm of A
	 */
	static FORCEINLINE float Log2( float a )
	{
		return log2( a );
	}

	/**
	 * @brief Loge
	 *
	 * @param a		Floating point value
	 * @return Return the natural (base-e) logarithm of arg (ln(arg) or loge(arg)) is returned.
	 */
	static FORCEINLINE float Loge( float a )
	{
		return logf( a );
	}

	/**
	 * @brief LogX
	 *
	 * @param base		Base of logarithm
	 * @param value		Value of logarithm
	 * @return Return the logarithm of Log( value ) with base of InBase
	 */
	static FORCEINLINE float LogX( float base, float value )
	{
		return Loge( value ) / Loge( base );
	}

	/**
	 * @brief Converts number with truncation towards zero
	 * 
	 * @param a		Value
	 * @return Rounds A toward zero, returning the nearest integral value that is not larger in magnitude than A
	 */
	static FORCEINLINE float Trunc( float a )
	{
		return truncf( a );
	}

	/**
	 * @brief Round to nearest a floating point number
	 * 
	 * @param a		Value
	 * @return Returns the integral value that is nearest to A, with halfway cases rounded away from zero
	 */
	static FORCEINLINE float Round( float a )
	{
		return roundf( a );
	}

	/**
	 * @brief Sin
	 * @param a		Value representing an angle expressed in radians
	 * @return Returns the sine of an angle of A radians
	 */
	static FORCEINLINE float Sin( float a )
	{
		return sinf( a );
	}

	/**
	 * @brief Cos
	 * @param a		Value representing an angle expressed in radians
	 * @return Returns the cosine of an angle of A radians
	 */
	static FORCEINLINE float Cos( float a )
	{
		return cosf( a );
	}

	/**
	 * @brief ASin
	 * @param a		Value whose arc sine is computed, in the interval [-1,+1].
	 * @return Returns principal arc sine of A, in the interval [-pi/2,+pi/2] radians.
	 */
	static FORCEINLINE float ASin( float a )
	{
		return asinf( a );
	}

	/**
	 * @brief ATan
	 * @param a		Value whose arc tangent is computed
	 * @return Principal arc tangent of A, in the interval [-pi/2,+pi/2] radians
	 */
	static FORCEINLINE float ATan( float a )
	{
		return atanf( a );
	}

	/**
	 * @brief ATan2
	 * @param y		Value representing the proportion of the y-coordinate
	 * @param x		Value representing the proportion of the x-coordinate.
	 * @return Return principal arc tangent of Y/X, in the interval [-pi,+pi] radians
	 */
	static FORCEINLINE float ATan2( float y, float x )
	{
		return atan2f( y, x );
	}

	/**
	 * @brief Sqrt
	 * @param a		Value whose square root is computed
	 * @return Square root of A
	 */
	static FORCEINLINE float Sqrt( float a )
	{
		return sqrtf( a );
	}

	/**
	 * @brief Floor
	 * 
	 * @param a		Value to round down
	 * @return Return the value of A rounded downward (as a floating-point value)
	 */
	static FORCEINLINE float Floor( float a )
	{
		return floorf( a );
	}

	/**
	 * @brief Clamp value in range InMin and InMax
	 *
	 * @param x		Value
	 * @param min	Min range
	 * @param max	Max range
	 * @return Return clamped value
	 */
	static FORCEINLINE float Clamp( float x, float min, float max )
	{
		return ::Clamp( x, min, max );
	}

	/**
	 * @brief Computes the absolute value of an number
	 * @param x		Value
	 * @return Return The absolute value of X (i.e. |X|), if it is representable.
	 */
	static FORCEINLINE float Abs( float x )
	{
		return abs( x );
	}

	/**
	 * @brief Distance between vectors
	 * 
	 * @param vectorA	Vector A
	 * @param veftorB	Vector B
	 * @return Return distance between vectors
	 */
	static FORCEINLINE float DistanceVector( const vec3_t& vectorA, const vec3_t& vectorB )
	{
		return glm::distance( vectorA, vectorB );
	}

	/**
	 * @brief Lerp
	 * 
	 * @param x		Value X
	 * @param y		Value Y
	 * @param a		Alpha
	 * @return Return the linear blend of X and Y using the floating-point value A
	 */
	static FORCEINLINE float Lerp( float x, float y, float a )
	{
		return glm::lerp( x, y, a );
	}

	/**
	 * @brief Snaps a value to the nearest grid multiple
	 * 
	 * @param value	Value
	 * @param grid	Grid
	 * @return Return snapped value to the nearest grid multiple
	 */
	static FORCEINLINE float GridSnap( float value, float grid )
	{
		if ( grid == 0.f )
		{
			return value;
		}
		else
		{
			return Floor( ( value + 0.5f * grid ) / grid ) * grid;
		}
	}

	/**
	 * @brief Snaps a vec2_t to the nearest grid multiple
	 * 
	 * @param value	Value
	 * @param grid	Grid
	 * @return Return snapped vec2_t to the nearest grid multiple
	 */
	static FORCEINLINE vec2_t GridSnap( const vec2_t& value, float grid )
	{
		vec2_t	retVec;
		GridSnap( value, grid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a vec2_t to the nearest grid multiple
	 *
	 * @param value			Value
	 * @param grid			Grid
	 * @param resultValue	Return snapped vec2_t to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const vec2_t& value, float grid, vec2_t& resultValue )
	{
		resultValue = vec2_t( GridSnap( value.x, grid ), GridSnap( value.y, grid ) );
	}

	/**
	 * @brief Snaps a vector to the nearest grid multiple
	 *
	 * @param value	Value
	 * @param grid	Grid
	 * @return Return snapped vector to the nearest grid multiple
	 */
	static FORCEINLINE vec3_t GridSnap( const vec3_t& value, float grid )
	{
		vec3_t	retVec;
		GridSnap( value, grid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a vec3_t to the nearest grid multiple
	 *
	 * @param value			Value
	 * @param grid			Grid
	 * @param resultValue	Return snapped vector to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const vec3_t& value, float grid, vec3_t& resultValue )
	{
		resultValue = vec3_t( GridSnap( value.x, grid ), GridSnap( value.y, grid ), GridSnap( value.z, grid ) );
	}

	/**
	 * @brief Snaps a vec4_t to the nearest grid multiple
	 *
	 * @param value	Value
	 * @param grid	Grid
	 * @return Return snapped vec4_t to the nearest grid multiple
	 */
	static FORCEINLINE vec4_t GridSnap( const vec4_t& value, float grid )
	{
		vec4_t	retVec;
		GridSnap( value, grid, retVec );
		return retVec;
	}

	/**
	 * @brief Snaps a vec4_t to the nearest grid multiple
	 *
	 * @param value			Value
	 * @param grid			Grid
	 * @param resultValue	Return snapped vec4_t to the nearest grid multiple
	 */
	static FORCEINLINE void GridSnap( const vec4_t& value, float grid, vec4_t& resultValue )
	{
		resultValue = vec4_t( GridSnap( value.x, grid ), GridSnap( value.y, grid ), GridSnap( value.z, grid ), GridSnap( value.w, grid ) );
	}

	/**
	 * @brief Checks if a floating point number is nearly zero
	 *	
	 * @param value				Number to compare
	 * @param errorTolerance	Maximum allowed difference for considering value as 'nearly zero'
	 * @return Return TRUE if value is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( float value, float errorTolerance = SMALL_NUMBER )
	{
		return Abs( value ) < errorTolerance;
	}

	/**
	 * @brief Checks if a vec2_t number is nearly zero
	 *
	 * @param value				vec2_t to compare
	 * @param errorTolerance	Maximum allowed difference for considering value as 'nearly zero'
	 * @return Return TRUE if value is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const vec2_t& value, float errorTolerance = SMALL_NUMBER )
	{
		return	Abs( value.x ) < errorTolerance &&
				Abs( value.y ) < errorTolerance;
	}

	/**
	 * @brief Checks if a vector number is nearly zero
	 *
	 * @param value				Vector to compare
	 * @param errorTolerance	Maximum allowed difference for considering value as 'nearly zero'
	 * @return Return TRUE if value is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const vec3_t& value, float errorTolerance = SMALL_NUMBER )
	{
		return	Abs( value.x ) < errorTolerance &&
				Abs( value.y ) < errorTolerance &&
				Abs( value.z ) < errorTolerance;
	}

	/**
	 * @brief Checks if a vec4_t number is nearly zero
	 *
	 * @param value				vec4_t to compare
	 * @param errorTolerance	Maximum allowed difference for considering value as 'nearly zero'
	 * @return Return TRUE if value is nearly zero
	 **/
	static FORCEINLINE bool IsNearlyZero( const vec4_t& value, float errorTolerance = SMALL_NUMBER )
	{
		return	Abs( value.x ) < errorTolerance &&
				Abs( value.y ) < errorTolerance &&
				Abs( value.z ) < errorTolerance &&
				Abs( value.w ) < errorTolerance;
	}

	/**
	 * @brief Calculates the floating-point remainder
	 * 
	 * @param x	Value X
	 * @param y	Value Y
	 * @return Returns the floating-point remainder of x / y
	 */
	static FORCEINLINE float Fmod( float x, float y )
	{
		return fmod( x, y );
	}

	static const vec3_t				vectorZero;			/**< Zero 3D vector */
	static const vec3_t				vectorOne;			/**< One 3D vector */
	static const quat_t				quaternionZero;		/**< quat_t zero */
	static const class CRotator		rotatorZero;		/**< Rotator zero */
	static const matrix_t			matrixIdentity;		/**< Identity matrix */
	static const class CTransform	transformZero;		/**< Transform zero */
	static const vec3_t				vectorForward;		/**< Forward vector */
	static const vec3_t				vectorRight;		/**< Right vector */
	static const vec3_t				vectorUp;			/**< Up vector */
};

#endif // !MATH_H
