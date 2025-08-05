#pragma once

/*
==================
CStudioViewport::R_GetActiveViewport
==================
*/
FORCEINLINE CStudioViewport* CStudioViewport::R_GetActiveViewport()
{
	Assert( Studio_IsInRenderThread() );
	return s_pActiveViewport;
}

/*
==================
CStudioViewport::GetIndex
==================
*/
FORCEINLINE uint32 CStudioViewport::GetIndex() const
{
	return index;
}

/*
==================
CStudioViewport::OnReleaseViewportIndex
==================
*/
FORCEINLINE CStudioViewport::COnReleaseViewportIndex& CStudioViewport::OnReleaseViewportIndex() const
{
	return onReleaseViewportIndex;
}

/*
==================
CStudioViewport::OnRenderPassUpdated
==================
*/
FORCEINLINE CStudioViewport::COnRenderPassUpdated& CStudioViewport::OnRenderPassUpdated() const
{
	return onRenderPassUpdated;
}

/*
==================
CStudioViewport::AcquireIndex
==================
*/
FORCEINLINE uint32 CStudioViewport::AcquireIndex()
{
	CScopeLock		scopeLock( s_ViewportIndexMutex );
	if ( s_FreeViewportIndices.empty() )
	{
		++s_LastViewportIndex;
		return s_LastViewportIndex;
	}

	uint32		freeViewportIndex = s_FreeViewportIndices.back();
	s_FreeViewportIndices.pop_back();
	return freeViewportIndex;
}

/*
==================
CStudioViewport::ReleaseIndex
==================
*/
FORCEINLINE void CStudioViewport::ReleaseIndex( uint32 viewportIndex )
{
	CScopeLock		scopeLock( s_ViewportIndexMutex );
	s_FreeViewportIndices.emplace_back( viewportIndex );
	onReleaseViewportIndex.Broadcast( this );
}
