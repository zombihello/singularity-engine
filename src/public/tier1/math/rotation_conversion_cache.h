#pragma once
#include "tier1/math/rotator.h"

//-----------------------------------------------------------------------------
// Struct for caching quaternion_t <-> CRotator conversions
//-----------------------------------------------------------------------------
struct rotationConversionCache_t
{
	// Uses the cached conversion if possible, and updates it if there was no match
	quaternion_t RotatorToQuaternion( const CRotator& rotator ) const;
	CRotator	 QuaternionToRotator( const quaternion_t& quaternion ) const;
	CRotator	 NormalizedQuaternionToRotator( const quaternion_t& normalizedQuaternion ) const;

	// Uses the cached conversion if possible, but does *NOT* update the cache if there was no match
	quaternion_t RotatorToQuaternion_ReadOnly( const CRotator& rotator ) const;
	CRotator	 QuaternionToRotator_ReadOnly( const quaternion_t& quaternion ) const;
	CRotator	 NormalizedQuaternionToRotator_ReadOnly( const quaternion_t& normalizedQuaternion ) const;

	const quaternion_t& GetCachedQuaternion() const;
	const CRotator&		GetCachedRotator() const;

private:
	mutable quaternion_t cachedQuaternion;
	mutable CRotator	 cachedRotator;
};

#include "tier1/math/rotation_conversion_cache.inl"
