#pragma once
#include "studiorender/studioapi_vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi_vk/vk_studioapi_device.h"
#include "studiorender/studioapi_vk/vk_studioapi_cmdlist.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CStudioAPICmdBufferVk;

//-----------------------------------------------------------------------------
// For uploading a data this the class is used, main feature of which is fixed memory footprint
//-----------------------------------------------------------------------------
class CStudioAPIDataUploaderVk
{
public:
	struct uploadParams_t
	{
		int32 partialUploadSize;					 // How much memory in bytes you allowed to upload,
													 // must be less or equal to whole data size. If by some constraints
													 // that amount of memory is not enough for any upload, return 0 from lambda.
													 // Lambda will be called one more time with more memory, but be aware of
													 // partialUploadSize must be less or equal to STAGING_BUFFER_SIZE
		uint64				   remainSizeToUpload;	 // How much memory of whole data size is remain to upload
		VkBuffer			   vkStagingBuffer;		 // Pass this buffer to vkCmdCopyBuffer/vkCmdCopyBufferToImage as source buffer
		uint32				   stagingBufferOffset;	 // Pass this value to VkBufferCopy::srcOffset or VkBufferImageCopy::bufferOffset
		byte*				   pStagingBufferData;	 // Copy data here, no need to offset it by stagingBufferOffset
		CStudioAPICmdBufferVk* pCmdBuffer;			 // Insert your copy commands, barriers here
	};

	CStudioAPIDataUploaderVk();
	~CStudioAPIDataUploaderVk();

	void Init();
	void Shutdown();

	// Initiates uploading of data
	// dataSize							Size in bytes we are going to upload
	// stagingBufferOffsetAlignment		Alignment requirements for VkBufferCopy::srcOffset or VkBufferImageCopy::bufferOffset
	// uploadLambda						Lambda will be called 1 or more times, because initialDataSize can be much bigger than STAGING_BUFFER_SIZE or
	//									there are datas of other resources in uploader that are waiting in a queue to be uploaded, thus data can be uploaded in parts.
	//									In that lambda you need to copy memory into staging buffer and insert commands in command buffer.
	//									Lambda must have following arguments list: (const uploadParams_t& uploadParams)
	//									Lambda must return 0, if partialUploadSize is too small for any upload, or amount of bytes uploaded.
	//									Amount of bytes uploaded must be less or equal uploadParams.partialUploadSize
	template<typename TUploadLambda>
	void Upload( uint64 dataSize, uint32 stagingBufferOffsetAlignment, TUploadLambda uploadLambda );

	// Flush upload data to GPU
	// To be sure that all datas has been uploaded, for example before submitting command buffers, call this
	void Flush();

private:
	enum
	{
		STAGING_BUFFER_SIZE = 64 * 1024 * 1024,
		NUM_STAGING_BUFFERS = 2
	};

	struct stagingBuffer_t
	{
		stagingBuffer_t()
			: vkBuffer( VK_NULL_HANDLE )
			, vmaAllocation( VK_NULL_HANDLE )
			, currentOffset( 0 )
			, pBufferData( NULL )
		{
		}

		VkBuffer						  vkBuffer;
		VmaAllocation					  vmaAllocation;
		uint32							  currentOffset;
		CRefPtr<CStudioAPICmdListBatchVk> pCmdListBatch;
		CRefPtr<CStudioAPICmdListVk>	  pCmdList;
		byte*							  pBufferData;
	};

	void		SubmitCmdBuffer( bool bWait = false );
	void		WaitUpload( uint8 stagingBufferIndex );
	static void OnStudioAPIVkShutdown( void* pUserData );

	uint8							 currentStagingBufferIndex;
	stagingBuffer_t					 stagingBuffers[NUM_STAGING_BUFFERS];
	COnStudioAPIVkShutdown::handle_t onStudioAPIVkShutdownHandle;
};

#include "studiorender/studioapi_vk/vk_studioapi_datauploader.inl"
