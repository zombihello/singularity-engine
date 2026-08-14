#pragma once

/*
==================
userCmd_t::userCmd_t
==================
*/
FORCEINLINE userCmd_t::userCmd_t()
	: viewAngles( 0.f, 0.f, 0.f )
	, forwardMove( 0.f )
	, sideMove( 0.f )
	, upMove( 0.f )
	, actions( USER_ACTION_FLAG_NONE )
{
}

/*
==================
userCmd_t::Reset
==================
*/
FORCEINLINE void userCmd_t::Reset()
{
	forwardMove = 0.f;
	sideMove	= 0.f;
	upMove		= 0.f;
	actions		= USER_ACTION_FLAG_NONE;
	viewAngles.Clear();
}
