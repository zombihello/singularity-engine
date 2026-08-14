#pragma once

/*
==================
CTimerWindows::CTimerWindows
==================
*/
FORCEINLINE CTimerWindows::CTimerWindows()
	: ticksPerSecond( 0 )
	, secondsPerTick( 0.0 )
	, startSecondsNegated( 0.0 )
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency( &frequency );
	ticksPerSecond = frequency.QuadPart;
	secondsPerTick = 1.0 / ticksPerSecond;

	Reset();
}

/*
==================
CTimerWindows::Reset
==================
*/
FORCEINLINE void CTimerWindows::Reset()
{
	uint64 ticks = 0;
	GetTicks( ticks );
	startSecondsNegated = -( ticks * secondsPerTick );
}

/*
==================
CTimerWindows::GetFrequency
==================
*/
FORCEINLINE uint64 CTimerWindows::GetFrequency() const
{
	return ticksPerSecond;
}

/*
==================
CTimerWindows::GetFrequency
==================
*/
FORCEINLINE void CTimerWindows::GetFrequency( uint64& frequency ) const
{
	frequency = ticksPerSecond;
}

/*
==================
CTimerWindows::GetSeconds
==================
*/
FORCEINLINE double CTimerWindows::GetSeconds() const
{
	double result = 0.0;
	GetSeconds( result );
	return result;
}

/*
==================
CTimerWindows::GetSeconds
==================
*/
FORCEINLINE void CTimerWindows::GetSeconds( double& seconds ) const
{
	uint64 ticks = 0;
	GetTicks( ticks );
	seconds = ( ticks * secondsPerTick ) + startSecondsNegated;
}

/*
==================
CTimerWindows::GetTicks
==================
*/
FORCEINLINE uint64 CTimerWindows::GetTicks()
{
	uint64 result = 0;
	GetTicks( result );
	return result;
}

/*
==================
CTimerWindows::GetTicks
==================
*/
FORCEINLINE void CTimerWindows::GetTicks( uint64& time )
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	time = counter.QuadPart;
}
