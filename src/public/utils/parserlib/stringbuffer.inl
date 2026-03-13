#pragma once

/*
==================
CParserStringBuffer::Clear
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE void CParserStringBuffer<TCharType, TCharContainerType>::Clear()
{
	if ( pBuffer )
	{
		pBuffer->ReleaseRef();
	}
	pBuffer = NULL;
}

/*
==================
CParserStringBuffer<char, eastl::string>::AsChar
==================
*/
template<>
FORCEINLINE const char* CParserStringBuffer<char, eastl::string>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : "";
}

/*
==================
CParserStringBuffer<wchar_t, eastl::wstring>::AsChar
==================
*/
template<>
FORCEINLINE const wchar_t* CParserStringBuffer<wchar_t, eastl::wstring>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : L"";
}

/*
==================
CParserStringBuffer::Clear
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool CParserStringBuffer<TCharType, TCharContainerType>::IsEmpty() const
{
	return pBuffer;
}

/*
==================
CParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE CParserStringBuffer<TCharType, TCharContainerType>& CParserStringBuffer<TCharType, TCharContainerType>::operator=( const CParserStringBuffer& other )
{
	if ( this != &other )
	{
		if ( pBuffer )
		{
			pBuffer->ReleaseRef();
		}
		pBuffer = other.pBuffer;
		if ( pBuffer )
		{
			pBuffer->AddRef();
		}
	}

	return *this;
}

/*
==================
CParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE CParserStringBuffer<TCharType, TCharContainerType>& CParserStringBuffer<TCharType, TCharContainerType>::operator=( const TCharContainerType& string )
{
	if ( pBuffer )
	{
		pBuffer->ReleaseRef();
	}

	if ( !string.empty() )
	{
		pBuffer = new CBuffer( string );
		pBuffer->AddRef();
	}
	else
	{
		pBuffer = NULL;
	}
	return *this;
}

/*
==================
CParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE CParserStringBuffer<TCharType, TCharContainerType>& CParserStringBuffer<TCharType, TCharContainerType>::operator=( const TCharType* pString )
{
	if ( pString )
	{
		pString->ReleaseRef();
	}

	if ( pString && *pString )
	{
		pString = new CBuffer( pString );
		pString->AddRef();
	}
	else
	{
		pString = NULL;
	}
	return *this;
}

/*
==================
CParserStringBuffer::operator==
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool CParserStringBuffer<TCharType, TCharContainerType>::operator==( const CParserStringBuffer& other ) const
{
	if ( pBuffer == other.pBuffer )
	{
		return true;
	}
	return !S_Strcmp( AsChar(), other.AsChar() );
}

/*
==================
CParserStringBuffer::operator==
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool CParserStringBuffer<TCharType, TCharContainerType>::operator==( const TCharType* pString ) const
{
	return !S_Strcmp( AsChar(), pString );
}

/*
==================
CParserStringBuffer::operator!=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool CParserStringBuffer<TCharType, TCharContainerType>::operator!=( const CParserStringBuffer& other ) const
{
	return !( operator==( other ) );
}

/*
==================
CParserStringBuffer::operator!=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool CParserStringBuffer<TCharType, TCharContainerType>::operator!=( const TCharType* pString ) const
{
	return !( operator!=( pString ) );
}
