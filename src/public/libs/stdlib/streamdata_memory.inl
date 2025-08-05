#pragma once

/*
==================
CBaseStreamDataMemory::Tell
==================
*/
template<class TBaseClass>
uint64 CBaseStreamDataMemory<TBaseClass>::Tell() const
{
	return offset;
}

/*
==================
CBaseStreamDataMemory::Seek
==================
*/
template<class TBaseClass>
void CBaseStreamDataMemory<TBaseClass>::Seek( uint64 position )
{
	offset = Min( position, GetSize() );
}

/*
==================
CBaseStreamDataMemory::Flush
==================
*/
template<class TBaseClass>
void CBaseStreamDataMemory<TBaseClass>::Flush()
{}

/*
==================
CBaseStreamDataMemory::IsEndOfStream
==================
*/
template<class TBaseClass>
bool CBaseStreamDataMemory<TBaseClass>::IsEndOfStream() const
{
	uint64		size = GetSize();
	return Tell() == size;
}

/*
==================
CBaseStreamDataMemory::IsReader
==================
*/
template<class TBaseClass>
bool CBaseStreamDataMemory<TBaseClass>::IsReader() const
{
	return false;
}

/*
==================
CBaseStreamDataMemory::IsWriter
==================
*/
template<class TBaseClass>
bool CBaseStreamDataMemory<TBaseClass>::IsWriter() const
{
	return false;
}

/*
==================
CBaseStreamDataMemory::GetPath
==================
*/
template<class TBaseClass>
const achar* CBaseStreamDataMemory<TBaseClass>::GetPath() const
{
	return "";
}

/*
==================
CBaseStreamDataMemory::GetType
==================
*/
template<class TBaseClass>
streamDataType_t CBaseStreamDataMemory<TBaseClass>::GetType() const
{
	return STREAMDATA_TYPE_MEMORY;
}
