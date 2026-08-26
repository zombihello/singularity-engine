#pragma once

/*
==================
CSchemaDataBase::CSchemaDataBase
==================
*/
template<class TBaseClass>
FORCEINLINE CSchemaDataBase<TBaseClass>::CSchemaDataBase( IStreamData* pStream )
	: pStream( pStream )
{
	Assert( pStream );
}

/*
==================
CSchemaDataBase::Tell
==================
*/
template<class TBaseClass>
FORCEINLINE uint64 CSchemaDataBase<TBaseClass>::Tell() const
{
	return pStream->Tell();
}

/*
==================
CSchemaDataBase::Seek
==================
*/
template<class TBaseClass>
FORCEINLINE void CSchemaDataBase<TBaseClass>::Seek( uint64 position )
{
	pStream->Seek( position );
}

/*
==================
CSchemaDataBase::Flush
==================
*/
template<class TBaseClass>
FORCEINLINE void CSchemaDataBase<TBaseClass>::Flush()
{
	pStream->Flush();
}

/*
==================
CSchemaDataBase::IsReader
==================
*/
template<class TBaseClass>
FORCEINLINE bool CSchemaDataBase<TBaseClass>::IsReader() const
{
	return false;
}

/*
==================
CSchemaDataBase::IsWriter
==================
*/
template<class TBaseClass>
FORCEINLINE bool CSchemaDataBase<TBaseClass>::IsWriter() const
{
	return false;
}

/*
==================
CSchemaDataBase::IsEndOfStream
==================
*/
template<class TBaseClass>
FORCEINLINE bool CSchemaDataBase<TBaseClass>::IsEndOfStream() const
{
	return pStream->IsEndOfStream();
}

/*
==================
CSchemaDataBase::GetSize
==================
*/
template<class TBaseClass>
FORCEINLINE uint64 CSchemaDataBase<TBaseClass>::GetSize() const
{
	return pStream->GetSize();
}

/*
==================
CSchemaDataBase::GetPath
==================
*/
template<class TBaseClass>
FORCEINLINE const char* CSchemaDataBase<TBaseClass>::GetPath() const
{
	return pStream->GetPath();
}

/*
==================
CSchemaDataBase::GetType
==================
*/
template<class TBaseClass>
FORCEINLINE streamDataType_t CSchemaDataBase<TBaseClass>::GetType() const
{
	return pStream->GetType();
}
