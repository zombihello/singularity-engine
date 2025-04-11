/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef BUILDNUM_H
#define BUILDNUM_H

#include "core/types.h"
#include "core/platform.h"

/**
 * @ingroup stdlib
 * @brief Build number counter
 * @param dateOfBeginning	Data of beginning to count build numbers (in days)
 */
template<uint32 dateOfBeginning>
class TBuildNumber
{
public:
	/**
	 * @brief Constructor
	 */
	TBuildNumber()
		: buildNumber( 0 )
	{
		ComputeBuildNumber();
	}

	/**
	 * @brief Get build number
	 * @return Return build number
	 */
	FORCEINLINE uint32 GetBuildNumber() const
	{
		return buildNumber;
	}

private:
	/**
	 * @brief Compute build number
	 */
	void ComputeBuildNumber()
	{
		const char* pDate			= __DATE__;
		const char* month[12]		= { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		const char	month_days[12]	= { 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31 };

		buildNumber		= 0;
		uint32	months	= 0;
		uint32	days	= 0;
		uint32	years	= 0;

		// Compute count days and years
		for ( months = 0; months < 11; ++months )
		{
			if ( S_Strncmp( &pDate[0], month[months], 3 ) == 0 )
			{
				break;
			}
			days += month_days[months];
		}
		days		+= S_Atoi( &pDate[4] ) - 1;
		years		= S_Atoi( &pDate[7] ) - 1900;
		
		// Compute build number
		buildNumber = days + ( uint32 )( ( years - 1 ) * 365.25f );
		if ( ( years % 4 == 0 ) && months > 1 )
		{
			++buildNumber;
		}

		buildNumber -= dateOfBeginning;
	}

	uint32		buildNumber;	/**< Build number */
};

#endif // !BUILDNUM_H