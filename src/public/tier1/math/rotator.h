#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// Implements a container for rotation information
// NOTE: All rotation values are stored in degrees
//-----------------------------------------------------------------------------
class CRotator
{
public:
	CRotator();
	CRotator( float value );
	CRotator( float pitch, float yaw, float roll );
	CRotator( const vector3_t& angles );
	CRotator( const quaternion_t& quaternion );

	static CRotator Make( float value );
	static CRotator Make( float pitch, float yaw, float roll );
	static CRotator Make( const vector3_t& angles );
	static CRotator Make( const quaternion_t& quaternion );

	// Clamps an angle to the range of [0, 360)
	static float ClampAxis( float angle );

	// Clamps an angle to the range of (-180, 180]
	static float NormalizeAxis( float angle );

	// Checks whether rotator is nearly zero within specified tolerance, when treated as an orientation
	// This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	bool IsNearlyZero( float tolerance = KINDA_SMALL_NUMBER ) const;

	// Checks whether this has exactly zero rotation, when treated as an orientation
	// This means that CRotator(0, 0, 360) is "zero", because it is the same final orientation as the zero rotator
	bool IsZero() const;

	// Checks whether two rotators are equal within specified tolerance, when treated as an orientation
	// This means that CRotator(0, 0, 360).Equals(CRotator(0,0,0)) is TRUE, because they represent the same final orientation
	bool Equals( const CRotator& other, float tolerance = KINDA_SMALL_NUMBER ) const;

	CRotator  Add( float deltaPitch, float deltaYaw, float deltaRoll );
	vector3_t RotateVector( const vector3_t& vector ) const;
	void	  RotateVector( const vector3_t& vector, vector3_t& result ) const;

	quaternion_t ToQuaternion() const;
	vector3_t	 ToVector3() const;

	void	 Inverse();
	CRotator GetInversed() const;
	void	 GetInversed( CRotator& result ) const;

	// Gets the rotation values so they fall within the range [0,360]
	void	 Clamp();
	CRotator GetClamped() const;
	void	 GetClamped( CRotator& result ) const;

	// Create a copy of this rotator and normalize, removes all winding and creates the "shortest route" rotation
	void	 Normalize();
	CRotator GetNormalized() const;
	void	 GetNormalized( CRotator& result ) const;

	CRotator  operator+( const CRotator& right ) const;
	CRotator  operator-( const CRotator& right ) const;
	bool	  operator==( const CRotator& right ) const;
	bool	  operator!=( const CRotator& right ) const;
	CRotator& operator+=( const CRotator& right );
	CRotator& operator-=( const CRotator& right );

	float pitch;  // Rotation around the right axis
	float yaw;	  // Rotation around the up axis
	float roll;	  // Rotation around the forward axis
};

#include "tier1/math/rotator.inl"
