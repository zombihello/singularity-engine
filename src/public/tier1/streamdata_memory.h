#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier0/types.h"
#include "tier1/istreamdata.h"

//-----------------------------------------------------------------------------
// The base class for read/write memory stream data
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CBaseStreamDataMemory : public TRefCounted<TBaseClass>
{
public:
	CBaseStreamDataMemory()
		: offset( 0 )
	{
	}

	// IStreamData interface
	virtual uint64 Tell() const override;
	virtual void   Seek( uint64 position ) override;
	virtual void   Flush() override;

	virtual bool			 IsReader() const override;
	virtual bool			 IsWriter() const override;
	virtual const char*		 GetPath() const override;
	virtual streamDataType_t GetType() const override;

protected:
	uint64 offset;
};

//-----------------------------------------------------------------------------
// The class for reading from the memory
//-----------------------------------------------------------------------------
class CStreamDataMemoryReader : public CBaseStreamDataMemory<IStreamDataReader>
{
public:
	CStreamDataMemoryReader( const byte* pData, uint64 size )
		: pData( pData )
		, size( size )
	{
	}

	// IStreamDataReader interface
	virtual void Read( void* pBuffer, uint64 size ) override;

	virtual bool   IsEndOfStream() const override;
	virtual bool   IsReader() const override;
	virtual uint64 GetSize() const override;

private:
	const byte* pData;
	uint64		size;
};

//-----------------------------------------------------------------------------
// The class for write to the memory
//-----------------------------------------------------------------------------
class CStreamDataMemoryWriter : public CBaseStreamDataMemory<IStreamDataWriter>
{
public:
	CStreamDataMemoryWriter( eastl::vector<byte>& data )
		: data( data )
	{
	}

	// IStreamDataWriter interface
	virtual void Write( void* pBuffer, uint64 size ) override;

	virtual bool   IsEndOfStream() const override;
	virtual bool   IsWriter() const override;
	virtual uint64 GetSize() const override;

private:
	eastl::vector<byte>& data;
};

#include "tier1/streamdata_memory.inl"
