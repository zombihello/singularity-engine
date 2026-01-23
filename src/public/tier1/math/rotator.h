#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// Implements a container for rotation information
// NOTE: All rotation values are stored in degrees
//-----------------------------------------------------------------------------
class CRotator
{
public:
	CRotator()
		: pitch( 0.f )
		, yaw( 0.f )
		, roll( 0.f )
	{
	}
	CRotator( float value )
		: pitch( value )
		, yaw( value )
		, roll( value )
	{
	}
	CRotator( float pitch, float yaw, float roll )
		: pitch( pitch )
		, yaw( yaw )
		, roll( roll )
	{
	}
	CRotator( const vec3_t& eulerAngles )
		: pitch( eulerAngles.x )
		, yaw( eulerAngles.y )
		, roll( eulerAngles.z )
	{
	}
	CRotator( const quat_t& quaternion )
	{
		vec3_t eulerAngles = S_QuaternionToAngles( quaternion );
		pitch			   = eulerAngles.x;
		yaw				   = eulerAngles.y;
		roll			   = eulerAngles.z;
	}

	// Convert a vector of floating-point Euler angles (in degrees) into a rotator. Rotator now stored in degrees
	static CRotator MakeFromEuler( const vec3_t& euler );
	static CRotator MakeFromEuler( float eulerX, float eulerY, float eulerZ );

	// Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation
	// This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	bool IsNearlyZero( float tolerance = KINDA_SMALL_NUMBER ) const;

	// Checks whether this has exactly zero rotation, when treated as an orientation
	// This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	bool IsZero() const;

	// Checks whether two rotators are equal within specified tolerance, when treated as an orientation
	// This means that CRotator(0, 0, 360).Equals(CRotator(0,0,0)) is TRUE, because they represent the same final orientation
	bool Equals( const CRotator& other, float tolerance = KINDA_SMALL_NUMBER ) const;

	CRotator Add( float deltaPitch, float deltaYaw, float deltaRoll );
	vec3_t	 RotateVector( const vec3_t& vector ) const;

	// Gets the rotation values so they fall within the range [0,360]
	CRotator Clamp() const;

	// In-place normalize, removes all winding and creates the "shortest route" rotation
	void Normalize();

	// Clamps an angle to the range of [0, 360)
	static float ClampAxis( float angle );

	// Clamps an angle to the range of (-180, 180]
	static float NormalizeAxis( float angle );

	quat_t	 AsQuaternion() const;
	vec3_t	 AsEuler() const;
	CRotator GetInverse() const;

	// Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation
	CRotator GetNormalized() const;

	// Create a copy of this rotator and denormalize, clamping each axis to 0 - 360
	CRotator GetDenormalized() const;

	CRotator operator+( const CRotator& other ) const;
	CRotator operator-( const CRotator& other ) const;
	bool	 operator==( const CRotator& other ) const;
	bool	 operator!=( const CRotator& other ) const;
	CRotator operator+=( const CRotator& other );
	CRotator operator-=( const CRotator& other );

	float pitch;  // Rotation around the right axis
	float yaw;	  // Rotation around the up axis
	float roll;	  // Rotation around the forward axis
};

//-----------------------------------------------------------------------------
// Struct for caching Quaternion <-> Rotator conversions
//-----------------------------------------------------------------------------
struct rotationConversionCache_t
{
	rotationConversionCache_t()
		: cachedQuat( g_quaternionZero )
		, cachedRotator( g_rotatorZero )
	{
	}

	// Uses the cached conversion if possible, and updates it if there was no match
	quat_t	 RotatorToQuat( const CRotator& rotator ) const;
	CRotator QuatToRotator( const quat_t& quat ) const;
	CRotator NormalizedQuatToRotator( const quat_t& normalizedQuat ) const;

	// Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	quat_t	 RotatorToQuat_ReadOnly( const CRotator& rotator ) const;
	CRotator QuatToRotator_ReadOnly( const quat_t& quat ) const;
	CRotator NormalizedQuatToRotator_ReadOnly( const quat_t& normalizedQuat ) const;

	quat_t	 GetCachedQuat() const;
	CRotator GetCachedRotator() const;

private:
	mutable quat_t	 cachedQuat;
	mutable CRotator cachedRotator;
};

#include "tier1/math/rotator.inl"