#ifndef BUILDNUM_H
#define BUILDNUM_H

#include "core/types.h"
#include "core/platform.h"

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

	uint32		buildNumber;
};

#include "stdlib/buildnum.inl"

#endif // !BUILDNUM_H