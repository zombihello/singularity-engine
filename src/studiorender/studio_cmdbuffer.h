/**
 * @file
 * @addtogroup studiorender studiorender
 */

#ifndef STUDIO_CMDBUFFER_H
#define STUDIO_CMDBUFFER_H

#include "core/threading.h"
#include "studiorender/istudio_cmdbuffer.h"

/**
 * @ingroup studiorender
 * @brief Studio render command buffer
 */
class CStudioCmdBuffer : public IStudioCmdBuffer
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param bufferSize	The size of the data buffer to allocate
	 * @param alignment		Alignment of each allocation unit (in bytes)
	 */
	CStudioCmdBuffer( uint32 bufferSize, uint32 alignment = 1 );

	/**
	 * @brief Destructor
	 */
	~CStudioCmdBuffer();

	/**
	 * @brief Allocate a chunk
	 * @warning After filling in the allocated chunk you have to call CommitAllocation
	 *
	 * @param allocationSize	The size of the allocation to make.
	 * @return Return context of allocated chunk
	 */
	virtual studioCmdAlloc_t GetAllocation( uint32 allocationSize ) override;

	/**
	 * @brief Commit the allocated chunk
	 * @param allocContext	Allocated chunk context
	 */
	virtual void CommitAllocation( studioCmdAlloc_t& allocContext ) override;

	/**
	 * @brief Flush render commands
	 */
	virtual void Flush() override;

	/**
	 * @brief Checks if there is data to be read from the ring buffer, and if so accesses the pointer to the data to be read
	 *
	 * @param pReadPointer	When returning TRUE, this will hold the pointer to the data to read
	 * @param readSize		When returning TRUE, this will hold the number of bytes available to read
	 * @return Return TRUE if there is data to be read, otherwise returns FALSE
	 */
	bool BeginRead( void*& pReadPointer, uint32& readSize );

	/**
	 * @brief Frees the first ReadSize bytes available for reading via BeginRead to the writing thread
	 * @param readSize	The number of bytes to free
	 */
	void EndRead( uint32 readSize );

	/**
	 * @brief Waits for data to be available for reading
	 * @param waitTime	Time in milliseconds to wait before returning
	 */
	void WaitForRead( uint32 waitTime = -1 );

	/**
	 * @brief Checks if some data has been written to or not
	 * @return Return TRUE if buffer not empty, FALSE otherwise
	 */
	FORCEINLINE bool IsReadBufferEmpty() const
	{
		return pReadPointer == pWritePointer;
	}

private:
	byte*				pData;					/**< Data buffer */
	byte*				pDataEnd;				/**< The first byte after the end of the data buffer */
	byte* volatile		pWritePointer;			/**< The next byte to be written to */
	bool				bWriting;				/**< TRUE if there is an GetAllocation() outstanding for this ring buffer */
	byte* volatile		pReadPointer;			/**< The next byte to be read from */
	uint32				alignment;				/**< Alignment of each allocation unit (in bytes) */
	CThreadEvent		dataWrittenEvent;		/**< The event used to signal the reader thread when the ring buffer has data to read */
};

#endif // !STUDIO_CMDBUFFER_H