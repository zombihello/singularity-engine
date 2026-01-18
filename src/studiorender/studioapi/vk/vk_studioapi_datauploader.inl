#pragma once

/*
==================
CStudioAPIDataUploaderVk::Upload
==================
*/
template<typename TUploadLambda>
FORCEINLINE void CStudioAPIDataUploaderVk::Upload( uint64 dataSize, uint32 stagingBufferOffsetAlignment, TUploadLambda uploadLambda )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_RENDERING );
	CStudioAPICmdContextVk* pTransferCmdContext = (CStudioAPICmdContextVk*)g_StudioAPIVk.GetImmediateCmdContext( STUDIOAPI_QUEUE_TYPE_TRANSFER );
	uint64					remainDataSize		= dataSize;
	while ( remainDataSize > 0 )
	{
		// Will wait only if staging buffer is in flight
		stagingBuffer_t& stagingBuffer = stagingBuffers[currentStagingBufferIndex];
		WaitUpload( currentStagingBufferIndex );

		// Initialize upload parameters
		uploadParams_t uploadParams		 = {};
		uint32		   offset			 = Align( stagingBuffer.currentOffset, stagingBufferOffsetAlignment );
		uploadParams.partialUploadSize	 = Min( STAGING_BUFFER_SIZE - (int32)offset, (int32)remainDataSize );
		uploadParams.remainSizeToUpload	 = remainDataSize;
		uploadParams.vkStagingBuffer	 = stagingBuffer.vkBuffer;
		uploadParams.stagingBufferOffset = offset;
		uploadParams.pStagingBufferData	 = stagingBuffer.pBufferData + offset;

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
		int32 uploadedSize = uploadLambda( uploadParams );
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
		remainDataSize -= uploadedSize;
		stagingBuffer.currentOffset = offset + uploadedSize;
	}

	// If the staging buffer is full submit it
	if ( stagingBuffers[currentStagingBufferIndex].currentOffset == STAGING_BUFFER_SIZE )
	{
		SubmitCmdBuffer();
	}
}

/*
==================
CStudioAPIDataUploaderVk::Flush
==================
*/
FORCEINLINE void CStudioAPIDataUploaderVk::Flush()
{
	SubmitCmdBuffer( true );
}
