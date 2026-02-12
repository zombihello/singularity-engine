#include "pch_tier1.h"
#include "tier1/streamdata_memory.h"

/*
==================
CStreamDataMemoryReader::Read
==================
*/
void CStreamDataMemoryReader::Read( void* pBuffer, uint64 size )
{
	Assert( offset <= GetSize() - size );
	Mem_Memcpy( pBuffer, pData + offset, size );
	offset += size;
}

/*
==================
CStreamDataMemoryReader::IsEndOfStream
==================
*/
bool CStreamDataMemoryReader::IsEndOfStream() const
{
	return Tell() == GetSize();
}

/*
==================
CStreamDataMemoryReader::IsReader
==================
*/
bool CStreamDataMemoryReader::IsReader() const
{
	return true;
}

/*
==================
CStreamDataMemoryReader::GetSize
==================
*/
uint64 CStreamDataMemoryReader::GetSize() const
{
	return size;
}

/*
==================
CStreamDataMemoryWriter::Write
==================
*/
void CStreamDataMemoryWriter::Write( void* pBuffer, uint64 size )
{
	if ( offset + size > GetSize() )
	{
		data.resize( GetSize() + size );
	}

	Mem_Memcpy( data.data() + offset, pBuffer, size );
	offset += size;
}

/*
==================
CStreamDataMemoryWriter::IsEndOfStream
==================
*/
bool CStreamDataMemoryWriter::IsEndOfStream() const
{
	return false;
}

/*
==================
CStreamDataMemoryWriter::IsWriter
==================
*/
bool CStreamDataMemoryWriter::IsWriter() const
{
	return true;
}

/*
==================
CStreamDataMemoryWriter::GetSize
==================
*/
uint64 CStreamDataMemoryWriter::GetSize() const
{
	return (uint64)data.size();
}
