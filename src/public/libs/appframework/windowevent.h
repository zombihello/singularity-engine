/**
 * @file
 * @addtogroup appframework appframework
 */

#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H

#include "core/types.h"

/**
 * @ingroup appframework
 * @brief Struct for storage event of window
 */
struct windowEvent_t
{
	/**
	 * @brief Enumeration of types event window
	 */
	enum type_t
	{
		EVENT_NONE,						/**< No event */
		EVENT_WINDOW_CLOSE,				/**< Close window */
		EVENT_WINDOW_RESIZE,			/**< Resize window */
		EVENT_WINDOW_FOCUS_GAINED,		/**< Window is gained focus */
		EVENT_WINDOW_FOCUS_LOST,		/**< Window is lost focus */
		EVENT_WINDOW_MOVE,				/**< Window moved */
		EVENT_WINDOW_MINIMIZED,			/**< Window has been minimized */
		EVENT_WINDOW_MAXIMIZED,			/**< Window has been maximized */
		EVENT_WINDOW_RESTORED,			/**< Window has been restored to normal size and position */
		EVENT_WINDOW_DISPLAY_CHANGED	/**< Window has been moved to another display */
	};

	/**
	 * @brief Window display changed event
	 */
	struct windowDisplayChangedEvent_t
	{	
		uint32		displayId;	/**< Display ID */
	};

	/**
	 * @brief Window resize event
	 */
	struct windowResizeEvent_t
	{
		int32		width;		/**< New width window */
		int32		height;		/**< New height window */
	};

	/**
	 * @brief Window move event
	 */
	struct windowMoveEvent_t
	{
		int32		x;			/**< Current position by X */
		int32		y;			/**< Current position by Y */
	};

	/**
	 * @brief Events union
	 */
	union uevents_t
	{
		windowResizeEvent_t			windowResize;			/**< Window resize event */
		windowMoveEvent_t			windowMove;				/**< Window move event */
		windowDisplayChangedEvent_t	windowDisplayChanged;	/**< Window has been moved to another display */
	};

	/**
	 * @brief Constructor
	 */
	windowEvent_t() 
		: type( EVENT_NONE )
		, windowId( 0 )
	{}

	type_t			type;		/**< Type event */
	uint32			windowId;	/**< Id window */
	uevents_t		events;		/**< Events */
};

#endif // !WINDOWEVENT_H