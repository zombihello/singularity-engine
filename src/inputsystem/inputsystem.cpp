#include "pch_inputsystem.h"
#include "tier1/convar.h"
#include "tier1/istreamdata.h"
#include "cvar/icvar.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/iinputsystem.h"

//-----------------------------------------------------------------------------
// Global values and cvars
//-----------------------------------------------------------------------------
CConVar mouse_sensitivity( "mouse_sensitivity", "0.5", "Mouse sensitivity", FCVAR_ARCHIVE );

// Table of button names
static const char* s_pButtonNames[] = {
	"",				// BUTTON_CODE_NONE
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

//-----------------------------------------------------------------------------
// Input system
//-----------------------------------------------------------------------------
class CInputSystem : public CBaseAppSystem<IInputSystem>
{
public:
	CInputSystem();
	~CInputSystem();

	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// IInputSystem interface
	virtual void AttachToWindow( windowId_t windowId ) override;
	virtual void DetachFromWindow() override;

	virtual void ClearInputState() override;

	// Functions set/get console command which binded on a button
	virtual void		SetBinding( buttonCode_t button, const char* pCommand ) override;
	virtual const char* GetBindingCommand( buttonCode_t button ) const override;
	virtual void		UnbindAll() override;

	virtual bool IsKeyDown( buttonCode_t key ) const override;
	virtual bool IsKeyUp( buttonCode_t key ) const override;
	virtual bool IsMouseKeyDown( buttonCode_t key ) const override;
	virtual bool IsMouseKeyUp( buttonCode_t key ) const override;
	virtual bool IsMouseWheel( buttonCode_t wheel ) const override;
	virtual bool IsMouseMoved( buttonCode_t mouseAxis ) const override;

	virtual vec2_t GetMouseLocation() const override;
	virtual vec2_t GetMouseOffset() const override;
	virtual float  GetMouseOffset( buttonCode_t mouseAxis ) const override;
	virtual float  GetMouseSensitivity() const override;

	virtual buttonEvent_t GetButtonEvent( buttonCode_t buttonCode ) const override;
	virtual buttonCode_t  GetButtonCodeByName( const char* pButtonName ) const override;
	virtual const char*	  GetButtonName( buttonCode_t buttonCode ) const override;

	void ExecBindingCommand( buttonCode_t button );

private:
	static void OnInputEvent( void* pUserData, const inputEvent_t& inputEvent );
	static void OnWriteConCmdsToConfigFile( void* pUserData, IStreamDataWriter* pStreamData );

	windowId_t									 windowId;	// A window that was attached the input system
	IWindowMgr::IOnInputEvent::funcDelegate_t*	 pInputEventDelegate;
	IOnWriteConCmdsToConfigFile::funcDelegate_t* pWriteConCmdsDelegate;
	buttonEvent_t								 buttonEvents[BUTTON_CODE_COUNT];
	vec2_t										 mouseLocation;
	vec2_t										 mouseOffset;
	eastl::string								 binds[BUTTON_CODE_COUNT];
};

// Input system singleton
static CInputSystem s_InputSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CInputSystem, IInputSystem, INPUTSYSTEM_INTERFACE_VERSION, s_InputSystem );

/*
==================
CInputSystem::CInputSystem
==================
*/
CInputSystem::CInputSystem()
	: windowId( INVALID_WINDOW_ID )
	, pInputEventDelegate( NULL )
	, pWriteConCmdsDelegate( NULL )
	, mouseLocation( g_vectorZero )
	, mouseOffset( g_vectorZero )
{
	Mem_Memset( &buttonEvents, BUTTON_EVENT_NONE, sizeof( buttonEvent_t ) * BUTTON_CODE_COUNT );
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
	if ( !ConnectTier1( pFactory ) )
	{
		return false;
	}

	g_pWindowMgr = (IWindowMgr*)pFactory( WINDOWMGR_INTERFACE_VERSION );
	if ( !g_pWindowMgr )
	{
		return false;
	}

	ConVar_Register();
	pWriteConCmdsDelegate = g_pCvar->OnWriteConCmdsToConfigFile()->AddFunc( &CInputSystem::OnWriteConCmdsToConfigFile, this );
	return true;
}

/*
==================
CInputSystem::Disconnect
==================
*/
void CInputSystem::Disconnect()
{
	DetachFromWindow();
	if ( pWriteConCmdsDelegate )
	{
		g_pCvar->OnWriteConCmdsToConfigFile()->RemoveFunc( pWriteConCmdsDelegate );
		pWriteConCmdsDelegate = NULL;
	}

	ConVar_Unregister();
	DisconnectTier1();
}

/*
==================
CInputSystem::AttachToWindow
==================
*/
void CInputSystem::AttachToWindow( windowId_t windowId )
{
	CInputSystem::windowId = windowId;
	if ( !pInputEventDelegate )
	{
		pInputEventDelegate = g_pWindowMgr->OnInputEvent()->AddFunc( &CInputSystem::OnInputEvent, this );
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
	windowId = INVALID_WINDOW_ID;
	if ( pInputEventDelegate )
	{
		g_pWindowMgr->OnInputEvent()->RemoveFunc( pInputEventDelegate );
		pInputEventDelegate = NULL;
	}

	Msg( "InputSystem: Detached from a window" );
}

/*
==================
CInputSystem::OnInputEvent
==================
*/
void CInputSystem::OnInputEvent( void* pUserData, const inputEvent_t& inputEvent )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_INPUT );
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
		pInputSystem->buttonEvents[inputEvent.key.code] = BUTTON_EVENT_PRESSED;
		pInputSystem->ExecBindingCommand( inputEvent.key.code );
		break;

		// Key released
	case INPUT_EVENT_TYPE_KEY_RELEASED:
		pInputSystem->buttonEvents[inputEvent.key.code] = BUTTON_EVENT_RELEASED;
		pInputSystem->ExecBindingCommand( inputEvent.key.code );
		break;

		// Mouse pressed
	case INPUT_EVENT_TYPE_MOUSE_PRESSED:
		pInputSystem->buttonEvents[inputEvent.mouseButton.code] = BUTTON_EVENT_PRESSED;
		pInputSystem->ExecBindingCommand( inputEvent.mouseButton.code );
		break;

		// Mouse released
	case INPUT_EVENT_TYPE_MOUSE_RELEASED:
		pInputSystem->buttonEvents[inputEvent.mouseButton.code] = BUTTON_EVENT_RELEASED;
		pInputSystem->ExecBindingCommand( inputEvent.mouseButton.code );
		break;

		// Mouse move
	case INPUT_EVENT_TYPE_MOUSE_MOVE:
		pInputSystem->mouseOffset.x += (float)inputEvent.mouseMove.xDirection;
		pInputSystem->mouseOffset.y += (float)inputEvent.mouseMove.yDirection;
		pInputSystem->mouseLocation.x = (float)inputEvent.mouseMove.x;
		pInputSystem->mouseLocation.y = (float)inputEvent.mouseMove.y;

		if ( pInputSystem->mouseOffset.x != 0.f )
		{
			pInputSystem->buttonEvents[MOUSE_X] = BUTTON_EVENT_MOVED;
			pInputSystem->ExecBindingCommand( MOUSE_X );
		}

		if ( pInputSystem->mouseOffset.y != 0.f )
		{
			pInputSystem->buttonEvents[MOUSE_Y] = BUTTON_EVENT_MOVED;
			pInputSystem->ExecBindingCommand( MOUSE_Y );
		}
		break;

		// Mouse wheel move
	case INPUT_EVENT_TYPE_MOUSE_WHEEL:
	{
		buttonCode_t button				   = inputEvent.mouseWheel.y > 0 ? MOUSE_WHEELUP : MOUSE_WHEELDOWN;
		pInputSystem->buttonEvents[button] = BUTTON_EVENT_SCROLLED;
		pInputSystem->ExecBindingCommand( button );
		break;
	}

		// Text input
	case INPUT_EVENT_TYPE_TEXT_INPUT:
		break;
	}
}

/*
==================
CInputSystem::OnWriteConCmdsToConfigFile
==================
*/
void CInputSystem::OnWriteConCmdsToConfigFile( void* pUserData, IStreamDataWriter* pStreamData )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( pUserData );
	CInputSystem* pInputSystem = (CInputSystem*)pUserData;

	eastl::string buffer;
	buffer += "unbindall\n";
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		const char* pCommand = pInputSystem->GetBindingCommand( (buttonCode_t)index );
		if ( !pCommand || !pCommand[0] )
		{
			continue;
		}

		buffer += S_Sprintf( "bind \"%s\" \"%s\"\n", pInputSystem->GetButtonName( (buttonCode_t)index ), pCommand );
	}

	// Write the buffer into the file
	pStreamData->Write( buffer.data(), buffer.size() * sizeof( char ) );
}

/*
==================
CInputSystem::ClearInputState
==================
*/
void CInputSystem::ClearInputState()
{
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( buttonEvents[index] == BUTTON_EVENT_RELEASED || buttonEvents[index] == BUTTON_EVENT_SCROLLED || buttonEvents[index] == BUTTON_EVENT_MOVED )
		{
			buttonEvents[index] = BUTTON_EVENT_NONE;
		}
	}

	mouseOffset = g_vectorZero;
}

/*
==================
CInputSystem::SetBinding
==================
*/
void CInputSystem::SetBinding( buttonCode_t button, const char* pCommand )
{
	Assert( button < BUTTON_CODE_COUNT );
	binds[button] = pCommand;
}

/*
==================
CInputSystem::UnbindAll
==================
*/
void CInputSystem::UnbindAll()
{
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
	return binds[button].c_str();
}

/*
==================
CInputSystem::ExecCommand
==================
*/
void CInputSystem::ExecBindingCommand( buttonCode_t button )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_INPUT );
	Assert( button < BUTTON_CODE_COUNT );
	if ( !binds[button].empty() )
	{
		g_pCvar->Exec( binds[button].c_str() );
	}
}

/*
==================
CInputSystem::IsKeyDown
==================
*/
bool CInputSystem::IsKeyDown( buttonCode_t key ) const
{
	if ( key < KEY_FIRST || key > KEY_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_PRESSED;
}

/*
==================
CInputSystem::IsKeyUp
==================
*/
bool CInputSystem::IsKeyUp( buttonCode_t key ) const
{
	if ( key < KEY_FIRST || key > KEY_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_RELEASED;
}

/*
==================
CInputSystem::IsMouseKeyDown
==================
*/
bool CInputSystem::IsMouseKeyDown( buttonCode_t key ) const
{
	if ( key < MOUSE_FIRST || key > MOUSE_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_PRESSED;
}

/*
==================
CInputSystem::IsMouseKeyUp
==================
*/
bool CInputSystem::IsMouseKeyUp( buttonCode_t key ) const
{
	if ( key < MOUSE_FIRST || key > MOUSE_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_RELEASED;
}

/*
==================
CInputSystem::IsMouseWheel
==================
*/
bool CInputSystem::IsMouseWheel( buttonCode_t wheel ) const
{
	if ( wheel != MOUSE_WHEELUP || wheel != MOUSE_WHEELDOWN )
	{
		return false;
	}

	return buttonEvents[wheel] == BUTTON_EVENT_SCROLLED;
}

/*
==================
CInputSystem::IsMouseMoved
==================
*/
bool CInputSystem::IsMouseMoved( buttonCode_t mouseAxis ) const
{
	if ( mouseAxis != MOUSE_X || mouseAxis != MOUSE_Y )
	{
		return false;
	}

	return buttonEvents[mouseAxis] == BUTTON_EVENT_MOVED;
}

/*
==================
CInputSystem::GetMouseLocation
==================
*/
vec2_t CInputSystem::GetMouseLocation() const
{
	return mouseLocation;
}

/*
==================
CInputSystem::GetMouseOffset
==================
*/
vec2_t CInputSystem::GetMouseOffset() const
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
CInputSystem::GetButtonEvent
==================
*/
buttonEvent_t CInputSystem::GetButtonEvent( buttonCode_t buttonCode ) const
{
	return buttonEvents[buttonCode];
}

/*
==================
CInputSystem::GetButtonCodeByName
==================
*/
buttonCode_t CInputSystem::GetButtonCodeByName( const char* pButtonName ) const
{
	Assert( pButtonName );
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
	if ( buttonCode == KEY_COUNT || buttonCode == MOUSE_COUNT || buttonCode == BUTTON_CODE_COUNT )
	{
		return "";
	}

	return s_pButtonNames[(uint32)buttonCode];
}

/*
==================
Bind command
==================
*/
CON_COMMAND( bind, "Bind a key", FCVAR_NONE )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_INPUT );
	if ( argc < 2 || !argv )
	{
		Msg( "InputSystem: bind <key> <command> : Attach a command to a key" );
		return;
	}

	// Get button code by name
	buttonCode_t buttonCode = s_InputSystem.GetButtonCodeByName( argv[0] );

	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "InputSystem: bind: \"%s\" isn't a valid key", argv[0] );
		return;
	}

	// Set binding
	s_InputSystem.SetBinding( buttonCode, argv[1] );
	Msg( "InputSystem: bind: \"%s\" = \"%s\"", argv[0], argv[1] );
}

/*
==================
Unbind command
==================
*/
CON_COMMAND( unbind, "Unbind a key", FCVAR_NONE )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_INPUT );
	if ( argc < 1 || !argv )
	{
		Msg( "InputSystem: unbind <key> : Remove commands from a key" );
		return;
	}

	// Get button code by name
	buttonCode_t buttonCode = s_InputSystem.GetButtonCodeByName( argv[0] );

	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "InputSystem: unbind: \"%s\" isn't a valid key", argv[0] );
		return;
	}

	// Unbind a key
	s_InputSystem.SetBinding( buttonCode, "" );
	Msg( "InputSystem: unbind: \"%s\" is unbind", argv[0] );
}

/*
==================
UnbindAll command
==================
*/
CON_COMMAND( unbindall, "Unbind all keys", FCVAR_NONE )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_INPUT );
	s_InputSystem.UnbindAll();
	Msg( "InputSystem: unbindall: All keys has been unbind" );
}
