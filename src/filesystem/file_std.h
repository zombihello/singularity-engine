#ifndef FILE_STD_H
#define FILE_STD_H

#include <stdio.h>

#include "core/platform.h"
#include "filesystem/file_base.h"

//-----------------------------------------------------------------------------
// The class for reading from a std file
//-----------------------------------------------------------------------------
class CStreamDataStdFileReader : public CBaseStreamDataFileReader
{
public:
	CStreamDataStdFileReader( FILE* pFile, const std::string& path );
	virtual ~CStreamDataStdFileReader();

	// IStreamData interface
	virtual uint64 Tell() const override;
	virtual void Seek( uint64 position ) override;
	virtual void Flush() override;
	
	virtual bool IsEndOfStream() const override;
	virtual uint64 GetSize() const override;
	
	// IStreamDataReader interface
	virtual void Read( void* pBuffer, uint64 size ) override;

	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;
};


//-----------------------------------------------------------------------------
// The class for write to the std file
//-----------------------------------------------------------------------------
class CStreamDataStdFileWriter : public CBaseStreamDataFileWriter
{
public:
	CStreamDataStdFileWriter( FILE* pFile, const std::string& path );
	virtual ~CStreamDataStdFileWriter();

	// IStreamData interface
	virtual uint64 Tell() const override;
	virtual void Seek( uint64 position ) override;
	virtual void Flush() override;

	virtual bool IsEndOfStream() const override;
	virtual uint64 GetSize() const override;

	// IStreamDataWriter interface
	virtual void Write( void* pBuffer, uint64 size ) override;

	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;
};

#endif // !FILE_STD_H