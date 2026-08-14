#include "pch_game_shared.h"
#include "inputsystem/inputaction.h"
#include "game/shared/baseplayer.h"

//-----------------------------------------------------------------------------
// CVars
//-----------------------------------------------------------------------------
static CCVar move_forwardspeed( "move_forwardspeed", "1000", "Forward/back move speed" );
static CCVar move_sidespeed( "move_sidespeed", "1000", "Strafe move speed" );
static CCVar move_upspeed( "move_upspeed", "1000", "Up/down move speed" );
static CCVar move_noclipspeed( "move_noclipspeed", "1000", "Noclip max speed" );
static CCVar move_noclipaccel( "move_noclipaccel", "5", "Noclip acceleration" );
static CCVar move_sprintscale( "move_sprintscale", "3", "Speed multiplier while the speed action is held" );
static CCVar move_friction( "move_friction", "4", "Move friction" );
static CCVar move_stopspeed( "move_stopspeed", "100", "Below this speed friction applies at full strength" );
static CCVar mouse_pitch( "mouse_pitch", "0.022", "Mouse pitch scale (negate to invert)" );
static CCVar mouse_yaw( "mouse_yaw", "0.022", "Mouse yaw scale (negate to invert)" );

//-----------------------------------------------------------------------------
// The movement actions
//-----------------------------------------------------------------------------
static CInputAction s_forwardAction;
static CInputAction s_backAction;
static CInputAction s_moveLeftAction;
static CInputAction s_moveRightAction;
static CInputAction s_moveUpAction;
static CInputAction s_moveDownAction;
static CInputAction s_speedAction;

// Every action, so BuildCmd can clear their per-frame edges in one pass
static CInputAction<>* s_pMovementActions[] = {
	&s_forwardAction,
	&s_backAction,
	&s_moveLeftAction,
	&s_moveRightAction,
	&s_moveUpAction,
	&s_moveDownAction,
	&s_speedAction
};

DEFINE_INPUTACTION_COMMANDS( forward, s_forwardAction, "Move forward" );
DEFINE_INPUTACTION_COMMANDS( back, s_backAction, "Move back" );
DEFINE_INPUTACTION_COMMANDS( moveleft, s_moveLeftAction, "Strafe left" );
DEFINE_INPUTACTION_COMMANDS( moveright, s_moveRightAction, "Strafe right" );
DEFINE_INPUTACTION_COMMANDS( moveup, s_moveUpAction, "Move up" );
DEFINE_INPUTACTION_COMMANDS( movedown, s_moveDownAction, "Move down" );
DEFINE_INPUTACTION_COMMANDS( speed, s_speedAction, "Move fast" );

/*
==================
CBasePlayer::Teleport
==================
*/
void CBasePlayer::Teleport( const vector3_t& location, const CRotator& viewAngles )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	state.location	 = location;
	state.viewAngles = viewAngles;
	state.velocity	 = g_vector000;
}

/*
==================
CBasePlayer::Update
==================
*/
void CBasePlayer::Update( float deltaTime )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	userCmd_t userCmd;
	BuildUserCmd( userCmd, deltaTime );
	Move( userCmd, deltaTime );
}

/*
==================
CBasePlayer::BuildUserCmd
==================
*/
void CBasePlayer::BuildUserCmd( userCmd_t& userCmd, float deltaTime )
{
	// Get a new mouse look
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	userCmd.Reset();

	vector2_t mouseOffset	   = g_pInputSystem->GetMouseOffset();
	float	  mouseSensitivity = g_pInputSystem->GetMouseSensitivity();
	userCmd.viewAngles		   = state.viewAngles;
	userCmd.viewAngles.yaw += mouseOffset.x * mouseSensitivity * mouse_yaw.GetFloat();
	userCmd.viewAngles.pitch += mouseOffset.y * mouseSensitivity * mouse_pitch.GetFloat();
	userCmd.viewAngles.pitch = S_Clamp( userCmd.viewAngles.pitch, -89.f, 89.f );  // Clamp so the view never flips over the poles
	userCmd.viewAngles.roll	 = 0.f;

	// Get movement speed
	userCmd.forwardMove = move_forwardspeed.GetFloat() * ( s_forwardAction.GetState() - s_backAction.GetState() );
	userCmd.sideMove	= move_sidespeed.GetFloat() * ( s_moveRightAction.GetState() - s_moveLeftAction.GetState() );
	userCmd.upMove		= move_upspeed.GetFloat() * ( s_moveUpAction.GetState() - s_moveDownAction.GetState() );
	if ( s_speedAction.IsDown() )
	{
		userCmd.actions |= USER_ACTION_FLAG_SPEED;
	}

	// Consume this frame's edges (pressed/released) now that they've been read
	for ( uint32 index = 0; index < ARRAYSIZE( s_pMovementActions ); ++index )
	{
		s_pMovementActions[index]->ClearEdges();
	}
}

/*
==================
CBasePlayer::Move
==================
*/
void CBasePlayer::Move( const userCmd_t& userCmd, float deltaTime )
{
	// Apply a new mouse look
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	state.viewAngles = userCmd.viewAngles;

	// Apply movement
	switch ( state.moveType )
	{
	case PLAYER_MOVETYPE_NOCLIP:
	default:
		MoveNoClip( userCmd, deltaTime );
		break;
	}
}

/*
==================
CBasePlayer::MoveNoClip
==================
*/
void CBasePlayer::MoveNoClip( const userCmd_t& userCmd, float deltaTime )
{
	// Calculate a view basis.
	// Forward/right the movement steers along the same basis the renderer uses for
	// the camera, so motion matches where you look
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_GAMELOGIC );
	quaternion_t viewRotationQuat = userCmd.viewAngles.ToQuaternion();
	vector3_t	 forward		  = S_VectorNormalize( viewRotationQuat * g_vectorForward );
	vector3_t	 right			  = S_VectorNormalize( viewRotationQuat * g_vectorRight );

	// Calculate wish velocity and the maximum speed taking into account sprint
	float	  sprintScale  = ( userCmd.actions & USER_ACTION_FLAG_SPEED ) ? move_sprintscale.GetFloat() : 1.f;
	float	  maxSpeed	   = move_noclipspeed.GetFloat() * sprintScale;
	vector3_t wishVelocity = ( forward * userCmd.forwardMove + right * userCmd.sideMove + g_vectorUp * userCmd.upMove ) * sprintScale;

	// Calculate wish speed and direction
	float	  wishSpeed = S_VectorLength( wishVelocity );
	vector3_t wishDir	= wishSpeed > SMALL_NUMBER ? wishVelocity / wishSpeed : g_vector000;
	wishSpeed			= S_Min( wishSpeed, maxSpeed );

	// Calculate acceleration.
	// Steer velocity toward `wishDir`, but never push the wish-dir component past
	// `wishSpeed` (Only the missing amount along `wishDir` is added, so turning doesn't add speed)
	float missingSpeed = wishSpeed - S_VectorDotProduct( state.velocity, wishDir );
	if ( missingSpeed > 0.f )
	{
		state.velocity += wishDir * S_Min( missingSpeed, move_noclipaccel.GetFloat() * wishSpeed * deltaTime );
	}

	// Apply friction.
	// Bleed speed toward zero. Below `move_stopspeed` the drop is computed against `move_stopspeed`
	// so slow motion comes to a firm stop instead of creeping
	float speed = S_VectorLength( state.velocity );
	if ( speed >= 0.1f )
	{
		float dropSpeed = S_Max( speed, move_stopspeed.GetFloat() ) * move_friction.GetFloat() * deltaTime;
		float newSpeed	= S_Max( speed - dropSpeed, 0.f );
		state.velocity *= newSpeed / speed;
	}
	else
	{
		// Treat velocity below this as fully stopped
		state.velocity = g_vector000;
	}

	// Apply the final movement
	state.location += state.velocity * deltaTime;
}
