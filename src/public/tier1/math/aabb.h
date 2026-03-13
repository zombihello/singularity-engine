#pragma once
#include "tier1/math/math.h"

//-----------------------------------------------------------------------------
// Axis-Aligned Bounding Box
//-----------------------------------------------------------------------------
class CAABB
{
public:
	CAABB();
	CAABB( const vector3_t& minLocation, const vector3_t& maxLocation );

	static CAABB Make( const vector3_t& location, const vector3_t& size );
	static CAABB Make( const vector3_t& location, const vector3_t& minXYZ, const vector3_t& maxXYZ );

	void Set( const vector3_t& location, const vector3_t& size );
	void Set( const vector3_t& location, const vector3_t& minXYZ, const vector3_t& maxXYZ );
	void Clear();

	bool			 IsInside( const vector3_t& location ) const;
	bool			 IsValid() const;
	const vector3_t& GetMin() const;
	const vector3_t& GetMax() const;

private:
	vector3_t minLocation;
	vector3_t maxLocation;
	bool	  bValid;
};

#include "tier1/math/aabb.inl"
