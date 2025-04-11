/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_CMDCONTEXT_H
#define ISTUDIOAPI_CMDCONTEXT_H

#include "studiorender/studioapi/istudioapi_resource.h"

/**
 * @ingroup studioapi
 * @brief StudioAPI queue type
 */
enum studioAPIQueueType_t
{
	STUDIOAPI_QUEUE_TYPE_GRAPHICS,		/**< A queue which support graphics operations */
	STUDIOAPI_QUEUE_TYPE_COMPUTE,		/**< A queue which support compute operations */
	STUDIOAPI_QUEUE_TYPE_TRANSFER,		/**< A queue which support transfer operations */
	STUDIOAPI_NUM_QUEUE_TYPES			/**< Queue types number */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI command context interface
 */
class IStudioAPICmdContext : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPICmdContext() {}

	/**
	 * @brief Has support of a queue type
	 * @param queueType		Queue type to check
	 * @return Return TRUE if the queue type is supported by the command context, otherwise FALSE
	 */
	virtual bool HasSupportQueueType( studioAPIQueueType_t queueType ) const = 0;
};

#endif // ISTUDIOAPI_CMDCONTEXT_H