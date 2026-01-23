#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// A rectangular minimum bounding volume
//-----------------------------------------------------------------------------
class CBox
{
public:
	CBox()
		: bIsValid( false )
	{
	}
	CBox( const vec3_t& min, const vec3_t& max )
		: minLocation( min )
		, maxLocation( max )
		, bIsValid( true )
	{
	}

	static CBox BuildAABB( const vec3_t& location, const vec3_t& size );
	static CBox BuildAABB( const vec3_t& location, const vec3_t& minXYZ, const vec3_t& maxXYZ );

	void Clear();

	bool		  IsValid() const;
	const vec3_t& GetMin() const;
	const vec3_t& GetMax() const;

private:
	vec3_t minLocation;
	vec3_t maxLocation;
	bool   bIsValid;
};

#include "tier1/math/box.inl"