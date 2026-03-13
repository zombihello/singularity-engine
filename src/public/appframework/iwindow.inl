#pragma once

/*
==================
IWindow::GetSize
==================
*/
FORCEINLINE vector2i_t IWindow::GetSize() const
{
	vector2i_t result;
	GetSize( result );
	return result;
}

/*
==================
IWindow::GetSizeInPixels
==================
*/
FORCEINLINE vector2i_t IWindow::GetSizeInPixels() const
{
	vector2i_t result;
	GetSizeInPixels( result );
	return result;
}
