#pragma once

/*
==================
CBaseStreamDataFile::IsReader
==================
*/
template<class TBaseClass>
FORCEINLINE bool CBaseStreamDataFile<TBaseClass>::IsReader() const
{
	return false;
}

/*
==================
CBaseStreamDataFile::IsWriter
==================
*/
template<class TBaseClass>
FORCEINLINE bool CBaseStreamDataFile<TBaseClass>::IsWriter() const
{
	return false;
}

/*
==================
CBaseStreamDataFile::GetPath
==================
*/
template<class TBaseClass>
FORCEINLINE const char* CBaseStreamDataFile<TBaseClass>::GetPath() const
{
	return !path.empty() ? path.c_str() : "";
}

/*
==================
CBaseStreamDataFile::GetType
==================
*/
template<class TBaseClass>
FORCEINLINE streamDataType_t CBaseStreamDataFile<TBaseClass>::GetType() const
{
	return STREAMDATA_TYPE_FILE;
}

/*
==================
CBaseStreamDataFileReader::IsReader
==================
*/
FORCEINLINE bool CBaseStreamDataFileReader::IsReader() const
{
	return true;
}

/*
==================
CBaseStreamDataFileWriter::IsWriter
==================
*/
FORCEINLINE bool CBaseStreamDataFileWriter::IsWriter() const
{
	return true;
}
