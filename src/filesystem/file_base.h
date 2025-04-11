/**
 * @file
 * @addtogroup filesystem filesystem
 */

#ifndef FILE_BASE_H
#define FILE_BASE_H

#include <string>
#include "stdlib/istreamdata.h"

/**
 * @ingroup filesystem
 * @brief The base class for file stream data
 */
template<class TBaseClass>
class CBaseStreamDataFile : public TRefCounted<TBaseClass>
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseStreamDataFile( const std::string& path )
		: path( path )
	{}

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
		return !path.empty() ? path.c_str() : "";
	}

	/**
	 * @brief Get the stream data type
	 * @return Return the stream data type
	 */
	virtual streamDataType_t GetType() const override
	{
		return STREAMDATA_TYPE_FILE;
	}

private:
	std::string		path;	/**< Path to file */
};

/**
 * @ingroup filesystem
 * @brief The base class for reading from a file
 */
class CBaseStreamDataFileReader : public CBaseStreamDataFile<IStreamDataReader>
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseStreamDataFileReader( const std::string& path )
		: CBaseStreamDataFile<IStreamDataReader>( path )
	{}

	/**
	 * @breif Is this stream reader
	 * @return Return TRUE if this stream is reader, otherwise returns FALSE
	 */
	virtual bool IsReader() const override
	{
		return true;
	}
};

/**
 * @ingroup filesystem
 * @brief The base class for write to the file
 */
class CBaseStreamDataFileWriter : public CBaseStreamDataFile<IStreamDataWriter>
{
public:
	/**
	 * @brief Constructor
	 * @param path	Path to file
	 */
	CBaseStreamDataFileWriter( const std::string& path )
		: CBaseStreamDataFile<IStreamDataWriter>( path )
	{}

	/**
	 * @brief Is this stream writer
	 * @return Return TRUE if this stream is writer, otherwise returns FALSE
	 */
	virtual bool IsWriter() const override
	{
		return true;
	}
};

#endif // !FILE_BASE_H