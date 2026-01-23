#pragma once
#include "tier0/types.h"
#include "tier1/refcount.h"

//-----------------------------------------------------------------------------
// Stream data interfaces
//-----------------------------------------------------------------------------
enum streamDataType_t
{
	STREAMDATA_TYPE_MEMORY,
	STREAMDATA_TYPE_FILE
};

class IStreamData : public IRefCounted
{
public:
	virtual uint64 Tell() const			   = 0;
	virtual void   Seek( uint64 position ) = 0;
	virtual void   Flush()				   = 0;

	virtual bool			 IsWriter() const	   = 0;
	virtual bool			 IsReader() const	   = 0;
	virtual bool			 IsEndOfStream() const = 0;
	virtual uint64			 GetSize() const	   = 0;
	virtual const char*		 GetPath() const	   = 0;
	virtual streamDataType_t GetType() const	   = 0;
};

class IStreamDataReader : public IStreamData
{
public:
	virtual void Read( void* pBuffer, uint64 size ) = 0;
};

class IStreamDataWriter : public IStreamData
{
public:
	virtual void Write( void* pBuffer, uint64 size ) = 0;
};
