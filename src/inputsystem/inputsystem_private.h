/**
 * @file
 * @addtogroup inputsystem inputsystem
 */

#ifndef INPUTSYSTEM_PRIVATE_H
#define INPUTSYSTEM_PRIVATE_H

/**
 * @brief Set relative mouse mode
 * @note Need implement on each platform
 * 
 * While the mouse is in relative mode, the cursor is hidden, and the
 * driver will try to report continuous motion in the current window.
 * Only relative motion events will be delivered, the mouse position
 * will not change.
 * 
 * @param bEnabled	 Whether or not to enable relative mode
 */
void Sys_SetRelativeMouseMode( bool bEnabled );

#endif // !INPUTSYSTEM_PRIVATE_H