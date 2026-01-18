#pragma once
#include "studiorender/studioapi/istudioapi_resource.h"

//-----------------------------------------------------------------------------
// StudioAPI queue type
//-----------------------------------------------------------------------------
enum studioAPIQueueType_t
{
	STUDIOAPI_QUEUE_TYPE_GRAPHICS,
	STUDIOAPI_QUEUE_TYPE_COMPUTE,
	STUDIOAPI_QUEUE_TYPE_TRANSFER,
	STUDIOAPI_NUM_QUEUE_TYPES
};

//-----------------------------------------------------------------------------
// StudioAPI command context interface
//-----------------------------------------------------------------------------
class IStudioAPICmdContext : public IStudioAPIResource
{
public:
	virtual ~IStudioAPICmdContext() {}
	virtual bool HasSupportQueueType( studioAPIQueueType_t queueType ) const = 0;
};