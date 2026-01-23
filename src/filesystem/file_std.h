#pragma once
#include <stdio.h>

#include "tier0/defines.h"
#include "filesystem/file_base.h"

//-----------------------------------------------------------------------------
// The class for reading from a eastl file
//-----------------------------------------------------------------------------
class CStreamDataStdFileReader : public CBaseStreamDataFileReader
{
public:
	CStreamDataStdFileReader( FILE* pFile, const eastl::string& path );
	virtual ~CStreamDataStdFileReader();

	// IStreamData interface
	virtual uint64 Tell() const override;
	virtual void   Seek( uint64 position ) override;
	virtual void   Flush() override;

	virtual bool   IsEndOfStream() const override;
	virtual uint64 GetSize() const override;

	// IStreamDataReader interface
	virtual void Read( void* pBuffer, uint64 size ) override;

	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE* pFile;
};

//-----------------------------------------------------------------------------
// The class for write to the eastl file
//-----------------------------------------------------------------------------
class CStreamDataStdFileWriter : public CBaseStreamDataFileWriter
{
public:
	CStreamDataStdFileWriter( FILE* pFile, const eastl::string& path );
	virtual ~CStreamDataStdFileWriter();

	// IStreamData interface
	virtual uint64 Tell() const override;
	virtual void   Seek( uint64 position ) override;
	virtual void   Flush() override;

	virtual bool   IsEndOfStream() const override;
	virtual uint64 GetSize() const override;

	// IStreamDataWriter interface
	virtual void Write( void* pBuffer, uint64 size ) override;

	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE* pFile;
};