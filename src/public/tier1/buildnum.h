#pragma once
#include "tier1/defines.h"
#include "tier1/types.h"

//-----------------------------------------------------------------------------
// Build number counter
// dateOfBeginning	Data of beginning to count build numbers (in days)
//-----------------------------------------------------------------------------
template<uint32 dateOfBeginning>
class TBuildNumber
{
public:
	TBuildNumber()
		: buildNumber( 0 )
	{
		ComputeBuildNumber();
	}

	uint32 GetBuildNumber() const;

private:
	void ComputeBuildNumber();

	uint32 buildNumber;
};

#include "tier1/buildnum.inl"