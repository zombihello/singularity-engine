#ifndef INPUTSYSTEM_PRIVATE_H
#define INPUTSYSTEM_PRIVATE_H

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------
// While the mouse is in relative mode, the cursor is hidden, and the
// driver will try to report continuous motion in the current window.
// Only relative motion events will be delivered, the mouse position
// will not change.
void Sys_SetRelativeMouseMode( bool bEnabled );

#endif // !INPUTSYSTEM_PRIVATE_H