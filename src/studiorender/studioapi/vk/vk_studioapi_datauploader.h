/**
 * @file
 * @addtogroup studioapi_vk studioapi_vk
 */

#ifndef VK_STUDIOAPI_DATAUPLOADER_H
#define VK_STUDIOAPI_DATAUPLOADER_H

#include "studiorender/studioapi/vk/vk_studioapi_delegates.h"
#include "studiorender/studioapi/vk/vk_studioapi_device.h"
#include "studiorender/studioapi/vk/vk_studioapi_cmdlist.h"

// Forward declarations
class CStudioAPICmdBufferVk;

/**
 * @ingroup studioapi_vk
 * @brief Vulkan StudioAPI data uploader
 * For uploading a data this the class is used, main feature of which is fixed memory footprint
 */
class CStudioAPIDataUploaderVk
{
public:
	/**
	 * @brief Upload parameters
	 */
	struct uploadParams_t
	{
		int32					partialUploadSize;		/**< How much memory in bytes you allowed to upload, must be less or equal to whole data size. If by some constraints that amount of memory is not enough for any upload, return 0 from lambda. Lambda will be called one more time with more memory, but be aware of partialUploadSize must be less or equal to STAGING_BUFFER_SIZE */
		uint64					remainSizeToUpload;		/**< How much memory of whole data size is remain to upload */
		VkBuffer				vkStagingBuffer;		/**< Pass this buffer to vkCmdCopyBuffer/vkCmdCopyBufferToImage as source buffer */
		uint32					stagingBufferOffset;	/**< Pass this value to VkBufferCopy::srcOffset or VkBufferImageCopy::bufferOffset */
		byte*					pStagingBufferData;		/**< Copy data here, no need to offset it by stagingBufferOffset */
		CStudioAPICmdBufferVk*	pCmdBuffer;				/**< Insert your copy commands, barriers here */
	};

	/**
	 * @brief Constructor
	 */
	CStudioAPIDataUploaderVk();

	/**
	 * @brief Destructor
	 */
	~CStudioAPIDataUploaderVk();

	/**
	 * @brief Initialize the buffer uploader
	 */
	void Init();

	/**
	 * @brief Shutdown the buffer uploader
	 */
	void Shutdown();

	/**
	 * @brief Initiates uploading of data
	 * @param dataSize						Size in bytes we are going to upload
	 * @param stagingBufferOffsetAlignment	Alignment requirements for VkBufferCopy::srcOffset or VkBufferImageCopy::bufferOffset
	 * @param uploadLambda					Lambda will be called 1 or more times, because initialDataSize can be much bigger than STAGING_BUFFER_SIZE or
	 *										there are datas of other resources in uploader that are waiting in a queue to be uploaded, thus data can be uploaded in parts.
     *										In that lambda you need to copy memory into staging buffer and insert commands in command buffer.
     *										Lambda must have following arguments list: (const uploadParams_t& uploadParams)
     *										Lambda must return 0, if partialUploadSize is too small for any upload, or amount of bytes uploaded.
     *										Amount of bytes uploaded must be less or equal uploadParams.partialUploadSize
	 */
	template<typename TUploadLambda>
	void Upload( uint64 dataSize, uint32 stagingBufferOffsetAlignment, TUploadLambda uploadLambda )
	{
		PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
		CStudioAPICmdContextVk*		pTransferCmdContext = ( CStudioAPICmdContextVk* )g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER );
		uint64						remainDataSize		= dataSize;
		while ( remainDataSize > 0 )
		{
			// Will wait only if staging buffer is in flight
			stagingBuffer_t&	stagingBuffer = stagingBuffers[currentStagingBufferIndex];
			WaitUpload( currentStagingBufferIndex );

			// Initialize upload parameters
			uploadParams_t		uploadParams	= {};
			uint32				offset			= Align( stagingBuffer.currentOffset, stagingBufferOffsetAlignment );
			uploadParams.partialUploadSize		= Min( STAGING_BUFFER_SIZE - ( int32 )offset, ( int32 )remainDataSize );
			uploadParams.remainSizeToUpload		= remainDataSize;
			uploadParams.vkStagingBuffer		= stagingBuffer.vkBuffer;
			uploadParams.stagingBufferOffset	= offset;
			uploadParams.pStagingBufferData		= stagingBuffer.pBufferData + offset;		

			// Submit the current staging buffer if we haven't enough memory
			if ( uploadParams.partialUploadSize <= 0 )
			{
				SubmitCmdBuffer();
				continue;
			}

			// Begin record to a command buffer if we haven't yet
			if ( stagingBuffer.pCmdList->GetState() == STUDIOAPI_VK_CMDLIST_STATE_READY_FOR_RECORD )
			{
				stagingBuffer.pCmdList->BeginRecord();
			}
			uploadParams.pCmdBuffer = stagingBuffer.pCmdList->GetCmdBuffer();

			// Call the lambda
			int32	uploadedSize = uploadLambda( uploadParams );
			Assert( uploadedSize <= uploadParams.partialUploadSize );
			if ( uploadedSize <= 0 )
			{
				// If we've offered maximum possible amount of memory for partial upload and its not enough then it is a wrong usage,
				// or STAGING_BUFFER_SIZE is not big enough
				AssertMsg( uploadParams.partialUploadSize != STAGING_BUFFER_SIZE, "Upload lambda wants more memory than buffer uploader can offer" );
				SubmitCmdBuffer();
				continue;
			}

			// Update current offset and remain size
			remainDataSize				-= uploadedSize;
			stagingBuffer.currentOffset = offset + uploadedSize;
		}

		// If the staging buffer is full submit it
		if ( stagingBuffers[currentStagingBufferIndex].currentOffset == STAGING_BUFFER_SIZE )
		{
			SubmitCmdBuffer();
		}
	}

	/**
	 * @brief Flush upload data to GPU
	 * To be sure that all datas has been uploaded, for example before submitting command buffers, call this
	 */
	FORCEINLINE void Flush()
	{
		SubmitCmdBuffer( true );
	}

private:
	enum
	{
		STAGING_BUFFER_SIZE = 64 * 1024 * 1024,		/**< Staging buffer size */
		NUM_STAGING_BUFFERS = 2						/**< Staging buffers number */
	};

	/**
	 * @brief Staging buffer
	 */
	struct stagingBuffer_t
	{
		/**
		 * @brief Constructor
		 */
		stagingBuffer_t()
			: vkBuffer( VK_NULL_HANDLE )
			, vmaAllocation( VK_NULL_HANDLE )
			, currentOffset( 0 )
			, pBufferData( NULL )
		{}

		VkBuffer							vkBuffer;			/**< Vulkan buffer */
		VmaAllocation						vmaAllocation;		/**< VMA allocation */
		uint32								currentOffset;		/**< Current offset in the buffer */
		TRefPtr<CStudioAPICmdListBatchVk>	pCmdListBatch;		/**< Command list batch to submit the staging buffer */
		TRefPtr<CStudioAPICmdListVk>		pCmdList;			/**< Command list for the staging buffer */
		byte*								pBufferData;		/**< Mapped buffer memory */
	};

	/**
	 * @brief Submit current command buffer
	 * @param bWait		Is need to wait when the command buffer will be done
	 */
	void SubmitCmdBuffer( bool bWait = false );

	/**
	 * @brief Wait upload
	 * @param stagingBufferIndex	Staging buffer index to wait
	 */
	void WaitUpload( uint8 stagingBufferIndex );

	/**
	 * @brief Delegate called when StudioAPI Vulkan is shutdown
	 * @param pUserData		Pointer to CStudioAPIDataUploaderVk
	 */
	static void OnStudioAPIVkShutdown( void* pUserData );

	uint8										currentStagingBufferIndex;				/**< Current staging buffer index */
	stagingBuffer_t								stagingBuffers[NUM_STAGING_BUFFERS];	/**< Staging buffers array */
	COnStudioAPIVkShutdown::funcDelegate_t*		pStudioAPIVkShutdownDelegate;			/**< Delegate handle 'on StudioAPI Vulkan shutdown' */
};

#endif // !VK_STUDIOAPI_DATAUPLOADER_H