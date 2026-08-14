#pragma once
#include "tier1/math/math.h"
#include "tier1/math/rotator.h"
#include "inputsystem/iinputsystem.h"
#include "game/shared/usercmd.h"

//-----------------------------------------------------------------------------
// Player movement modes
//-----------------------------------------------------------------------------
enum playerMoveType_t
{
	PLAYER_MOVETYPE_NOCLIP	// Free-fly, no collision
};

//-----------------------------------------------------------------------------
// Pure player state
//-----------------------------------------------------------------------------
struct playerState_t
{
	playerState_t();

	vector3_t		 location;
	vector3_t		 velocity;
	CRotator		 viewAngles;
	playerMoveType_t moveType;
};

//-----------------------------------------------------------------------------
// A base player class
//-----------------------------------------------------------------------------
class CBasePlayer
{
public:
	virtual void Teleport( const vector3_t& location, const CRotator& viewAngles );
	virtual void Update( float deltaTime );

	const playerState_t& GetState() const;

protected:
	virtual void BuildUserCmd( userCmd_t& userCmd, float deltaTime );
	virtual void Move( const userCmd_t& userCmd, float deltaTime );

	playerState_t state;

private:
	void MoveNoClip( const userCmd_t& userCmd, float deltaTime );
};

#include "game/shared/baseplayer.inl"
