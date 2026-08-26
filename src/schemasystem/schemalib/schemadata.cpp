#include "pch_schemalib.h"
#include "schemasystem/schemalib/schemadata.h"

//-----------------------------------------------------------------------------
// Help macros to implement read/write methods
//-----------------------------------------------------------------------------
#define SCHEMA_IMPLEMENT_BINARY_READ( ValueType, MethodName )    \
	bool CSchemaDataReaderBinary::MethodName( ValueType& value ) \
	{                                                            \
		return ReadChecked( &value, sizeof( value ) );           \
	}

#define SCHEMA_IMPLEMENT_BINARY_WRITE( ValueType, MethodName )  \
	void CSchemaDataWriterBinary::MethodName( ValueType value ) \
	{                                                           \
		Write( (void*)&value, sizeof( value ) );                \
	}

/*
==================
CSchemaDataReaderBinary::CSchemaDataReaderBinary
==================
*/
CSchemaDataReaderBinary::CSchemaDataReaderBinary( IStreamDataReader* pStream, schemaDataFormat_t format /* = SCHEMA_DATA_FORMAT_TAGGED */ )
	: CSchemaDataBase<ISchemaDataReader>( pStream )
	, format( format )
{
}

/*
==================
CSchemaDataReaderBinary::IsReader
==================
*/
bool CSchemaDataReaderBinary::IsReader() const
{
	return true;
}

/*
==================
CSchemaDataReaderBinary::Read
==================
*/
void CSchemaDataReaderBinary::Read( void* pBuffer, uint64 size )
{
	( (IStreamDataReader*)pStream )->Read( pBuffer, size );
}

/*
==================
CSchemaDataReaderBinary::GetFormat
==================
*/
schemaDataFormat_t CSchemaDataReaderBinary::GetFormat() const
{
	return format;
}

/*
==================
CSchemaDataReaderBinary::ReadChecked
==================
*/
bool CSchemaDataReaderBinary::ReadChecked( void* pBuffer, uint64 size )
{
	if ( Tell() + size > GetSize() )
	{
		return false;
	}

	Read( pBuffer, size );
	return true;
}

SCHEMA_IMPLEMENT_BINARY_READ( bool, ReadBool )
SCHEMA_IMPLEMENT_BINARY_READ( int8, ReadInt8 )
SCHEMA_IMPLEMENT_BINARY_READ( int16, ReadInt16 )
SCHEMA_IMPLEMENT_BINARY_READ( int32, ReadInt32 )
SCHEMA_IMPLEMENT_BINARY_READ( int64, ReadInt64 )
SCHEMA_IMPLEMENT_BINARY_READ( uint8, ReadUInt8 )
SCHEMA_IMPLEMENT_BINARY_READ( uint16, ReadUInt16 )
SCHEMA_IMPLEMENT_BINARY_READ( uint32, ReadUInt32 )
SCHEMA_IMPLEMENT_BINARY_READ( uint64, ReadUInt64 )
SCHEMA_IMPLEMENT_BINARY_READ( float, ReadFloat )
SCHEMA_IMPLEMENT_BINARY_READ( double, ReadDouble )
#undef SCHEMA_IMPLEMENT_BINARY_READ

/*
==================
CSchemaDataWriterBinary::CSchemaDataWriterBinary
==================
*/
CSchemaDataWriterBinary::CSchemaDataWriterBinary( IStreamDataWriter* pStream, schemaDataFormat_t format /* = SCHEMA_DATA_FORMAT_TAGGED */ )
	: CSchemaDataBase<ISchemaDataWriter>( pStream )
	, format( format )
{
}

/*
==================
CSchemaDataWriterBinary::IsWriter
==================
*/
bool CSchemaDataWriterBinary::IsWriter() const
{
	return true;
}

/*
==================
CSchemaDataWriterBinary::Write
==================
*/
void CSchemaDataWriterBinary::Write( void* pBuffer, uint64 size )
{
	( (IStreamDataWriter*)pStream )->Write( pBuffer, size );
}

/*
==================
CSchemaDataWriterBinary::GetFormat
==================
*/
schemaDataFormat_t CSchemaDataWriterBinary::GetFormat() const
{
	return format;
}

SCHEMA_IMPLEMENT_BINARY_WRITE( bool, WriteBool )
SCHEMA_IMPLEMENT_BINARY_WRITE( int8, WriteInt8 )
SCHEMA_IMPLEMENT_BINARY_WRITE( int16, WriteInt16 )
SCHEMA_IMPLEMENT_BINARY_WRITE( int32, WriteInt32 )
SCHEMA_IMPLEMENT_BINARY_WRITE( int64, WriteInt64 )
SCHEMA_IMPLEMENT_BINARY_WRITE( uint8, WriteUInt8 )
SCHEMA_IMPLEMENT_BINARY_WRITE( uint16, WriteUInt16 )
SCHEMA_IMPLEMENT_BINARY_WRITE( uint32, WriteUInt32 )
SCHEMA_IMPLEMENT_BINARY_WRITE( uint64, WriteUInt64 )
SCHEMA_IMPLEMENT_BINARY_WRITE( float, WriteFloat )
SCHEMA_IMPLEMENT_BINARY_WRITE( double, WriteDouble )
#undef SCHEMA_IMPLEMENT_BINARY_WRITE
