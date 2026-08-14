#pragma once

/*
==================
playerState_t::playerState_t
==================
*/
FORCEINLINE playerState_t::playerState_t()
	: location( 0.f, 0.f, 0.f )
	, velocity( 0.f, 0.f, 0.f )
	, viewAngles( 0.f, 0.f, 0.f )
	, moveType( PLAYER_MOVETYPE_NOCLIP )
{
}

/*
==================
CBasePlayer::GetState
==================
*/
FORCEINLINE const playerState_t& CBasePlayer::GetState() const
{
	return state;
}
