#pragma once
#include "tier0/types.h"
#include "tier1/math/types.h"

//-----------------------------------------------------------------------------
// Constants, types and forward declarations
//-----------------------------------------------------------------------------
struct display_t;
typedef uint32 windowId_t;

enum windowMode_t
{
	WINDOW_MODE_HIDDEN,
	WINDOW_MODE_WINDOWED,
	WINDOW_MODE_BORDERLESS_FULLSCREEN,
	WINDOW_MODE_EXCLUSIVE_FULLSCREEN
};

struct windowCreateInfo_t
{
	const char*		 pTitle;
	uint32			 width;
	uint32			 height;
	float			 refreshRate;
	windowMode_t	 mode;
	const display_t* pDisplay;	// Optional
};

//-----------------------------------------------------------------------------
// A window interface
//-----------------------------------------------------------------------------
class IWindow
{
public:
	virtual bool Create( const windowCreateInfo_t& createInfo ) = 0;
	virtual void Close()										= 0;

	virtual void Show( bool bShow = true ) = 0;
	virtual void Maximize()				   = 0;
	virtual void Minimize()				   = 0;

	virtual void SetTitle( const char* pTitle )			= 0;
	virtual void SetSize( uint32 width, uint32 height ) = 0;
	virtual void SetRefreshRate( float refreshRate )	= 0;
	virtual void SetDisplay( const display_t& display ) = 0;
	virtual void SetMode( windowMode_t mode )			= 0;

	virtual bool		   IsOpen() const						  = 0;
	virtual windowMode_t   GetMode() const						  = 0;
	virtual float		   GetRefreshRate() const				  = 0;
	virtual float		   GetPixelDensity() const				  = 0;
	virtual float		   GetDisplayScale() const				  = 0;
	virtual void		   GetSize( ivec2_t& size ) const		  = 0;
	virtual void		   GetSizeInPixels( ivec2_t& size ) const = 0;
	ivec2_t				   GetSize() const;
	ivec2_t				   GetSizeInPixels() const;
	virtual windowHandle_t GetHandle() const = 0;
	virtual windowId_t	   GetId() const	 = 0;
};

#include "appframework/iwindow.inl"
