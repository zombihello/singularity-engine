#pragma once

/*
==================
CAABB::CAABB
==================
*/
FORCEINLINE CAABB::CAABB()
	: bValid( false )
{
}

/*
==================
CAABB::CAABB
==================
*/
FORCEINLINE CAABB::CAABB( const vector3_t& minLocation, const vector3_t& maxLocation )
	: minLocation( minLocation )
	, maxLocation( maxLocation )
	, bValid( true )
{
}

/*
==================
CAABB::Make
==================
*/
FORCEINLINE CAABB CAABB::Make( const vector3_t& location, const vector3_t& size )
{
	return CAABB( location - size, location + size );
}

/*
==================
CAABB::Make
==================
*/
FORCEINLINE CAABB CAABB::Make( const vector3_t& location, const vector3_t& minXYZ, const vector3_t& maxXYZ )
{
	return CAABB( location + minXYZ, location + maxXYZ );
}

/*
==================
CAABB::Set
==================
*/
FORCEINLINE void CAABB::Set( const vector3_t& location, const vector3_t& size )
{
	minLocation = location - size;
	maxLocation = location + size;
	bValid		= true;
}

/*
==================
CAABB::Set
==================
*/
FORCEINLINE void CAABB::Set( const vector3_t& location, const vector3_t& minXYZ, const vector3_t& maxXYZ )
{
	minLocation = location + minXYZ;
	maxLocation = location + maxXYZ;
	bValid		= true;
}

/*
==================
CAABB::Clear
==================
*/
FORCEINLINE void CAABB::Clear()
{
	bValid = false;
}

/*
==================
CAABB::IsInside
==================
*/
FORCEINLINE bool CAABB::IsInside( const vector3_t& location ) const
{
	return S_VectorAll( S_VectorGreaterThanEqual( location, minLocation ) ) && S_VectorAll( S_VectorLessThanEqual( location, maxLocation ) );
}

/*
==================
CAABB::IsValid
==================
*/
FORCEINLINE bool CAABB::IsValid() const
{
	return bValid;
}

/*
==================
CAABB::GetMin
==================
*/
FORCEINLINE const vector3_t& CAABB::GetMin() const
{
	return minLocation;
}

/*
==================
CAABB::GetMax
==================
*/
FORCEINLINE const vector3_t& CAABB::GetMax() const
{
	return maxLocation;
}
