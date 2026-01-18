#pragma once

/*
==================
CRotator::IsNearlyZero
==================
*/
FORCEINLINE bool CRotator::IsNearlyZero( float tolerance /*= KINDA_SMALL_NUMBER*/ ) const
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
FORCEINLINE bool CRotator::Equals( const CRotator& other, float tolerance /*= KINDA_SMALL_NUMBER*/ ) const
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
CRotator::GetInverse
==================
*/
FORCEINLINE CRotator CRotator::GetInverse() const
{
	return CRotator( S_QuaternionToAngles( S_QuaternionInverse( AsQuaternion() ) ) );
}

/*
==================
CRotator::AsQuaternion
==================
*/
FORCEINLINE quat_t CRotator::AsQuaternion() const
{
	return S_AnglesToQuaternionYZX( pitch, yaw, roll );
}

/*
==================
CRotator::AsEuler
==================
*/
FORCEINLINE vec3_t CRotator::AsEuler() const
{
	return vec3_t( pitch, yaw, roll );
}

/*
==================
CRotator::RotateVector
==================
*/
FORCEINLINE vec3_t CRotator::RotateVector( const vec3_t& vector ) const
{
	return AsQuaternion() * vector;
}

/*
==================
CRotator::Clamp
==================
*/
FORCEINLINE CRotator CRotator::Clamp() const
{
	return CRotator( ClampAxis( pitch ), ClampAxis( yaw ), ClampAxis( roll ) );
}

/*
==================
CRotator::GetNormalized
==================
*/
FORCEINLINE CRotator CRotator::GetNormalized() const
{
	CRotator rotator = *this;
	rotator.Normalize();
	return rotator;
}

/*
==================
CRotator::GetDenormalized
==================
*/
FORCEINLINE CRotator CRotator::GetDenormalized() const
{
	CRotator rotator = *this;
	rotator.pitch	 = ClampAxis( rotator.pitch );
	rotator.yaw		 = ClampAxis( rotator.yaw );
	rotator.roll	 = ClampAxis( rotator.roll );
	return rotator;
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
CRotator::MakeFromEuler
==================
*/
FORCEINLINE CRotator CRotator::MakeFromEuler( const vec3_t& euler )
{
	return CRotator( euler.x, euler.y, euler.z );
}

/*
==================
CRotator::MakeFromEuler
==================
*/
FORCEINLINE CRotator CRotator::MakeFromEuler( float eulerX, float eulerY, float eulerZ )
{
	return CRotator( eulerX, eulerY, eulerZ );
}

/*
==================
CRotator::operator+
==================
*/
FORCEINLINE CRotator CRotator::operator+( const CRotator& other ) const
{
	return CRotator( pitch + other.pitch, yaw + other.yaw, roll + other.roll );
}

/*
==================
CRotator::operator-
==================
*/
FORCEINLINE CRotator CRotator::operator-( const CRotator& other ) const
{
	return CRotator( pitch - other.pitch, yaw - other.yaw, roll - other.roll );
}

/*
==================
CRotator::operator==
==================
*/
FORCEINLINE bool CRotator::operator==( const CRotator& other ) const
{
	return pitch == other.pitch && yaw == other.yaw && roll == other.roll;
}

/*
==================
CRotator::operator!=
==================
*/
FORCEINLINE bool CRotator::operator!=( const CRotator& other ) const
{
	return pitch != other.pitch || yaw != other.yaw || roll != other.roll;
}

/*
==================
CRotator::operator+=
==================
*/
FORCEINLINE CRotator CRotator::operator+=( const CRotator& other )
{
	pitch += other.pitch;
	yaw += other.yaw;
	roll += other.roll;
	return *this;
}

/*
==================
CRotator::operator-=
==================
*/
FORCEINLINE CRotator CRotator::operator-=( const CRotator& other )
{
	pitch -= other.pitch;
	yaw -= other.yaw;
	roll -= other.roll;
	return *this;
}

/*
==================
rotationConversionCache_t::RotatorToQuat
==================
*/
FORCEINLINE quat_t rotationConversionCache_t::RotatorToQuat( const CRotator& rotator ) const
{
	if ( cachedRotator != rotator )
	{
		cachedRotator = rotator.GetNormalized();
		cachedQuat	  = cachedRotator.AsQuaternion();
	}
	return cachedQuat;
}

/*
==================
rotationConversionCache_t::RotatorToQuat_ReadOnly
==================
*/
FORCEINLINE quat_t rotationConversionCache_t::RotatorToQuat_ReadOnly( const CRotator& rotator ) const
{
	if ( cachedRotator == rotator )
	{
		return cachedQuat;
	}
	return rotator.AsQuaternion();
}

/*
==================
rotationConversionCache_t::QuatToRotator
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::QuatToRotator( const quat_t& quat ) const
{
	if ( cachedQuat != quat )
	{
		cachedQuat	  = S_QuaternionNormalize( quat );
		cachedRotator = CRotator( S_QuaternionToAngles( cachedQuat ) );
	}
	return cachedRotator;
}

/*
==================
rotationConversionCache_t::QuatToRotator_ReadOnly
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::QuatToRotator_ReadOnly( const quat_t& quat ) const
{
	if ( cachedQuat == quat )
	{
		return cachedRotator;
	}
	return CRotator( S_QuaternionToAngles( S_QuaternionNormalize( quat ) ) );
}

/*
==================
rotationConversionCache_t::NormalizedQuatToRotator
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::NormalizedQuatToRotator( const quat_t& normalizedQuat ) const
{
	if ( cachedQuat != normalizedQuat )
	{
		cachedQuat	  = normalizedQuat;
		cachedRotator = CRotator( S_QuaternionToAngles( normalizedQuat ) );
	}
	return cachedRotator;
}

/*
==================
rotationConversionCache_t::NormalizedQuatToRotator_ReadOnly
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::NormalizedQuatToRotator_ReadOnly( const quat_t& normalizedQuat ) const
{
	if ( cachedQuat == normalizedQuat )
	{
		return cachedRotator;
	}
	return CRotator( S_QuaternionToAngles( normalizedQuat ) );
}

/*
==================
rotationConversionCache_t::GetCachedQuat
==================
*/
FORCEINLINE quat_t rotationConversionCache_t::GetCachedQuat() const
{
	return cachedQuat;
}

/*
==================
rotationConversionCache_t::GetCachedRotator
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::GetCachedRotator() const
{
	return cachedRotator;
}