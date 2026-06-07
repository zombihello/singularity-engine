#pragma once
#include "inputsystem/iinputsystem.h"

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
	virtual void WriteBindings( IStreamDataWriter* pStreamData ) const override;
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

	virtual vector2_t GetMouseLocation() const override;
	virtual vector2_t GetMouseOffset() const override;
	virtual float	  GetMouseOffset( buttonCode_t mouseAxis ) const override;
	virtual float	  GetMouseSensitivity() const override;

	virtual buttonEvent_t GetButtonEvent( buttonCode_t buttonCode ) const override;
	virtual buttonCode_t  GetButtonCodeByName( const char* pButtonName ) const override;
	virtual const char*	  GetButtonName( buttonCode_t buttonCode ) const override;

	void ExecBindingCommand( buttonCode_t button );

private:
	static void OnInputEvent( void* pUserData, const inputEvent_t& inputEvent );

	windowId_t							windowId;  // A window that was attached the input system
	IWindowMgr::IOnInputEvent::handle_t onInputEventHandle;
	buttonEvent_t						buttonEvents[BUTTON_CODE_COUNT];
	vector2_t							mouseLocation;
	vector2_t							mouseOffset;
	eastl::string						binds[BUTTON_CODE_COUNT];
};

extern CInputSystem g_inputSystem;
