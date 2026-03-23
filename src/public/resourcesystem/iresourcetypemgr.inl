#pragma once

/*
==================
IResourceTypeMgr::LoadResource
==================
*/
FORCEINLINE CRefPtr<IResource> IResourceTypeMgr::LoadResource( const char* pPath, uint8 flags /* = RESOURCE_FLAG_NONE */ )
{
	return LoadResource( pPath, pPath, flags );
}
