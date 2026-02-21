#pragma once

/*
==================
IFileSystem::GetCurrentDirectory
==================
*/
FORCEINLINE void IFileSystem::GetCurrentDirectory( eastl::string& dest, bool bShrinkToFit /* = true */ ) const
{
	// Pre-allocate memory for current directory
	dest.resize( 1024 );

	// Try get the current directory
	while ( !GetCurrentDirectory( dest.data(), (uint32)dest.size() ) )
	{
		dest.resize( dest.size() * 2 );
	}

	// Remove all extra null terminator characters
	dest = dest.c_str();

	// Shrink to fit if it need
	if ( bShrinkToFit )
	{
		dest.shrink_to_fit();
	}
}

/*
==================
IFileSystem::GetCurrentDirectory
==================
*/
FORCEINLINE eastl::string IFileSystem::GetCurrentDirectory( bool bShrinkToFit /* = true */ ) const
{
	eastl::string result;
	GetCurrentDirectory( result, bShrinkToFit );
	return result;
}
