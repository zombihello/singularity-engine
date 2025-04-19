#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include "core/core.h"
#include "stdlib/refcount.h"

//-----------------------------------------------------------------------------
// Parser string buffer (shared string)
//-----------------------------------------------------------------------------
template<typename TCharType, class TCharContainerType>
class TParserStringBuffer
{
public:
	TParserStringBuffer()
		: pBuffer( NULL )
	{}

	TParserStringBuffer( const TCharType* pString )
		: pBuffer( NULL )
	{
		if ( pString && *pString )
		{
			pBuffer = new CBuffer( pString );
			pBuffer->AddRef();
		}
	}

	TParserStringBuffer( const TCharContainerType& string )
		: pBuffer( NULL )
	{
		if ( !string.empty() )
		{
			pBuffer = new CBuffer( string );
			pBuffer->AddRef();
		}
	}

	TParserStringBuffer( const TParserStringBuffer& other )
		: pBuffer( other.pBuffer )
	{
		if ( pBuffer )
		{
			pBuffer->AddRef();
		}
	}

	~TParserStringBuffer()
	{
		if ( pBuffer )
		{
			pBuffer->ReleaseRef();
		}
	}

	void Clear();
	bool IsEmpty() const;
	const TCharType* AsChar() const;

	TParserStringBuffer& operator=( const TParserStringBuffer& other );
	TParserStringBuffer& operator=( const TCharContainerType& string );
	TParserStringBuffer& operator=( const TCharType* pString );
	bool operator==( const TParserStringBuffer& other ) const;
	bool operator==( const TCharType* pString ) const;
	bool operator!=( const TParserStringBuffer& other ) const;
	bool operator!=( const TCharType* pString ) const;

private:
	class CBuffer : public TRefCounted<IRefCounted>
	{
	public:
		CBuffer( const TCharContainerType& string )
			: text( string )
		{}

		CBuffer( const TCharType* pString )
			: text( pString )
		{}

		TCharContainerType		text;
	};

	CBuffer*		pBuffer;
};


//-----------------------------------------------------------------------------
// Default types of ANSI and Unicode string buffer
//-----------------------------------------------------------------------------
typedef TParserStringBuffer<achar, std::string>					parserStringBufferANSI_t;
typedef TParserStringBuffer<wchar, std::wstring>				parserStringBufferWCHAR_t;

#include "parserlib/stringbuffer.inl"

#endif // !STRINGBUFFER_H