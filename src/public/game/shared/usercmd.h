#pragma once
#include "tier0/types.h"
#include "tier1/math/math.h"
#include "tier1/math/rotator.h"

//-----------------------------------------------------------------------------
// Action intent bits (what the controller wants this frame)
//-----------------------------------------------------------------------------
enum userActionFlag_t
{
	USER_ACTION_FLAG_NONE  = 0,
	USER_ACTION_FLAG_SPEED = BIT( 0 ),
};

//-----------------------------------------------------------------------------
// A per-frame snapshot of the controller's intent. Produced by some input
// source (human/AI/replay/network) and consumed by the movement. The
// movement never looks at a device - only at this
//-----------------------------------------------------------------------------
struct userCmd_t
{
	userCmd_t();
	void Reset();

	CRotator viewAngles;   // Desired view orientation
	float	 forwardMove;  // Desired move intent along view forward (units/sec)
	float	 sideMove;	   // Desired move intent along view right (units/sec)
	float	 upMove;	   // Desired move intent along world up (units/sec)
	uint8	 actions;	   // userActionFlag_t bitmask
};

#include "game/shared/usercmd.inl"
