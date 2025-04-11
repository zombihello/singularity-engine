/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef AXIS_H
#define AXIS_H

/**
 * @ingroup stdlib
 * @brief Axis type
 */
enum axis_t
{
	AXIS_X			= 1 << 0,		/**< X axis */
	AXIS_Y			= 1 << 1,		/**< Y axis */
	AXIS_Z			= 1 << 2,		/**< Z axis */

	AXIS_RIGHT		= AXIS_X,		/**< Right axis */
	AXIS_UP			= AXIS_Y,		/**< Up axis */
	AXIS_FORWARD	= AXIS_Z,		/**< Forward axis */
};

/**
 * @ingroup stdlib
 * @brief Axis up type
 */
enum axisUp_t
{
	AXIS_UP_PLUS_X,						/**< +X */
	AXIS_UP_MINUS_X,					/**< -X */
	AXIS_UP_PLUS_Y,						/**< +Y */
	AXIS_UP_MINUS_Y,					/**< -Y */
	AXIS_UP_PLUS_Z,						/**< +Z */
	AXIS_UP_MINUS_Z,					/**< -Z */
	AXIS_UP_NUM,						/**< Axis ups number */
	AXIS_UP_DEFAULT	= AXIS_UP_PLUS_Y	/**< Default axis up */
};

#endif // !AXIS_H