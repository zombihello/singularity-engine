/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef STREAMDATA_MEMORY_H
#define STREAMDATA_MEMORY_H

#include <string>
#include <vector>

#include "core/types.h"
#include "stdlib/istreamdata.h"

/**
 * @ingroup stdlib
 * @brief The base class for read/write memory stream data
 */
template<class TBaseClass>
class CBaseStreamDataMemory : public TRefCounted<TBaseClass>
{
public:
	/**
	 * @brief Constructor
	 */
	CBaseStreamDataMemory()
		: offset( 0 )
	{}

	/**
	 * @brief Get current position in the stream
	 * @return Return current position in the stream
	 */
	virtual uint64 Tell() const override
	{
		return offset;
	}

	/**
	 * @brief Set current position in the stream
	 * @param position	New position in the stream
	 */
	virtual void Seek( uint64 position ) override
	{
		offset = Min( position, GetSize() );
	}

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override
	{}

	/**
	 * @breif Is end of the stream
	 * @return Return TRUE if the end of the stream, otherwise will return FALSE
	 */
	virtual bool IsEndOfStream() const override
	{
		uint64		size = GetSize();
		return Tell() == size;
	}

	/**
	 * @breif Is this stream reader
	 * @return Return TRUE if this stream is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const override
	{
		return false;
	}

	/**
	 * @brief Is this stream writer
	 * @return Return TRUE if this stream is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const override
	{
		return false;
	}

	/**
	 * @brief Get path of the stream data
	 * @return Return the path of the stream data
	 */
	virtual const achar* GetPath() const override
	{
		return "";
	}

	/**
	 * @brief Get the stream data type
	 * @return Return the stream data type
	 */
	virtual streamDataType_t GetType() const override
	{
		return STREAMDATA_TYPE_MEMORY;
	}

protected:	
	uint64		offset;		/**< Offset in the data array */
};

/**
 * @ingroup stdlib
 * @brief The class for reading from the memory
 */
class CStreamDataMemoryReader : public CBaseStreamDataMemory<IStreamDataReader>
{
public:
	/**
	 * @brief Constructor
	 * @param pData		Array with data
	 * @param size		Array size
	 */
	CStreamDataMemoryReader( const byte* pData, uint64 size )
		: pData( pData )
		, size( size )
	{}
	
	/**
	 * @brief Read data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 */
	virtual void Read( void* pBuffer, uint64 size ) override;

	/**
	 * @breif Is this stream reader
	 * @return Return TRUE if this stream is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const override;

	/**
	 * @brief Get size of the stream data
	 * @return Return size of the stream data
	 */
	virtual uint64 GetSize() const override
	{
		return size;
	}

private:
	const byte*		pData;	/**< Array with data */
	uint64			size;	/**< Array size */
};

/**
 * @ingroup stdlib
 * @brief The class for write to the memory
 */
class CStreamDataMemoryWriter : public CBaseStreamDataMemory<IStreamDataWriter>
{
public:
	/**
	 * @brief Constructor
	 * @param data		Array with data
	 */
	CStreamDataMemoryWriter( std::vector<byte>& data )
		: data( data )
	{}

	/**
	 * @brief Write data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 */
	virtual void Write( void* pBuffer, uint64 size ) override;

	/**
	 * @brief Is this stream writer
	 * @return Return TRUE if this stream is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const override;

	/**
	 * @brief Get size of the stream data
	 * @return Return size of the stream data
	 */
	virtual uint64 GetSize() const override
	{
		return ( uint64 )data.size();
	}

private:
	std::vector<byte>&		data;		/**< Array with data */
};

#endif // !STREAMDATA_MEMORY_H