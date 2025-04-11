/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef BOX_H
#define BOX_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
 * @brief A rectangular minimum bounding volume
 */
class CBox
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CBox()
		: bIsValid( false )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param min	Min position
	 * @param max	Max position
	 */
	FORCEINLINE CBox( const vec3_t& min, const vec3_t& max )
		: minLocation( min )
		, maxLocation( max )
		, bIsValid( true )
	{}

	/**
	 * @brief Utility function to build an AABB from InLocation and InSize
	 * 
	 * @param location	Location of AABB
	 * @param size		Size of AABB
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const vec3_t& location, const vec3_t& size )
	{
		return CBox( location - size, location + size );
	}

	/**
	 * @brief Utility function to build an AABB from InLocation, InMinXYZ and InMaxXYZ
	 *
	 * @param location	Location of AABB
	 * @param minXYZ	Minimum by XYZ
	 * @param maxXYZ	Maximum by XYZ
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const vec3_t& location, const vec3_t& minXYZ, const vec3_t& maxXYZ )
	{
		return CBox( location + minXYZ, location + maxXYZ );
	}

	/**
	 * @brief Clear bounding box
	 */
	FORCEINLINE void Clear()
	{
		bIsValid = false;
	}

	/**
	 * @brief Get min
	 * @return Return min
	 */
	FORCEINLINE const vec3_t& GetMin() const
	{
		return minLocation;
	}

	/**
	 * @brief Get max
	 * @return Return max
	 */
	FORCEINLINE const vec3_t& GetMax() const
	{
		return maxLocation;
	}

	/**
	 * @brief Is valid AABB
	 * @return Return true if AABB is valid, else returning false
	 */
	FORCEINLINE bool IsValid() const
	{
		return bIsValid;
	}

private:
	vec3_t	minLocation;		/**< Min position */
	vec3_t	maxLocation;		/**< Max position */
	bool	bIsValid;			/**< Is valid box */
};

#endif // !BOX_H