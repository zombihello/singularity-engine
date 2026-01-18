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
	virtual void Update( float deltaSeconds ) = 0;

	virtual void DrawFrame( IStudioViewport* pStudioViewport )		  = 0;
	virtual void R_BeginDrawFrame( IStudioViewport* pStudioViewport ) = 0;
	virtual void R_EndDrawFrame( IStudioViewport* pStudioViewport )	  = 0;
};

//-----------------------------------------------------------------------------
// Base implementation of a studio viewport client
//-----------------------------------------------------------------------------
template<class TBaseClass>
class TBaseStudioViewportClient : public TBaseClass
{
public:
	virtual void Update( float deltaSeconds ) override {}
	virtual void DrawFrame( class IStudioViewport* pStudioViewport ) override {}
	virtual void R_BeginDrawFrame( IStudioViewport* pStudioViewport ) override {}
	virtual void R_EndDrawFrame( IStudioViewport* pStudioViewport ) override {}
};