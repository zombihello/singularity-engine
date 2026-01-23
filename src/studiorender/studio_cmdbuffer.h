#pragma once
#include "tier0/threading.h"
#include "studiorender/istudio_cmdbuffer.h"

//-----------------------------------------------------------------------------
// Studio render command buffer
//-----------------------------------------------------------------------------
class CStudioCmdBuffer : public IStudioCmdBuffer
{
public:
	// IStudioCmdBuffer interface
	// Allocate a chunk
	// NOTE: After filling in the allocated chunk you have to call CommitAllocation
	virtual studioCmdAlloc_t GetAllocation( uint32 allocationSize ) override;

	virtual void CommitAllocation( studioCmdAlloc_t& allocContext ) override;
	virtual void Flush() override;

	CStudioCmdBuffer( uint32 bufferSize, uint32 alignment = 1 );
	~CStudioCmdBuffer();

	// Checks if there is data to be read from the ring buffer, and if so accesses the pointer to the data to be read
	bool BeginRead( void*& pReadPointer, uint32& readSize );

	// Frees the first ReadSize bytes available for reading via BeginRead to the writing thread
	void EndRead( uint32 readSize );

	// Waits for data to be available for reading
	void WaitForRead( uint32 waitTime = -1 );

	// Checks if some data has been written to or not
	FORCEINLINE bool IsReadBufferEmpty() const
	{
		return pReadPointer == pWritePointer;
	}

private:
	byte* pData;
	byte* pDataEnd;
	byte* volatile pWritePointer;
	bool bWriting;	// TRUE if there is an GetAllocation() outstanding for this ring buffer
	byte* volatile pReadPointer;
	uint32		 alignment;
	CThreadEvent dataWrittenEvent;	// The event used to signal the reader thread when the ring buffer has data to read
};