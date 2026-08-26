#pragma once
#include "schemasystem/ischemasystem.h"
#include "schemasystem/ischemadata.h"

//-----------------------------------------------------------------------------
// The base class for schema serializers
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CSchemaDataBase : public CRefCounted<TBaseClass>
{
public:
	// IStreamData interface
	virtual uint64			 Tell() const override;
	virtual void			 Seek( uint64 position ) override;
	virtual void			 Flush() override;
	virtual bool			 IsReader() const override;
	virtual bool			 IsWriter() const override;
	virtual bool			 IsEndOfStream() const override;
	virtual uint64			 GetSize() const override;
	virtual const char*		 GetPath() const override;
	virtual streamDataType_t GetType() const override;

	CSchemaDataBase( IStreamData* pStream );

protected:
	IStreamData* pStream;
};

//-----------------------------------------------------------------------------
// The schema serializers
//-----------------------------------------------------------------------------
class CSchemaDataReaderBinary : public CSchemaDataBase<ISchemaDataReader>
{
public:
	// IStreamData interface
	virtual bool IsReader() const override;

	// IStreamDataReader interface
	virtual void Read( void* pBuffer, uint64 size ) override;

	// ISchemaDataReader interface
	virtual bool ReadBool( bool& value ) override;
	virtual bool ReadInt8( int8& value ) override;
	virtual bool ReadInt16( int16& value ) override;
	virtual bool ReadInt32( int32& value ) override;
	virtual bool ReadInt64( int64& value ) override;
	virtual bool ReadUInt8( uint8& value ) override;
	virtual bool ReadUInt16( uint16& value ) override;
	virtual bool ReadUInt32( uint32& value ) override;
	virtual bool ReadUInt64( uint64& value ) override;
	virtual bool ReadFloat( float& value ) override;
	virtual bool ReadDouble( double& value ) override;

	virtual schemaDataFormat_t GetFormat() const override;

	CSchemaDataReaderBinary( IStreamDataReader* pStream, schemaDataFormat_t format = SCHEMA_DATA_FORMAT_TAGGED );

protected:
	// Reads `size` bytes when the stream really has them
	bool ReadChecked( void* pBuffer, uint64 size );

private:
	schemaDataFormat_t format;
};

class CSchemaDataWriterBinary : public CSchemaDataBase<ISchemaDataWriter>
{
public:
	// IStreamData interface
	virtual bool IsWriter() const override;

	// IStreamDataWriter interface
	virtual void Write( void* pBuffer, uint64 size ) override;

	// ISchemaDataWriter interface
	virtual void WriteBool( bool value ) override;
	virtual void WriteInt8( int8 value ) override;
	virtual void WriteInt16( int16 value ) override;
	virtual void WriteInt32( int32 value ) override;
	virtual void WriteInt64( int64 value ) override;
	virtual void WriteUInt8( uint8 value ) override;
	virtual void WriteUInt16( uint16 value ) override;
	virtual void WriteUInt32( uint32 value ) override;
	virtual void WriteUInt64( uint64 value ) override;
	virtual void WriteFloat( float value ) override;
	virtual void WriteDouble( double value ) override;

	virtual schemaDataFormat_t GetFormat() const override;

	CSchemaDataWriterBinary( IStreamDataWriter* pStream, schemaDataFormat_t format = SCHEMA_DATA_FORMAT_TAGGED );

private:
	schemaDataFormat_t format;
};

#include "schemasystem/schemalib/schemadata.inl"
