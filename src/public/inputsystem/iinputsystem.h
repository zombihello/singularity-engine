#pragma once
#include "tier1/math/math.h"
#include "appframework/iappsystem.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/buttoncode.h"

//-----------------------------------------------------------------------------
// Button events
//-----------------------------------------------------------------------------
enum buttonEvent_t
{
	BUTTON_EVENT_NONE,
	BUTTON_EVENT_PRESSED,
	BUTTON_EVENT_RELEASED,
	BUTTON_EVENT_SCROLLED,
	BUTTON_EVENT_MOVED,
};

//-----------------------------------------------------------------------------
// Input system interface
//-----------------------------------------------------------------------------
#define INPUTSYSTEM_INTERFACE_VERSION "SInputSystem001"
class IInputSystem : public IAppSystem
{
public:
	virtual void AttachToWindow( windowId_t windowId ) = 0;
	virtual void DetachFromWindow()					   = 0;

	virtual void ClearInputState() = 0;

	// Functions set/get console command which binded on a button
	virtual void		SetBinding( buttonCode_t button, const char* pCommand ) = 0;
	virtual const char* GetBindingCommand( buttonCode_t button ) const			= 0;
	virtual void		UnbindAll()												= 0;

	virtual bool IsKeyDown( buttonCode_t key ) const		  = 0;
	virtual bool IsKeyUp( buttonCode_t key ) const			  = 0;
	virtual bool IsMouseKeyDown( buttonCode_t key ) const	  = 0;
	virtual bool IsMouseKeyUp( buttonCode_t key ) const		  = 0;
	virtual bool IsMouseWheel( buttonCode_t wheel ) const	  = 0;
	virtual bool IsMouseMoved( buttonCode_t mouseAxis ) const = 0;

	virtual vector2_t GetMouseLocation() const						  = 0;
	virtual vector2_t GetMouseOffset() const						  = 0;
	virtual float  GetMouseOffset( buttonCode_t mouseAxis ) const = 0;
	virtual float  GetMouseSensitivity() const					  = 0;

	virtual buttonEvent_t GetButtonEvent( buttonCode_t buttonCode ) const	   = 0;
	virtual buttonCode_t  GetButtonCodeByName( const char* pButtonName ) const = 0;
	virtual const char*	  GetButtonName( buttonCode_t buttonCode ) const	   = 0;
};
