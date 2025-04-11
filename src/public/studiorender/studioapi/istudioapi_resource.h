/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_RESOURCE_H
#define ISTUDIOAPI_RESOURCE_H

#include "stdlib/refcount.h"

/**
 * @ingroup studioapi
 * @brief StudioAPI base resource interface
 */
class IStudioAPIResource : public IRefCounted
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIResource() {}
};

#endif // !ISTUDIOAPI_RESOURCE_H