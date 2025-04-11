/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef ISTREAMDATA_H
#define ISTREAMDATA_H

#include "core/types.h"
#include "stdlib/refcount.h"

/**
 * @ingroup stdlib
 * @brief Stream data type
 */
enum streamDataType_t
{
	STREAMDATA_TYPE_MEMORY,		/**< Memory data stream */
	STREAMDATA_TYPE_FILE		/**< File data stream */
};

/**
 * @ingroup stdlib
 * @brief The base class for stream data
 */
class IStreamData : public IRefCounted
{
public:
	/**
	 * @brief Get current position in the stream
	 * @return Return current position in the stream
	 */
	virtual uint64 Tell() const = 0;

	/**
	 * @brief Set current position in the stream
	 * @param position	New position in the stream
	 */
	virtual void Seek( uint64 position ) = 0;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() = 0;

	/**
	 * @brief Is this stream writer
	 * @return Return TRUE if this stream is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const = 0;

	/**
	 * @breif Is this stream reader
	 * @return Return TRUE if this stream is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const = 0;

	/**
	 * @breif Is end of the stream
	 * @return Return TRUE if the end of the stream, otherwise will return FALSE
	 */
	virtual bool IsEndOfStream() const = 0;

	/**
	 * @brief Get size of the stream data
	 * @return Return size of the stream data
	 */
	virtual uint64 GetSize() const = 0;

	/**
	 * @brief Get path of the stream data
	 * @return Return the path of the stream data
	 */
	virtual const achar* GetPath() const = 0;

	/**
	 * @brief Get the stream data type
	 * @return Return the stream data type
	 */
	virtual streamDataType_t GetType() const = 0;
};

/**
 * @ingroup stdlib
 * @brief The class for read stream data
 */
class IStreamDataReader : public IStreamData
{
public:
	/**
	 * @brief Read data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 */
	virtual void Read( void* pBuffer, uint64 size ) = 0;
};

/**
 * @ingroup stdlib
 * @brief The class for write stream data
 */
class IStreamDataWriter : public IStreamData
{
public:
	/**
	 * @brief Write data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 */
	virtual void Write( void* pBuffer, uint64 size ) = 0;
};

#endif // !ISTREAMDATA_H