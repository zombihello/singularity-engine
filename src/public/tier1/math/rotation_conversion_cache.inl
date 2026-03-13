#pragma once

/*
==================
rotationConversionCache_t::RotatorToQuaternion
==================
*/
FORCEINLINE quaternion_t rotationConversionCache_t::RotatorToQuaternion( const CRotator& rotator ) const
{
	if ( cachedRotator != rotator )
	{
		rotator.GetNormalized( cachedRotator );
		cachedQuaternion = cachedRotator.ToQuaternion();
	}
	return cachedQuaternion;
}

/*
==================
rotationConversionCache_t::QuaternionToRotator
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::QuaternionToRotator( const quaternion_t& quaternion ) const
{
	if ( cachedQuaternion != quaternion )
	{
		S_QuaternionNormalize( quaternion, cachedQuaternion );
		cachedRotator = CRotator::Make( S_QuaternionToAngles( cachedQuaternion ) );
	}
	return cachedRotator;
}

/*
==================
rotationConversionCache_t::NormalizedQuaternionToRotator
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::NormalizedQuaternionToRotator( const quaternion_t& normalizedQuaternion ) const
{
	if ( cachedQuaternion != normalizedQuaternion )
	{
		cachedQuaternion = normalizedQuaternion;
		cachedRotator	 = CRotator::Make( S_QuaternionToAngles( cachedQuaternion ) );
	}
	return cachedRotator;
}

/*
==================
rotationConversionCache_t::RotatorToQuaternion_ReadOnly
==================
*/
FORCEINLINE quaternion_t rotationConversionCache_t::RotatorToQuaternion_ReadOnly( const CRotator& rotator ) const
{
	if ( cachedRotator == rotator )
	{
		return cachedQuaternion;
	}
	return rotator.ToQuaternion();
}

/*
==================
rotationConversionCache_t::QuaternionToRotator_ReadOnly
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::QuaternionToRotator_ReadOnly( const quaternion_t& quaternion ) const
{
	if ( cachedQuaternion == quaternion )
	{
		return cachedRotator;
	}
	return CRotator::Make( S_QuaternionToAngles( S_QuaternionNormalize( quaternion ) ) );
}

/*
==================
rotationConversionCache_t::NormalizedQuaternionToRotator_ReadOnly
==================
*/
FORCEINLINE CRotator rotationConversionCache_t::NormalizedQuaternionToRotator_ReadOnly( const quaternion_t& normalizedQuaternion ) const
{
	if ( cachedQuaternion == normalizedQuaternion )
	{
		return cachedRotator;
	}
	return CRotator::Make( S_QuaternionToAngles( normalizedQuaternion ) );
}

/*
==================
rotationConversionCache_t::GetCachedQuaternion
==================
*/
FORCEINLINE const quaternion_t& rotationConversionCache_t::GetCachedQuaternion() const
{
	return cachedQuaternion;
}

/*
==================
rotationConversionCache_t::GetCachedRotator
==================
*/
FORCEINLINE const CRotator& rotationConversionCache_t::GetCachedRotator() const
{
	return cachedRotator;
}
