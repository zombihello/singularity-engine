#pragma once
#include "appframework/windowmgr_types.h"

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

	virtual void SetTitle( const char* pTitle )				 = 0;
	virtual void SetSize( uint32 width, uint32 height )		 = 0;
	virtual void SetRefreshRate( float refreshRate )		 = 0;
	virtual void SetDisplay( displayHandle_t displayHandle ) = 0;
	virtual void SetMode( windowMode_t mode )				 = 0;
	virtual void SetCursorVisible( bool bVisible )			 = 0;

	virtual bool		   IsOpen() const						  = 0;
	virtual bool		   IsCursorVisible() const				  = 0;
	virtual windowMode_t   GetMode() const						  = 0;
	virtual float		   GetRefreshRate() const				  = 0;
	virtual float		   GetPixelDensity() const				  = 0;
	virtual float		   GetDisplayScale() const				  = 0;
	virtual void		   GetSize( vector2i_t& size ) const		  = 0;
	virtual void		   GetSizeInPixels( vector2i_t& size ) const = 0;
	vector2i_t				   GetSize() const;
	vector2i_t				   GetSizeInPixels() const;
	virtual windowHandle_t GetHandle() const = 0;
	virtual windowId_t	   GetId() const	 = 0;
};

#include "appframework/iwindow.inl"
