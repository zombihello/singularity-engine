#include "pch_inputsystem.h"
#include "tier1/istreamdata.h"
#include "cvar/icmdsystem.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/inputsystem.h"
#include "inputsystem/cvars.h"

CInputSystem g_inputSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CInputSystem, IInputSystem, INPUTSYSTEM_INTERFACE_VERSION, g_inputSystem );

// Table of button names
static const char* s_pButtonNames[] = {
	"0",			// KEY_0
	"1",			// KEY_1
	"2",			// KEY_2
	"3",			// KEY_3
	"4",			// KEY_4
	"5",			// KEY_5
	"6",			// KEY_6
	"7",			// KEY_7
	"8",			// KEY_8
	"9",			// KEY_9
	"a",			// KEY_A
	"b",			// KEY_B
	"c",			// KEY_C
	"d",			// KEY_D
	"e",			// KEY_E
	"f",			// KEY_F
	"g",			// KEY_G
	"h",			// KEY_H
	"i",			// KEY_I
	"j",			// KEY_J
	"k",			// KEY_K
	"l",			// KEY_L
	"m",			// KEY_M
	"n",			// KEY_N
	"o",			// KEY_O
	"p",			// KEY_P
	"q",			// KEY_Q
	"r",			// KEY_R
	"s",			// KEY_S
	"t",			// KEY_T
	"u",			// KEY_U
	"v",			// KEY_V
	"w",			// KEY_W
	"x",			// KEY_X
	"y",			// KEY_Y
	"z",			// KEY_Z
	"kp_0",			// KEYPAD_0
	"kp_1",			// KEYPAD_1
	"kp_2",			// KEYPAD_2
	"kp_3",			// KEYPAD_3
	"kp_4",			// KEYPAD_4
	"kp_5",			// KEYPAD_5
	"kp_6",			// KEYPAD_6
	"kp_7",			// KEYPAD_7
	"kp_8",			// KEYPAD_8
	"kp_9",			// KEYPAD_9
	"kp_slash",		// KEYPAD_DIVIDE
	"kp_multiply",	// KEYPAD_MULTIPLY
	"kp_minus",		// KEYPAD_MINUS
	"kp_plus",		// KEYPAD_PLUS
	"kp_enter",		// KEYPAD_ENTER
	"kp_del",		// KEYPAD_DECIMAL
	"[",			// KEY_LBRACKET
	"]",			// KEY_RBRACKET
	"semicolon",	// KEY_SEMICOLON
	"'",			// KEY_APOSTROPHE
	"`",			// KEY_BACKQUOTE
	",",			// KEY_COMMA
	".",			// KEY_PERIOD
	"/",			// KEY_SLASH
	"\\",			// KEY_BACKSLASH
	"-",			// KEY_MINUS
	"=",			// KEY_EQUAL
	"enter",		// KEY_ENTER
	"space",		// KEY_SPACE
	"backspace",	// KEY_BACKSPACE
	"tab",			// KEY_TAB
	"capslock",		// KEY_CAPSLOCK
	"numlock",		// KEY_NUMLOCK
	"esc",			// KEY_ESCAPE
	"scrolllock",	// KEY_SCROLLLOCK
	"ins",			// KEY_INSERT
	"del",			// KEY_DELETE
	"home",			// KEY_HOME
	"end",			// KEY_END
	"pgup",			// KEY_PAGEUP
	"pgdn",			// KEY_PAGEDOWN
	"pause",		// KEY_BREAK
	"shift",		// KEY_LSHIFT
	"rshift",		// KEY_RSHIFT
	"alt",			// KEY_LALT
	"ralt",			// KEY_RALT
	"ctrl",			// KEY_LCONTROL
	"rctrl",		// KEY_RCONTROL
	"win",			// KEY_LWIN
	"rwin",			// KEY_RWIN
	"app",			// KEY_APP
	"up",			// KEY_UP
	"left",			// KEY_LEFT
	"down",			// KEY_DOWN
	"right",		// KEY_RIGHT
	"f1",			// KEY_F1
	"f2",			// KEY_F2
	"f3",			// KEY_F3
	"f4",			// KEY_F4
	"f5",			// KEY_F5
	"f6",			// KEY_F6
	"f7",			// KEY_F7
	"f8",			// KEY_F8
	"f9",			// KEY_F9
	"f10",			// KEY_F10
	"f11",			// KEY_F11
	"f12",			// KEY_F12

	"mouse1",	   // MOUSE_1
	"mouse2",	   // MOUSE_2
	"mouse3",	   // MOUSE_3
	"mouse4",	   // MOUSE_4
	"mouse5",	   // MOUSE_5
	"mouse6",	   // MOUSE_6
	"mouse7",	   // MOUSE_7
	"mouse8",	   // MOUSE_8
	"mwheelup",	   // MOUSE_WHEELUP
	"mwheeldown",  // MOUSE_WHEELDOWN
	"mousex",	   // MOUSE_X
	"mousey"	   // MOUSE_Y
};
static_assert( ARRAYSIZE( s_pButtonNames ) == BUTTON_CODE_COUNT, "Array size 's_pButtonNames' must be equal to BUTTON_CODE_COUNT" );

/*
==================
CInputSystem::CInputSystem
==================
*/
CInputSystem::CInputSystem()
	: windowId( INVALID_WINDOW_ID )
	, onInputEventHandle( INVALID_HANDLE )
	, mouseLocation( g_vector000 )
	, mouseOffset( g_vector000 )
{
	Mem_Memset( &buttonStates, BUTTON_STATE_FLAG_NONE, BUTTON_CODE_COUNT * sizeof( uint8 ) );
}

/*
==================
CInputSystem::~CInputSystem
==================
*/
CInputSystem::~CInputSystem()
{
	DetachFromWindow();
}

/*
==================
CInputSystem::Connect
==================
*/
bool CInputSystem::Connect( createInterfaceFn_t pFactory )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	g_pWindowMgr = (IWindowMgr*)pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	LinkCmds();
	LinkCVars();
	return true;
}

/*
==================
CInputSystem::Disconnect
==================
*/
void CInputSystem::Disconnect()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	DetachFromWindow();
	UnlinkCVars();
	UnlinkCmds();
	DisconnectTier1();
}

/*
==================
CInputSystem::AttachToWindow
==================
*/
void CInputSystem::AttachToWindow( windowId_t windowId )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	CInputSystem::windowId = windowId;
	if ( onInputEventHandle == INVALID_HANDLE )
	{
		onInputEventHandle = g_pWindowMgr->OnInputEvent()->Subscribe( &CInputSystem::OnInputEvent, this );
	}

	Msg( "InputSystem: Attached to a window (windowId: %i)", windowId );
}

/*
==================
CInputSystem::DetachFromWindow
==================
*/
void CInputSystem::DetachFromWindow()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	if ( onInputEventHandle != INVALID_HANDLE )
	{
		g_pWindowMgr->OnInputEvent()->Unsubscribe( onInputEventHandle );
		onInputEventHandle = INVALID_HANDLE;
	}

	windowId = INVALID_WINDOW_ID;
	Msg( "InputSystem: Detached from a window" );
}

/*
==================
CInputSystem::OnInputEvent
==================
*/
void CInputSystem::OnInputEvent( void* pUserData, const inputEvent_t& inputEvent )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	Assert( pUserData );
	CInputSystem* pInputSystem = (CInputSystem*)pUserData;
	if ( inputEvent.windowId != pInputSystem->windowId )
	{
		return;
	}

	switch ( inputEvent.type )
	{
		// Key pressed
	case INPUT_EVENT_TYPE_KEY_PRESSED:
		if ( Input_IsKeyCode( inputEvent.key.code ) )
		{
			pInputSystem->buttonStates[inputEvent.key.code] |= BUTTON_STATE_FLAG_DOWN | BUTTON_STATE_FLAG_PRESSED;
			pInputSystem->ExecBindingCommand( inputEvent.key.code, BUTTON_STATE_FLAG_PRESSED );
		}
		break;

		// Key pressed repeating
	case INPUT_EVENT_TYPE_KEY_PRESSED_REPEATING:
		if ( Input_IsKeyCode( inputEvent.key.code ) )
		{
			pInputSystem->buttonStates[inputEvent.key.code] |= BUTTON_STATE_FLAG_DOWN | BUTTON_STATE_FLAG_REPEATED;
		}
		break;

		// Key released
	case INPUT_EVENT_TYPE_KEY_RELEASED:
		if ( Input_IsKeyCode( inputEvent.key.code ) )
		{
			uint8& state = pInputSystem->buttonStates[inputEvent.key.code];
			state &= ~BUTTON_STATE_FLAG_DOWN;
			state |= BUTTON_STATE_FLAG_RELEASED;
			pInputSystem->ExecBindingCommand( inputEvent.key.code, BUTTON_STATE_FLAG_RELEASED );
		}
		break;

		// Mouse pressed
	case INPUT_EVENT_TYPE_MOUSE_PRESSED:
		if ( Input_IsMouseCode( inputEvent.mouseButton.code ) )
		{
			pInputSystem->buttonStates[inputEvent.mouseButton.code] |= BUTTON_STATE_FLAG_DOWN | BUTTON_STATE_FLAG_PRESSED;
			pInputSystem->ExecBindingCommand( inputEvent.mouseButton.code, BUTTON_STATE_FLAG_PRESSED );
		}
		break;

		// Mouse double pressed
	case INPUT_EVENT_TYPE_MOUSE_DOUBLE_PRESSED:
		if ( Input_IsMouseCode( inputEvent.mouseButton.code ) )
		{
			pInputSystem->buttonStates[inputEvent.mouseButton.code] |= BUTTON_STATE_FLAG_DOWN | BUTTON_STATE_FLAG_DOUBLE_PRESSED;
			pInputSystem->ExecBindingCommand( inputEvent.mouseButton.code, BUTTON_STATE_FLAG_PRESSED );
		}
		break;

		// Mouse released
	case INPUT_EVENT_TYPE_MOUSE_RELEASED:
		if ( Input_IsMouseCode( inputEvent.mouseButton.code ) )
		{
			uint8& state = pInputSystem->buttonStates[inputEvent.mouseButton.code];
			state &= ~BUTTON_STATE_FLAG_DOWN;
			state |= BUTTON_STATE_FLAG_RELEASED;
			pInputSystem->ExecBindingCommand( inputEvent.mouseButton.code, BUTTON_STATE_FLAG_RELEASED );
		}
		break;

		// Mouse move
	case INPUT_EVENT_TYPE_MOUSE_MOVE:
		pInputSystem->mouseOffset.x += (float)inputEvent.mouseMove.xDirection;
		pInputSystem->mouseOffset.y += (float)inputEvent.mouseMove.yDirection;
		pInputSystem->mouseLocation.x = (float)inputEvent.mouseMove.x;
		pInputSystem->mouseLocation.y = (float)inputEvent.mouseMove.y;

		if ( pInputSystem->mouseOffset.x != 0.f )
		{
			pInputSystem->buttonStates[MOUSE_X] |= BUTTON_STATE_FLAG_MOVED;
		}
		if ( pInputSystem->mouseOffset.y != 0.f )
		{
			pInputSystem->buttonStates[MOUSE_Y] |= BUTTON_STATE_FLAG_MOVED;
		}
		break;

		// Mouse wheel move
	case INPUT_EVENT_TYPE_MOUSE_WHEEL:
	{
		buttonCode_t button = inputEvent.mouseWheel.y > 0 ? MOUSE_WHEELUP : MOUSE_WHEELDOWN;
		pInputSystem->buttonStates[button] |= BUTTON_STATE_FLAG_SCROLLED;
		pInputSystem->ExecBindingCommand( button, BUTTON_STATE_FLAG_SCROLLED );
		break;
	}

		// Text input
	case INPUT_EVENT_TYPE_TEXT_INPUT:
		break;
	}
}

/*
==================
CInputSystem::FrameUpdate
==================
*/
void CInputSystem::FrameUpdate( float deltaTime )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		buttonStates[index] &= BUTTON_STATE_FLAG_DOWN;
	}
	mouseOffset = g_vector000;
}

/*
==================
CInputSystem::WriteBindings
==================
*/
void CInputSystem::WriteBindings( IStreamDataWriter* pStreamData ) const
{
	// Write bindings into a string buffer
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_IO );
	Assert( pStreamData );

	eastl::string buffer;
	buffer += "unbindall\n";
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		const char* pCmd = GetBindingCommand( (buttonCode_t)index );
		if ( !pCmd || !pCmd[0] )
		{
			continue;
		}

		buffer += S_Sprintf( "bind \"%s\" \"%s\"\n", GetButtonName( (buttonCode_t)index ), pCmd );
	}

	// Write the buffer into the stream data
	pStreamData->Write( buffer.data(), buffer.size() * sizeof( char ) );
}

/*
==================
CInputSystem::SetBinding
==================
*/
void CInputSystem::SetBinding( buttonCode_t button, const char* pCommand )
{
	if ( Ensure( Input_IsValidCode( button ) ) )
	{
		binds[button] = pCommand;
	}
}

/*
==================
CInputSystem::UnbindAll
==================
*/
void CInputSystem::UnbindAll()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		binds[index].clear();
	}
}

/*
==================
CInputSystem::GetBindingCommand
==================
*/
const char* CInputSystem::GetBindingCommand( buttonCode_t button ) const
{
	return Ensure( Input_IsValidCode( button ) ) ? binds[button].c_str() : "";
}

/*
==================
CInputSystem::ExecBindingCommand
==================
*/
void CInputSystem::ExecBindingCommand( buttonCode_t button, buttonStateFlag_t state )
{
	// Do nothing if the button hasn't a command
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	Assert( Input_IsValidCode( button ) );
	const eastl::string& command = binds[button];
	if ( command.empty() )
	{
		return;
	}

	// +/- commands: '+cmd' on press, '-cmd' on release; wheel scroll pulses both
	if ( command[0] == '+' )
	{
		const char* pCommand = command.c_str() + 1;
		if ( state == BUTTON_STATE_FLAG_PRESSED || state == BUTTON_STATE_FLAG_SCROLLED )
		{
			g_pCmdSystem->AppendCommandString( CMD_EXECUTION_APPEND_END, va( "+%s %i", pCommand, (uint32)button ) );
		}
		if ( state == BUTTON_STATE_FLAG_RELEASED || state == BUTTON_STATE_FLAG_SCROLLED )
		{
			g_pCmdSystem->AppendCommandString( CMD_EXECUTION_APPEND_END, va( "-%s %i", pCommand, (uint32)button ) );
		}
	}
	// Regular command: fire on press/scroll, never on release
	else if ( state == BUTTON_STATE_FLAG_PRESSED || state == BUTTON_STATE_FLAG_SCROLLED )
	{
		g_pCmdSystem->AppendCommandString( CMD_EXECUTION_APPEND_END, command.c_str() );
	}
}

/*
==================
CInputSystem::WasKeyPressed
==================
*/
bool CInputSystem::WasKeyPressed( buttonCode_t key ) const
{
	return Ensure( Input_IsKeyCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_PRESSED );
}

/*
==================
CInputSystem::WasKeyReleased
==================
*/
bool CInputSystem::WasKeyReleased( buttonCode_t key ) const
{
	return Ensure( Input_IsKeyCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_RELEASED );
}

/*
==================
CInputSystem::IsKeyDown
==================
*/
bool CInputSystem::IsKeyDown( buttonCode_t key ) const
{
	return Ensure( Input_IsKeyCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_DOWN );
}

/*
==================
CInputSystem::WasMousePressed
==================
*/
bool CInputSystem::WasMousePressed( buttonCode_t key ) const
{
	return Ensure( Input_IsMouseCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_PRESSED );
}

/*
==================
CInputSystem::WasMouseDoublePressed
==================
*/
bool CInputSystem::WasMouseDoublePressed( buttonCode_t key ) const
{
	return Ensure( Input_IsMouseCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_DOUBLE_PRESSED );
}

/*
==================
CInputSystem::WasMouseReleased
==================
*/
bool CInputSystem::WasMouseReleased( buttonCode_t key ) const
{
	return Ensure( Input_IsMouseCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_RELEASED );
}

/*
==================
CInputSystem::IsMouseDown
==================
*/
bool CInputSystem::IsMouseDown( buttonCode_t key ) const
{
	return Ensure( Input_IsMouseCode( key ) ) && ( buttonStates[key] & BUTTON_STATE_FLAG_DOWN );
}

/*
==================
CInputSystem::WasMouseWheel
==================
*/
bool CInputSystem::WasMouseWheel( buttonCode_t wheel ) const
{
	return Ensure( Input_IsMouseWheel( wheel ) ) && ( buttonStates[wheel] & BUTTON_STATE_FLAG_SCROLLED );
}

/*
==================
CInputSystem::WasMouseMoved
==================
*/
bool CInputSystem::WasMouseMoved( buttonCode_t mouseAxis ) const
{
	return Ensure( Input_IsMouseAxis( mouseAxis ) ) && ( buttonStates[mouseAxis] & BUTTON_STATE_FLAG_MOVED );
}

/*
==================
CInputSystem::GetMouseLocation
==================
*/
vector2_t CInputSystem::GetMouseLocation() const
{
	return mouseLocation;
}

/*
==================
CInputSystem::GetMouseOffset
==================
*/
vector2_t CInputSystem::GetMouseOffset() const
{
	return mouseOffset;
}

/*
==================
CInputSystem::GetMouseOffset
==================
*/
float CInputSystem::GetMouseOffset( buttonCode_t mouseAxis ) const
{
	Ensure( Input_IsMouseAxis( mouseAxis ) );
	switch ( mouseAxis )
	{
	case MOUSE_X: return mouseOffset.x;
	case MOUSE_Y: return mouseOffset.y;
	default: return 0.f;
	}
}

/*
==================
CInputSystem::GetMouseSensitivity
==================
*/
float CInputSystem::GetMouseSensitivity() const
{
	return mouse_sensitivity.GetFloat();
}

/*
==================
CInputSystem::GetButtonStateFlags
==================
*/
uint8 CInputSystem::GetButtonStateFlags( buttonCode_t buttonCode ) const
{
	return Ensure( Input_IsValidCode( buttonCode ) ) ? buttonStates[buttonCode] : BUTTON_STATE_FLAG_NONE;
}

/*
==================
CInputSystem::GetButtonByName
==================
*/
buttonCode_t CInputSystem::GetButtonByName( const char* pButtonName ) const
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_INPUT );
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( !S_Stricmp( s_pButtonNames[index], pButtonName ) )
		{
			return (buttonCode_t)index;
		}
	}
	return BUTTON_CODE_NONE;
}

/*
==================
CInputSystem::GetButtonName
==================
*/
const char* CInputSystem::GetButtonName( buttonCode_t buttonCode ) const
{
	return Ensure( Input_IsValidCode( buttonCode ) ) ? s_pButtonNames[(uint32)buttonCode] : "";
}
