#pragma once

/*
==================
CResource::AddFlags
==================
*/
FORCEINLINE void CResource::AddFlags( uint8 flags )
{
	CResource::flags |= flags;
}

/*
==================
CResource::RemoveFlags
==================
*/
FORCEINLINE void CResource::RemoveFlags( uint8 flags )
{
	CResource::flags &= ~flags;
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
		CGuardValue<uint8> guardValue( flags, flags & ~RESOURCE_FLAG_PERMANENT );
		Uncache();
	}

	// Set a new data
	if ( pData )
	{
		AddFlags( RESOURCE_FLAG_CACHED );
	}
	path			 = pPath;
	CResource::pData = pData;
}
