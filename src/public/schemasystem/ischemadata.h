#pragma once
#include "tier1/istreamdata.h"
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// Schema serialize data format
//-----------------------------------------------------------------------------
enum schemaDataFormat_t
{
	SCHEMA_DATA_FORMAT_RAW,		// Values only, in the order the type declares its fields. Smallest, and worth nothing once the type changes
	SCHEMA_DATA_FORMAT_TAGGED,	// Every field carries a tag, so the data survives fields moving, appearing and going away
	SCHEMA_DATA_NUM_FORMATS
};

//-----------------------------------------------------------------------------
// Reading described data back
//-----------------------------------------------------------------------------
class ISchemaDataReader : public IStreamDataReader
{
public:
	virtual bool ReadBool( bool& value )	 = 0;
	virtual bool ReadInt8( int8& value )	 = 0;
	virtual bool ReadInt16( int16& value )	 = 0;
	virtual bool ReadInt32( int32& value )	 = 0;
	virtual bool ReadInt64( int64& value )	 = 0;
	virtual bool ReadUInt8( uint8& value )	 = 0;
	virtual bool ReadUInt16( uint16& value ) = 0;
	virtual bool ReadUInt32( uint32& value ) = 0;
	virtual bool ReadUInt64( uint64& value ) = 0;
	virtual bool ReadFloat( float& value )	 = 0;
	virtual bool ReadDouble( double& value ) = 0;

	virtual schemaDataFormat_t GetFormat() const = 0;
};

//-----------------------------------------------------------------------------
// Writing described data out
//-----------------------------------------------------------------------------
class ISchemaDataWriter : public IStreamDataWriter
{
public:
	virtual void WriteBool( bool value )	 = 0;
	virtual void WriteInt8( int8 value )	 = 0;
	virtual void WriteInt16( int16 value )	 = 0;
	virtual void WriteInt32( int32 value )	 = 0;
	virtual void WriteInt64( int64 value )	 = 0;
	virtual void WriteUInt8( uint8 value )	 = 0;
	virtual void WriteUInt16( uint16 value ) = 0;
	virtual void WriteUInt32( uint32 value ) = 0;
	virtual void WriteUInt64( uint64 value ) = 0;
	virtual void WriteFloat( float value )	 = 0;
	virtual void WriteDouble( double value ) = 0;

	virtual schemaDataFormat_t GetFormat() const = 0;
};
