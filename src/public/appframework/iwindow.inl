#pragma once

/*
==================
IWindow::GetSize
==================
*/
FORCEINLINE ivec2_t IWindow::GetSize() const
{
	ivec2_t result;
	GetSize( result );
	return result;
}

/*
==================
IWindow::GetSizeInPixels
==================
*/
FORCEINLINE ivec2_t IWindow::GetSizeInPixels() const
{
	ivec2_t result;
	GetSizeInPixels( result );
	return result;
}
