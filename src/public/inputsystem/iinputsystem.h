#pragma once
#include "tier1/math/math.h"
#include "appframework/iappsystem.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/buttoncode.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStreamDataWriter;

//-----------------------------------------------------------------------------
// Button state flags
//-----------------------------------------------------------------------------
enum buttonStateFlag_t
{
	BUTTON_STATE_FLAG_NONE			 = 0,
	BUTTON_STATE_FLAG_DOWN			 = BIT( 0 ),
	BUTTON_STATE_FLAG_PRESSED		 = BIT( 1 ),
	BUTTON_STATE_FLAG_DOUBLE_PRESSED = BIT( 2 ),
	BUTTON_STATE_FLAG_RELEASED		 = BIT( 3 ),
	BUTTON_STATE_FLAG_REPEATED		 = BIT( 4 ),
	BUTTON_STATE_FLAG_SCROLLED		 = BIT( 5 ),
	BUTTON_STATE_FLAG_MOVED			 = BIT( 6 ),
};

//-----------------------------------------------------------------------------
// Input system interface
//-----------------------------------------------------------------------------
#define INPUTSYSTEM_INTERFACE_VERSION "SInputSystem001"
class IInputSystem : public IAppSystem
{
public:
	virtual void AttachToWindow( windowId_t windowId )				   = 0;
	virtual void DetachFromWindow()									   = 0;
	virtual void WriteBindings( IStreamDataWriter* pStreamData ) const = 0;
	virtual void FrameUpdate( float deltaTime )						   = 0;

	// Functions set/get console command which binded on a button
	virtual void		SetBinding( buttonCode_t button, const char* pCommand ) = 0;
	virtual const char* GetBindingCommand( buttonCode_t button ) const			= 0;
	virtual void		UnbindAll()												= 0;

	virtual bool WasKeyPressed( buttonCode_t key ) const		 = 0;
	virtual bool WasKeyReleased( buttonCode_t key ) const		 = 0;
	virtual bool IsKeyDown( buttonCode_t key ) const			 = 0;
	virtual bool WasMousePressed( buttonCode_t key ) const		 = 0;
	virtual bool WasMouseDoublePressed( buttonCode_t key ) const = 0;
	virtual bool WasMouseReleased( buttonCode_t key ) const		 = 0;
	virtual bool IsMouseDown( buttonCode_t key ) const			 = 0;
	virtual bool WasMouseWheel( buttonCode_t wheel ) const		 = 0;
	virtual bool WasMouseMoved( buttonCode_t mouseAxis ) const	 = 0;

	virtual vector2_t GetMouseLocation() const						 = 0;
	virtual vector2_t GetMouseOffset() const						 = 0;
	virtual float	  GetMouseOffset( buttonCode_t mouseAxis ) const = 0;
	virtual float	  GetMouseSensitivity() const					 = 0;

	virtual uint8		 GetButtonStateFlags( buttonCode_t buttonCode ) const = 0;
	virtual buttonCode_t GetButtonByName( const char* pButtonName ) const	  = 0;
	virtual const char*	 GetButtonName( buttonCode_t buttonCode ) const		  = 0;
};
