#pragma once
#include "studiorender/studioapi/istudioapi_swapchain.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioViewport;

//-----------------------------------------------------------------------------
// Studio viewport client interface
//-----------------------------------------------------------------------------
class IStudioViewportClient
{
public:
	virtual ~IStudioViewportClient() {}
	virtual void Update( float deltaSeconds )				   = 0;
	virtual void DrawFrame( IStudioViewport* pStudioViewport ) = 0;
};

//-----------------------------------------------------------------------------
// Base implementation of a studio viewport client
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CBaseStudioViewportClient : public TBaseClass
{
public:
	virtual void Update( float deltaSeconds ) override {}
	virtual void DrawFrame( IStudioViewport* pStudioViewport ) override {}
};
