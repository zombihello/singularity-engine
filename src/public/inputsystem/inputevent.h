/**
 * @file
 * @addtogroup inputsystem inputsystem
 */

#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include "core/types.h"
#include "inputsystem/buttoncode.h"

/**
 * @ingroup inputsystem
 * @brief Struct for storage input event
 */
struct inputEvent_t
{
	/**
	 * @brief Enumeration of types event window
	 */
	enum type_t
	{
		EVENT_NONE,				/**< No event */
		EVENT_KEY_PRESSED,		/**< Key pressed */
		EVENT_KEY_RELEASED,		/**< Key released */
		EVENT_MOUSE_PRESSED,	/**< Mouse pressed */
		EVENT_MOUSE_RELEASED,	/**< Mouse released */
		EVENT_MOUSE_MOVE,		/**< Mouse move */
		EVENT_MOUSE_WHEEL,		/**< Mouse wheel moved */
		EVENT_TEXT_INPUT		/**< Input text in window */
	};

	/**
	 * @brief Key pressed/released event
	 */
	struct keyEvent_t
	{
		buttonCode_t	code;			/**< Code of key */
		bool		bAlt;			/**< Is event with alt */
		bool		bControl;		/**< Is event with ctrl */
		bool		bShift;			/**< Is event with shift */
		bool		bSuper;			/**< Is event with super (key Windows on keyboard) */
		bool		bCapsLock;		/**< Is event with caps lock */
		bool		bNumLock;		/**< Is event with num lock */
	};

	/**
	 * @brief Mouse button pressed/released event
	 */
	struct mouseButtonEvent_t
	{
		buttonCode_t	code;		/**< Code of mouse button */
		int32		x;			/**< Mouse position by X */
		int32		y;			/**< Mouse position by Y */
	};

	/**
	 * @brief Mouse moving event
	 */
	struct mouseMoveEvent_t
	{
		int32		x;					/**< Current position by X */
		int32		y;					/**< Current position by Y */
		int32		xDirection;			/**< Offset by X */
		int32		yDirection;			/**< Offset by Y */
	};

	/**
	 * @brief Mouse wheel moving event
	 */
	struct mouseWheelEvent_t
	{
		int32		x;					/**< Moving wheel by X */
		int32		y;					/**< Moving wheel by Y */
	};

	/**
	 * @brief Text input event
	 */
	struct textInputEvent_t
	{
		achar*		pText;				/**< Entered text */
	};

	/**
	 * @brief Events union
	 */
	union uevents_t
	{
		keyEvent_t			key;				/**< Key pressed/released event */
		mouseButtonEvent_t	mouseButton;		/**< Mouse button pressed/released event */
		mouseMoveEvent_t		mouseMove;			/**< Mouse moving event */
		mouseWheelEvent_t		mouseWheel;			/**< Mouse wheel moving event */
		textInputEvent_t		textInputEvent;		/**< Input text event */
	};

	/**
	 * @brief Constructor
	 */
	inputEvent_t()
		: type( EVENT_NONE )
	{}

	type_t			type;	/**< Type event */
	uevents_t		events;	/**< Events */
};

#endif // !INPUTEVENT_H