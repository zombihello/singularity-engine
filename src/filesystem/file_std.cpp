#include "pch_filesystem.h"
#include "filesystem/file_std.h"

/*
==================
CStreamDataStdFileReader::CStreamDataStdFileReader
==================
*/
CStreamDataStdFileReader::CStreamDataStdFileReader( FILE* pFile, const std::string& path )
	: CBaseStreamDataFileReader( path )
	, pFile( pFile )
{}

/*
==================
CStreamDataStdFileReader::~CStreamDataStdFileReader
==================
*/
CStreamDataStdFileReader::~CStreamDataStdFileReader()
{
	fclose( pFile );
}

/*
==================
CStreamDataStdFileReader::GetSize
==================
*/
uint64 CStreamDataStdFileReader::GetSize() const
{
	uint64	currentPosition = Tell();

	fseek( pFile, 0, SEEK_END );
	uint64	sizeFile = Tell();
	fseek( pFile, ( long )currentPosition, SEEK_SET );
	return sizeFile;
}

/*
==================
CStreamDataStdFileReader::Seek
==================
*/
void CStreamDataStdFileReader::Seek( uint64 position )
{
	fseek( pFile, ( long )position, SEEK_SET );
}

/*
==================
CStreamDataStdFileReader::Flush
==================
*/
void CStreamDataStdFileReader::Flush()
{}

/*
==================
CStreamDataStdFileReader::Tell
==================
*/
uint64 CStreamDataStdFileReader::Tell() const
{
	return ftell( pFile );
}

/*
==================
CStreamDataStdFileReader::Read
==================
*/
void CStreamDataStdFileReader::Read( void* pBuffer, uint64 size )
{
	fread( pBuffer, size, 1, pFile );
}

/*
==================
CStreamDataStdFileReader::IsEndOfStream
==================
*/
bool CStreamDataStdFileReader::IsEndOfStream() const
{
	return feof( pFile ) != 0;
}

/*
==================
CStreamDataStdFileWriter::CStreamDataStdFileWriter
==================
*/
CStreamDataStdFileWriter::CStreamDataStdFileWriter( FILE* pFile, const std::string& path )
	: CBaseStreamDataFileWriter( path )
	, pFile( pFile )
{}

/*
==================
CStreamDataStdFileWriter::~CStreamDataStdFileWriter
==================
*/
CStreamDataStdFileWriter::~CStreamDataStdFileWriter()
{
	Flush();
	fclose( pFile );
}

/*
==================
CStreamDataStdFileWriter::GetSize
==================
*/
uint64 CStreamDataStdFileWriter::GetSize() const
{
	// Make sure that all data is written before looking at file size
	const_cast<CStreamDataStdFileWriter*>( this )->Flush();
	uint64	currentPosition = Tell();

	fseek( pFile, 0, SEEK_END );
	uint64	sizeFile = Tell();
	fseek( pFile, ( long )currentPosition, SEEK_SET );
	return sizeFile;
}

/*
==================
CStreamDataStdFileWriter::Seek
==================
*/
void CStreamDataStdFileWriter::Seek( uint64 position )
{
	Flush();
	fseek( pFile, ( long )position, SEEK_SET );
}

/*
==================
CStreamDataStdFileWriter::Flush
==================
*/
void CStreamDataStdFileWriter::Flush()
{
	fflush( pFile );
}

/*
==================
CStreamDataStdFileWriter::Tell
==================
*/
uint64 CStreamDataStdFileWriter::Tell() const
{
	const_cast<CStreamDataStdFileWriter*>( this )->Flush();
	return ftell( pFile );
}

/*
==================
CStreamDataStdFileWriter::Write
==================
*/
void CStreamDataStdFileWriter::Write( void* pBuffer, uint64 size )
{
	fwrite( pBuffer, size, 1, pFile );
	Flush();
}

/*
==================
CStreamDataStdFileWriter::IsEndOfStream
==================
*/
bool CStreamDataStdFileWriter::IsEndOfStream() const
{
	return feof( pFile ) != 0;
}