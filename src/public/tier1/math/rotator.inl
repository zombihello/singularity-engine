#pragma once

/*
==================
CRotator::CRotator
==================
*/
FORCEINLINE CRotator::CRotator()
{
}

/*
==================
CRotator::CRotator
==================
*/
FORCEINLINE CRotator::CRotator( float value )
	: pitch( value )
	, yaw( value )
	, roll( value )
{
}

/*
==================
CRotator::CRotator
==================
*/
FORCEINLINE CRotator::CRotator( float pitch, float yaw, float roll )
	: pitch( pitch )
	, yaw( yaw )
	, roll( roll )
{
}

/*
==================
CRotator::CRotator
==================
*/
FORCEINLINE CRotator::CRotator( const vector3_t& angles )
	: pitch( angles.x )
	, yaw( angles.y )
	, roll( angles.z )
{
}

/*
==================
CRotator::CRotator
==================
*/
FORCEINLINE CRotator::CRotator( const quaternion_t& quaternion )
{
	vector3_t angles;
	S_QuaternionToAngles( quaternion, angles );
	pitch = angles.x;
	yaw	  = angles.y;
	roll  = angles.z;
}

/*
==================
CRotator::Make
==================
*/
FORCEINLINE CRotator CRotator::Make( float value )
{
	return CRotator( value );
}

/*
==================
CRotator::Make
==================
*/
FORCEINLINE CRotator CRotator::Make( float pitch, float yaw, float roll )
{
	return CRotator( pitch, yaw, roll );
}

/*
==================
CRotator::Make
==================
*/
FORCEINLINE CRotator CRotator::Make( const vector3_t& angles )
{
	return CRotator( angles );
}

/*
==================
CRotator::Make
==================
*/
FORCEINLINE CRotator CRotator::Make( const quaternion_t& quaternion )
{
	return CRotator( quaternion );
}

/*
==================
CRotator::IsNearlyZero
==================
*/
FORCEINLINE bool CRotator::IsNearlyZero( float tolerance ) const
{
	return S_Abs( NormalizeAxis( pitch ) ) <= tolerance && S_Abs( NormalizeAxis( yaw ) ) <= tolerance && S_Abs( NormalizeAxis( roll ) ) <= tolerance;
}

/*
==================
CRotator::IsZero
==================
*/
FORCEINLINE bool CRotator::IsZero() const
{
	return ClampAxis( pitch ) == 0.f && ClampAxis( yaw ) == 0.f && ClampAxis( roll ) == 0.f;
}

/*
==================
CRotator::Equals
==================
*/
FORCEINLINE bool CRotator::Equals( const CRotator& other, float tolerance ) const
{
	return S_Abs( NormalizeAxis( pitch - other.pitch ) ) <= tolerance && S_Abs( NormalizeAxis( yaw - other.yaw ) ) <= tolerance && S_Abs( NormalizeAxis( roll - other.roll ) ) <= tolerance;
}

/*
==================
CRotator::Add
==================
*/
FORCEINLINE CRotator CRotator::Add( float deltaPitch, float deltaYaw, float deltaRoll )
{
	pitch += deltaPitch;
	yaw += deltaYaw;
	roll += deltaRoll;
	return *this;
}

/*
==================
CRotator::RotateVector
==================
*/
FORCEINLINE vector3_t CRotator::RotateVector( const vector3_t& vector ) const
{
	vector3_t result;
	RotateVector( vector, result );
	return result;
}

/*
==================
CRotator::RotateVector
==================
*/
FORCEINLINE void CRotator::RotateVector( const vector3_t& vector, vector3_t& result ) const
{
	result = ToQuaternion() * vector;
}

/*
==================
CRotator::ClampAxis
==================
*/
FORCEINLINE float CRotator::ClampAxis( float angle )
{
	// Returns Angle in the range (-360,360)
	angle = S_Fmod( angle, 360.f );
	if ( angle < 0.f )
	{
		// Shift to [0,360) range
		angle += 360.f;
	}

	return angle;
}

/*
==================
CRotator::NormalizeAxis
==================
*/
FORCEINLINE float CRotator::NormalizeAxis( float angle )
{
	// Returns Angle in the range [0,360)
	angle = ClampAxis( angle );
	if ( angle > 180.f )
	{
		// Shift to (-180,180]
		angle -= 360.f;
	}

	return angle;
}

/*
==================
CRotator::Clear
==================
*/
FORCEINLINE void CRotator::Clear()
{
	pitch = 0.f;
	yaw	  = 0.f;
	roll  = 0.f;
}

/*
==================
CRotator::ToQuaternion
==================
*/
FORCEINLINE quaternion_t CRotator::ToQuaternion() const
{
	return S_AnglesToQuaternionYZX( pitch, yaw, roll );
}

/*
==================
CRotator::ToVector3
==================
*/
FORCEINLINE vector3_t CRotator::ToVector3() const
{
	return vector3_t( pitch, yaw, roll );
}

/*
==================
CRotator::Inverse
==================
*/
FORCEINLINE void CRotator::Inverse()
{
	quaternion_t invQuaternion = S_QuaternionInverse( ToQuaternion() );
	vector3_t	 angles		   = S_QuaternionToAngles( invQuaternion );
	pitch					   = angles.x;
	yaw						   = angles.y;
	roll					   = angles.z;
}

/*
==================
CRotator::GetInversed
==================
*/
FORCEINLINE CRotator CRotator::GetInversed() const
{
	CRotator result;
	GetInversed( result );
	return result;
}

/*
==================
CRotator::GetInversed
==================
*/
FORCEINLINE void CRotator::GetInversed( CRotator& result ) const
{
	result = *this;
	result.Inverse();
}

/*
==================
CRotator::Clamp
==================
*/
FORCEINLINE void CRotator::Clamp()
{
	pitch = ClampAxis( pitch );
	yaw	  = ClampAxis( yaw );
	roll  = ClampAxis( roll );
}

/*
==================
CRotator::GetClamped
==================
*/
FORCEINLINE CRotator CRotator::GetClamped() const
{
	CRotator result;
	GetClamped( result );
	return result;
}

/*
==================
CRotator::GetClamped
==================
*/
FORCEINLINE void CRotator::GetClamped( CRotator& result ) const
{
	result = *this;
	result.Clamp();
}

/*
==================
CRotator::Normalize
==================
*/
FORCEINLINE void CRotator::Normalize()
{
	pitch = NormalizeAxis( pitch );
	yaw	  = NormalizeAxis( yaw );
	roll  = NormalizeAxis( roll );
}

/*
==================
CRotator::GetNormalized
==================
*/
FORCEINLINE CRotator CRotator::GetNormalized() const
{
	CRotator result = *this;
	GetNormalized( result );
	return result;
}

/*
==================
CRotator::GetNormalized
==================
*/
FORCEINLINE void CRotator::GetNormalized( CRotator& result ) const
{
	result = *this;
	result.Normalize();
}

/*
==================
CRotator::operator+
==================
*/
FORCEINLINE CRotator CRotator::operator+( const CRotator& right ) const
{
	return CRotator( pitch + right.pitch, yaw + right.yaw, roll + right.roll );
}

/*
==================
CRotator::operator-
==================
*/
FORCEINLINE CRotator CRotator::operator-( const CRotator& right ) const
{
	return CRotator( pitch - right.pitch, yaw - right.yaw, roll - right.roll );
}

/*
==================
CRotator::operator==
==================
*/
FORCEINLINE bool CRotator::operator==( const CRotator& right ) const
{
	return pitch == right.pitch && yaw == right.yaw && roll == right.roll;
}

/*
==================
CRotator::operator!=
==================
*/
FORCEINLINE bool CRotator::operator!=( const CRotator& right ) const
{
	return pitch != right.pitch || yaw != right.yaw || roll != right.roll;
}

/*
==================
CRotator::operator+=
==================
*/
FORCEINLINE CRotator& CRotator::operator+=( const CRotator& right )
{
	pitch += right.pitch;
	yaw += right.yaw;
	roll += right.roll;
	return *this;
}

/*
==================
CRotator::operator-=
==================
*/
FORCEINLINE CRotator& CRotator::operator-=( const CRotator& right )
{
	pitch -= right.pitch;
	yaw -= right.yaw;
	roll -= right.roll;
	return *this;
}
