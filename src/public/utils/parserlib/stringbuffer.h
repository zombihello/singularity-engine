#pragma once
#include "tier0/tier0.h"
#include "tier1/refcount.h"

//-----------------------------------------------------------------------------
// Parser string buffer (shared string)
//-----------------------------------------------------------------------------
template<typename TCharType, class TCharContainerType>
class CParserStringBuffer
{
public:
	CParserStringBuffer()
		: pBuffer( NULL )
	{
	}

	CParserStringBuffer( const TCharType* pString )
		: pBuffer( NULL )
	{
		if ( pString && *pString )
		{
			pBuffer = new CBuffer( pString );
			pBuffer->AddRef();
		}
	}

	CParserStringBuffer( const TCharContainerType& string )
		: pBuffer( NULL )
	{
		if ( !string.empty() )
		{
			pBuffer = new CBuffer( string );
			pBuffer->AddRef();
		}
	}

	CParserStringBuffer( const CParserStringBuffer& other )
		: pBuffer( other.pBuffer )
	{
		if ( pBuffer )
		{
			pBuffer->AddRef();
		}
	}

	~CParserStringBuffer()
	{
		if ( pBuffer )
		{
			pBuffer->ReleaseRef();
		}
	}

	void			 Clear();
	bool			 IsEmpty() const;
	const TCharType* AsChar() const;

	CParserStringBuffer& operator=( const CParserStringBuffer& other );
	CParserStringBuffer& operator=( const TCharContainerType& string );
	CParserStringBuffer& operator=( const TCharType* pString );
	bool				 operator==( const CParserStringBuffer& other ) const;
	bool				 operator==( const TCharType* pString ) const;
	bool				 operator!=( const CParserStringBuffer& other ) const;
	bool				 operator!=( const TCharType* pString ) const;

private:
	class CBuffer : public CRefCounted<IRefCounted>
	{
	public:
		CBuffer( const TCharContainerType& string )
			: text( string )
		{
		}

		CBuffer( const TCharType* pString )
			: text( pString )
		{
		}

		TCharContainerType text;
	};

	CBuffer* pBuffer;
};

//-----------------------------------------------------------------------------
// Default types of ANSI and Unicode string buffer
//-----------------------------------------------------------------------------
typedef CParserStringBuffer<char, eastl::string>	 parserStringBufferANSI_t;
typedef CParserStringBuffer<wchar_t, eastl::wstring> parserStringBufferWCHAR_t;

#include "utils/parserlib/stringbuffer.inl"
