/**
 * @file
 * @addtogroup filesystem filesystem
 */

#ifndef FILE_STD_H
#define FILE_STD_H

#include <stdio.h>

#include "core/platform.h"
#include "filesystem/file_base.h"

/**
 * @ingroup filesystem
 * @brief The class for reading from a std file
 */
class CStreamDataStdFileReader : public CBaseStreamDataFileReader
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param pFile	Pointer to std file
	 * @param path	Path to file
	 */
	CStreamDataStdFileReader( FILE* pFile, const std::string& path );

	/**
	 * @brief Destructor
	 */
	virtual ~CStreamDataStdFileReader();

	/**
	 * @brief Read data
	 *
	 * @param pBuffer	Pointer to buffer for read
	 * @param size		Size of buffer
	 */
	virtual void Read( void* pBuffer, uint64 size ) override;

	/**
	 * @brief Get current position in the stream
	 * @return Return current position in the stream
	 */
	virtual uint64 Tell() const override;

	/**
	 * @brief Set current position in the stream
	 * @param position	New position in the stream
	 */
	virtual void Seek( uint64 position ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * @breif Is end of the stream
	 * @return Return TRUE if the end of the stream, otherwise will return FALSE
	 */
	virtual bool IsEndOfStream() const override;

	/**
	 * @brief Get size of the stream data
	 * @return Return size of the stream data
	 */
	virtual uint64 GetSize() const override;

	/**
	 * @brief Get file handle
	 * @return Return pointer to std file
	 */
	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;	/**< Pointer to file */
};

/**
 * @ingroup filesystem
 * @brief The class for write to the std file
 */
class CStreamDataStdFileWriter : public CBaseStreamDataFileWriter
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param pFile	Pointer to std file
	 * @param path	Path to file
	 */
	CStreamDataStdFileWriter( FILE* pFile, const std::string& path );

	/**
	 * @brief Destructor
	 */
	virtual ~CStreamDataStdFileWriter();

	/**
	 * @brief Write data
	 *
	 * @param pBuffer	Pointer to buffer for write
	 * @param size		Size of buffer
	 */
	virtual void Write( void* pBuffer, uint64 size ) override;

	/**
	 * @brief Get current position in the stream
	 * @return Return current position in the stream
	 */
	virtual uint64 Tell() const override;

	/**
	 * @brief Set current position in the stream
	 * @param position	New position in the stream
	 */
	virtual void Seek( uint64 position ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * @breif Is end of the stream
	 * @return Return TRUE if the end of the stream, otherwise will return FALSE
	 */
	virtual bool IsEndOfStream() const override;

	/**
	 * @brief Get size of the stream data
	 * @return Return size of the stream data
	 */
	virtual uint64 GetSize() const override;

	/**
	 * @brief Get file handle
	 * @return Return pointer to std file
	 */
	FORCEINLINE FILE* GetHandle() const
	{
		return pFile;
	}

private:
	FILE*	pFile;	/**< Pointer to file */
};

#endif // !FILE_STD_H