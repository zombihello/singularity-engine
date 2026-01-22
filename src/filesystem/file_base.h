#pragma once
#include <EASTL/string.h>
#include "stdlib/istreamdata.h"

//-----------------------------------------------------------------------------
// The base class for file stream data
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CBaseStreamDataFile : public TRefCounted<TBaseClass>
{
public:
	CBaseStreamDataFile( const eastl::string& path )
		: path( path )
	{
	}

	// IStreamData interface
	virtual bool			 IsReader() const override;
	virtual bool			 IsWriter() const override;
	virtual const char*		 GetPath() const override;
	virtual streamDataType_t GetType() const override;

private:
	eastl::string path;
};

//-----------------------------------------------------------------------------
// The base class for reading from a file
//-----------------------------------------------------------------------------
class CBaseStreamDataFileReader : public CBaseStreamDataFile<IStreamDataReader>
{
public:
	CBaseStreamDataFileReader( const eastl::string& path )
		: CBaseStreamDataFile<IStreamDataReader>( path )
	{
	}

	// IStreamData interface
	bool IsReader() const override;
};

//-----------------------------------------------------------------------------
// The base class for write to the file
//-----------------------------------------------------------------------------
class CBaseStreamDataFileWriter : public CBaseStreamDataFile<IStreamDataWriter>
{
public:
	CBaseStreamDataFileWriter( const eastl::string& path )
		: CBaseStreamDataFile<IStreamDataWriter>( path )
	{
	}

	// IStreamData interface
	virtual bool IsWriter() const override;
};

#include "filesystem/file_base.inl"
