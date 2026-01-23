#pragma once

/*
==================
TParserStringBuffer::Clear
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE void TParserStringBuffer<TCharType, TCharContainerType>::Clear()
{
	if ( pBuffer )
	{
		pBuffer->ReleaseRef();
	}
	pBuffer = NULL;
}

/*
==================
TParserStringBuffer<char, eastl::string>::AsChar
==================
*/
template<>
FORCEINLINE const char* TParserStringBuffer<char, eastl::string>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : "";
}

/*
==================
TParserStringBuffer<wchar_t, eastl::wstring>::AsChar
==================
*/
template<>
FORCEINLINE const wchar_t* TParserStringBuffer<wchar_t, eastl::wstring>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : L"";
}

/*
==================
TParserStringBuffer::Clear
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool TParserStringBuffer<TCharType, TCharContainerType>::IsEmpty() const
{
	return pBuffer;
}

/*
==================
TParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE TParserStringBuffer<TCharType, TCharContainerType>& TParserStringBuffer<TCharType, TCharContainerType>::operator=( const TParserStringBuffer& other )
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
TParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE TParserStringBuffer<TCharType, TCharContainerType>& TParserStringBuffer<TCharType, TCharContainerType>::operator=( const TCharContainerType& string )
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
TParserStringBuffer::operator=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE TParserStringBuffer<TCharType, TCharContainerType>& TParserStringBuffer<TCharType, TCharContainerType>::operator=( const TCharType* pString )
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
TParserStringBuffer::operator==
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool TParserStringBuffer<TCharType, TCharContainerType>::operator==( const TParserStringBuffer& other ) const
{
	if ( pBuffer == other.pBuffer )
	{
		return true;
	}
	return !S_Strcmp( AsChar(), other.AsChar() );
}

/*
==================
TParserStringBuffer::operator==
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool TParserStringBuffer<TCharType, TCharContainerType>::operator==( const TCharType* pString ) const
{
	return !S_Strcmp( AsChar(), pString );
}

/*
==================
TParserStringBuffer::operator!=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool TParserStringBuffer<TCharType, TCharContainerType>::operator!=( const TParserStringBuffer& other ) const
{
	return !( operator==( other ) );
}

/*
==================
TParserStringBuffer::operator!=
==================
*/
template<typename TCharType, class TCharContainerType>
FORCEINLINE bool TParserStringBuffer<TCharType, TCharContainerType>::operator!=( const TCharType* pString ) const
{
	return !( operator!=( pString ) );
}
