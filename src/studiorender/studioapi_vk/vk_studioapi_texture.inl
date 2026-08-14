#pragma once

/*
==================
CStudioAPIImageSubresourceRangeVk::CreateForAllSubresources
==================
*/
FORCEINLINE CStudioAPIImageSubresourceRangeVk CStudioAPIImageSubresourceRangeVk::CreateForAllSubresources()
{
	return CStudioAPIImageSubresourceRangeVk( 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS );
}

/*
==================
CStudioAPIImageSubresourceRangeVk::CreateForSubresources
==================
*/
FORCEINLINE CStudioAPIImageSubresourceRangeVk CStudioAPIImageSubresourceRangeVk::CreateForSubresources( uint32 startMip, uint32 numMips /*= 1*/, uint32 startLayer /*= 0*/, uint32 numLayers /*= 1*/ )
{
	return CStudioAPIImageSubresourceRangeVk( startMip, numMips, startLayer, numLayers );
}

/*
==================
CStudioAPIImageSubresourceRangeVk::IsAllSubresources
==================
*/
FORCEINLINE bool CStudioAPIImageSubresourceRangeVk::IsAllSubresources() const
{
	return numMips == VK_REMAINING_MIP_LEVELS && numLayers == VK_REMAINING_ARRAY_LAYERS;
}

/*
==================
CStudioAPIImageSubresourceRangeVk::GetStartMip
==================
*/
FORCEINLINE uint32 CStudioAPIImageSubresourceRangeVk::GetStartMip() const
{
	return startMip;
}

/*
==================
CStudioAPIImageSubresourceRangeVk::GetStartLayer
==================
*/
FORCEINLINE uint32 CStudioAPIImageSubresourceRangeVk::GetStartLayer() const
{
	return startLayer;
}

/*
==================
CStudioAPIImageSubresourceRangeVk::GetNumMips
==================
*/
FORCEINLINE uint32 CStudioAPIImageSubresourceRangeVk::GetNumMips() const
{
	Assert( !IsAllSubresources() );
	return numMips;
}

/*
==================
CStudioAPIImageSubresourceRangeVk::GetNumLayers
==================
*/
FORCEINLINE uint32 CStudioAPIImageSubresourceRangeVk::GetNumLayers() const
{
	Assert( !IsAllSubresources() );
	return numLayers;
}

/*
==================
CStudioAPIImageSubresourceRangeVk::AsVkStructure
==================
*/
FORCEINLINE VkImageSubresourceRange CStudioAPIImageSubresourceRangeVk::AsVkStructure( VkImageAspectFlags vkImageAspectFlags ) const
{
	VkImageSubresourceRange vkImageSubresourceRange = {};
	vkImageSubresourceRange.aspectMask				= vkImageAspectFlags;
	if ( IsAllSubresources() )
	{
		vkImageSubresourceRange.baseMipLevel   = 0;
		vkImageSubresourceRange.levelCount	   = VK_REMAINING_MIP_LEVELS;
		vkImageSubresourceRange.baseArrayLayer = 0;
		vkImageSubresourceRange.layerCount	   = VK_REMAINING_ARRAY_LAYERS;
	}
	else
	{
		vkImageSubresourceRange.baseMipLevel   = startMip;
		vkImageSubresourceRange.levelCount	   = numMips;
		vkImageSubresourceRange.baseArrayLayer = startLayer;
		vkImageSubresourceRange.layerCount	   = numLayers;
	}
	return vkImageSubresourceRange;
}

/*
==================
CStudioAPISamplerVk::GetVkSampler
==================
*/
FORCEINLINE VkSampler CStudioAPISamplerVk::GetVkSampler() const
{
	return vkSampler;
}

/*
==================
CStudioAPITextureVk::GetVkImageAspectFlags
==================
*/
FORCEINLINE VkImageAspectFlags CStudioAPITextureVk::GetVkImageAspectFlags() const
{
	return vkImageAspectFlags;
}

/*
==================
CStudioAPITextureVk::GetVkImage
==================
*/
FORCEINLINE VkImage CStudioAPITextureVk::GetVkImage() const
{
	return vkImage;
}

/*
==================
CStudioAPITextureVk::GetVkImageView
==================
*/
FORCEINLINE VkImageView CStudioAPITextureVk::GetVkImageView() const
{
	Assert( !VK_IsDepthPixelFormat( pixelFormat ) );
	return vkImageViews[0];
}

/*
==================
CStudioAPITextureVk::GetVkImageView
==================
*/
FORCEINLINE VkImageView CStudioAPITextureVk::GetVkImageView( uint32 mip, uint32 layer ) const
{
	Assert( usageFlags & STUDIOAPI_TEXTURE_USAGE_FLAG_RENDER_TARGET );
	Assert( mip < numMips && layer < numLayers );
	return vkImageViews[GetVkImageViewIndex( mip, layer )];
}

/*
==================
CStudioAPITextureVk::GetVkImageView
==================
*/
FORCEINLINE VkImageView CStudioAPITextureVk::GetVkImageView( bool bDepthOnly, bool bStencilOnly ) const
{
	Assert( VK_IsDepthPixelFormat( pixelFormat ) );
	uint32 index = bDepthOnly	  ? IMAGE_VIEW_INDEX_DEPTH_ONLY
				   : bStencilOnly ? IMAGE_VIEW_INDEX_STENCIL_ONLY
								  : IMAGE_VIEW_INDEX_DEPTH_AND_STENCIL;

	// Formats without a stencil aspect only have the depth-only view
	VkImageView vkImageView = vkImageViews[index];
	return vkImageView != VK_NULL_HANDLE ? vkImageView : vkImageViews[IMAGE_VIEW_INDEX_DEPTH_ONLY];
}

/*
==================
CStudioAPITextureVk::GetSyncState
==================
*/
FORCEINLINE const studioAPISyncStateImageVk_t& CStudioAPITextureVk::GetSyncState( uint32 mip /*= 0*/, uint32 layer /*= 0*/ ) const
{
	return syncStates[GetSyncStateIndex( mip, layer )];
}

/*
==================
CStudioAPITextureVk::GetVkImageViewIndex
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetVkImageViewIndex( uint32 mip, uint32 layer ) const
{
	// A Vulkan image view at 0 index is main view
	return 1 + mip + numMips * layer;
}

/*
==================
CStudioAPITextureVk::GetLayerSize
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetLayerSize() const
{
	return GetMipOffset( numMips );
}

/*
==================
CStudioAPITextureVk::GetLayerOffset
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetLayerOffset( uint32 layer ) const
{
	return layer * GetLayerSize();
}

/*
==================
CStudioAPITextureVk::GetMipSize
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetMipSize( uint32 mip ) const
{
	pixelFormatInfo_t* pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
	uint32			   blockSizeX		= pPixelFormatInfo->blockSizeX;
	uint32			   blockSizeY		= pPixelFormatInfo->blockSizeY;
	uint32			   blockSizeZ		= pPixelFormatInfo->blockSizeZ;
	uint32			   blockBytes		= pPixelFormatInfo->blockBytes;
	uint32			   mipSizeX			= S_Max( sizeX >> mip, blockSizeX );
	uint32			   mipSizeY			= S_Max( sizeY >> mip, blockSizeY );
	uint32			   mipSizeZ			= S_Max( sizeZ >> mip, blockSizeZ );
	uint32			   numBlocksX		= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
	uint32			   numBlocksY		= ( mipSizeY + blockSizeY - 1 ) / blockSizeY;
	uint32			   numBlocksZ		= ( mipSizeZ + blockSizeZ - 1 ) / blockSizeZ;
	return numBlocksX * numBlocksY * numBlocksZ * blockBytes;
}

/*
==================
CStudioAPITextureVk::GetMipRowPitch
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetMipRowPitch( uint32 mip ) const
{
	pixelFormatInfo_t* pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
	uint32			   blockSizeX		= pPixelFormatInfo->blockSizeX;
	uint32			   blockBytes		= pPixelFormatInfo->blockBytes;
	uint32			   mipSizeX			= S_Max( sizeX >> mip, blockSizeX );
	uint32			   numBlocksX		= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
	return numBlocksX * blockBytes;
}

/*
==================
CStudioAPITextureVk::GetMipDepthPitch
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetMipDepthPitch( uint32 mip ) const
{
	pixelFormatInfo_t* pPixelFormatInfo = &g_PixelFormatInfos[pixelFormat];
	uint32			   blockSizeX		= pPixelFormatInfo->blockSizeX;
	uint32			   blockSizeY		= pPixelFormatInfo->blockSizeY;
	uint32			   blockBytes		= pPixelFormatInfo->blockBytes;
	uint32			   mipSizeX			= S_Max( sizeX >> mip, blockSizeX );
	uint32			   mipSizeY			= S_Max( sizeY >> mip, blockSizeY );
	uint32			   numBlocksX		= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
	uint32			   numBlocksY		= ( mipSizeY + blockSizeY - 1 ) / blockSizeY;
	return numBlocksX * numBlocksY * blockBytes;
}

/*
==================
CStudioAPITextureVk::GetMipOffset
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetMipOffset( uint32 mip ) const
{
	uint32 offset = 0;
	for ( uint32 mipIdx = 0; mipIdx < mip; ++mipIdx )
	{
		offset += GetMipSize( mipIdx );
	}
	return offset;
}

/*
==================
CStudioAPITextureVk::GetSyncStateIndex
==================
*/
FORCEINLINE uint32 CStudioAPITextureVk::GetSyncStateIndex( uint32 mip, uint32 layer ) const
{
	Assert( mip + numMips * layer < (uint32)syncStates.size() );
	return mip + numMips * layer;
}
