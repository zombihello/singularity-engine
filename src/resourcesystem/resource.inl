#pragma once

/*
==================
CResource::AddFlags
==================
*/
FORCEINLINE void CResource::AddFlags( uint8 flags )
{
	CResource::flags.fetch_or( flags, eastl::memory_order_release );
}

/*
==================
CResource::RemoveFlags
==================
*/
FORCEINLINE void CResource::RemoveFlags( uint8 flags )
{
	CResource::flags.fetch_and( ~flags, eastl::memory_order_release );
}

/*
==================
CResource::ChangeData
==================
*/
FORCEINLINE void CResource::ChangeData( const char* pPath, void* pData )
{
	// Uncache an old data
	if ( CResource::pData )
	{
		Uncache( true );
	}

	// Set a new data
	if ( pData )
	{
		AddFlags( RESOURCE_FLAG_CACHED );
	}
	path			 = pPath;
	CResource::pData = pData;
}
