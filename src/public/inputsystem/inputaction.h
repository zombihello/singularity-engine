#pragma once
#include "inputsystem/iinputsystem.h"

//-----------------------------------------------------------------------------
// The macro registers the paired +cmd/-cmd console commands
// that drive a `CInputAction`, so a key bound to '+cmd' presses/releases it.
//
// For example:
// static CInputAction s_forwardAction;
// DEFINE_INPUTACTION_COMMANDS( forward, s_forwardAction, "Move forward" );
//-----------------------------------------------------------------------------
#define DEFINE_INPUTACTION_COMMANDS( Name, InputAction, Description )                               \
	static void CmdExec_##Name##Press( const CCmdArgs& args )                                       \
	{                                                                                               \
		bool bHasKey = args.Argc() > 1 && args.Argv( 1 )[0];                                        \
		InputAction.Press( bHasKey ? (buttonCode_t)S_Atoi( args.Argv( 1 ) ) : BUTTON_CODE_NONE );   \
	}                                                                                               \
	static void CmdExec_##Name##Release( const CCmdArgs& args )                                     \
	{                                                                                               \
		bool bHasKey = args.Argc() > 1 && args.Argv( 1 )[0];                                        \
		InputAction.Release( bHasKey ? (buttonCode_t)S_Atoi( args.Argv( 1 ) ) : BUTTON_CODE_NONE ); \
	}                                                                                               \
	static cmdLink_t s_cmd##Name##Press( "+" #Name, CmdExec_##Name##Press, Description );           \
	static cmdLink_t s_cmd##Name##Release( "-" #Name, CmdExec_##Name##Release, Description );

//-----------------------------------------------------------------------------
// A named action that carries its own state
// NOTE: maxKeys - How many keys may hold one action at once
//-----------------------------------------------------------------------------
template<uint32 maxKeys = 2>
class CInputAction
{
public:
	CInputAction();

	// Add/remove a key holding the action.
	// NOTE: BUTTON_CODE_NONE is keyless press/release
	void Press( buttonCode_t key );
	void Release( buttonCode_t key );

	// Clear the per-frame edges (press/release), keep the down bit.
	// Call once per frame, after the edges for the frame have been read
	void ClearEdges();

	bool  IsDown() const;
	bool  WasPressed() const;
	bool  WasReleased() const;
	float GetState() const;

private:
	uint32 GetKeySlot( buttonCode_t key ) const;

	buttonCode_t keys[maxKeys];
	uint8		 state;
};

#include "inputsystem/inputaction.inl"
