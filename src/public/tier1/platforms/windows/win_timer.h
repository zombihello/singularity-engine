#pragma once
#include "tier0/types.h"
#include "tier0/defines.h"

//-----------------------------------------------------------------------------
// A windows timer
//-----------------------------------------------------------------------------
class CTimerWindows
{
public:
	CTimerWindows();

	void   Reset();
	uint64 GetFrequency() const;
	void   GetFrequency( uint64& frequency ) const;
	double GetSeconds() const;
	void   GetSeconds( double& seconds ) const;

	static uint64 GetTicks();
	static void	  GetTicks( uint64& time );

private:
	uint64 ticksPerSecond;
	double secondsPerTick;
	double startSecondsNegated;
};

#include "tier1/platforms/windows/win_timer.inl"
