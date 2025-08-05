#pragma once

/*
==================
CBox::BuildAABB
==================
*/
FORCEINLINE CBox CBox::BuildAABB( const vec3_t& location, const vec3_t& size )
{
	return CBox( location - size, location + size );
}

/*
==================
CBox::BuildAABB
==================
*/
FORCEINLINE CBox CBox::BuildAABB( const vec3_t& location, const vec3_t& minXYZ, const vec3_t& maxXYZ )
{
	return CBox( location + minXYZ, location + maxXYZ );
}

/*
==================
CBox::Clear
==================
*/
FORCEINLINE void CBox::Clear()
{
	bIsValid = false;
}

/*
==================
CBox::GetMin
==================
*/
FORCEINLINE const vec3_t& CBox::GetMin() const
{
	return minLocation;
}

/*
==================
CBox::GetMax
==================
*/
FORCEINLINE const vec3_t& CBox::GetMax() const
{
	return maxLocation;
}

/*
==================
CBox::IsValid
==================
*/
FORCEINLINE bool CBox::IsValid() const
{
	return bIsValid;
}
