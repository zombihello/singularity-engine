/**
 * @file
 * @addtogroup parserlib parserlib
 */

#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include "core/core.h"
#include "stdlib/refcount.h"

/**
 * @ingroup parserlib
 * @brief Parser string buffer (shared string)
 */
template<typename TCharType, class TCharContainerType>
class TParserStringBuffer
{
public:
	/**
	 * @brief Constructor
	 */
	TParserStringBuffer()
		: pBuffer( NULL )
	{}

	/**
	 * @brief Constructor
	 * @param pString	String
	 */
	TParserStringBuffer( const TCharType* pString )
		: pBuffer( NULL )
	{
		if ( pString && *pString )
		{
			pBuffer = new CBuffer( pString );
			pBuffer->AddRef();
		}
	}

	/**
	 * @brief Constructor
	 * @param string	String
	 */
	TParserStringBuffer( const TCharContainerType& string )
		: pBuffer( NULL )
	{
		if ( !string.empty() )
		{
			pBuffer = new CBuffer( string );
			pBuffer->AddRef();
		}
	}

	/**
	 * @brief Constructor
	 * @param other		Other a parser string buffer
	 */
	TParserStringBuffer( const TParserStringBuffer& other )
		: pBuffer( other.pBuffer )
	{
		if ( pBuffer )
		{
			pBuffer->AddRef();
		}
	}

	/**
	 * @brief Destructor
	 */
	~TParserStringBuffer()
	{
		if ( pBuffer )
		{
			pBuffer->ReleaseRef();
		}
	}

	/**
	 * @brief Clear the buffer
	 */
	FORCEINLINE void Clear()
	{
		if ( pBuffer )
		{
			pBuffer->ReleaseRef();
		}
		pBuffer = NULL;
	}

	/**
	 * @brief Get the buffer as array of chars
	 * @retrun Return pointer to first char in the buffer. If buffer is empty returns NULL
	 */
	const TCharType* AsChar() const;

	/**
	 * @brief Is empty the buffer
	 * @return Return TRUE if the buffer is empty, otherwise FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return pBuffer;
	}

	/**
	 * @brief Copy operator
	 * @param other		Other a parser string buffer
	 * @return Return reference to self
	 */
	FORCEINLINE TParserStringBuffer& operator=( const TParserStringBuffer& other )
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

	/**
	 * @brief Copy operator
	 * @param string	String
	 * @return Return reference to self
	 */
	FORCEINLINE TParserStringBuffer& operator=( const TCharContainerType& string )
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

	/**
	 * @brief Copy operator
	 * @param pString	String
	 * @return Return reference to self
	 */
	FORCEINLINE TParserStringBuffer& operator=( const TCharType* pString )
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

	/**
	 * @brief Compare operator 
	 * @param other		Other a parser string buffer
	 * @return Return TRUE if two buffers are equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TParserStringBuffer& other ) const
	{
		if ( pBuffer == other.pBuffer )
		{
			return true;
		}

		return !S_Strcmp( AsChar(), other.AsChar() );
	}

	/**
	 * @brief Compare operator
	 * @param pString	String
	 * @return Return TRUE if two buffers are equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const TCharType* pString ) const
	{
		return !S_Strcmp( AsChar(), pString );
	}

	/**
	 * @brief Compare operator
	 * @param other		Other a parser string buffer
	 * @return Return TRUE if two buffers aren't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const TParserStringBuffer& other ) const
	{
		return !( operator==( other ) );
	}

	/**
	 * @brief Compare operator
	 * @param pString	String
	 * @return Return TRUE if two buffers aren't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const TCharType* pString ) const
	{
		return !( operator!=( pString ) );
	}

private:
	/**
	 * @brief Buffer
	 */
	class CBuffer : public TRefCounted<IRefCounted>
	{
	public:
		/**
		 * @brief Constructor
		 * @param string	String
		 */
		FORCEINLINE CBuffer( const TCharContainerType& string )
			: text( string )
		{}

		/**
		 * @brief Constructor
		 * @param pString	String
		 */
		FORCEINLINE CBuffer( const TCharType* pString )
			: text( pString )
		{}

		TCharContainerType		text;	/**< String */
	};

	CBuffer*		pBuffer;	/**< String buffer */
};

/*
==================
TParserStringBuffer<achar, std::string>::AsChar
==================
*/
template<>
FORCEINLINE const achar* TParserStringBuffer<achar, std::string>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : "";
}

/*
==================
TParserStringBuffer<wchar, std::wstring>::AsChar
==================
*/
template<>
FORCEINLINE const wchar* TParserStringBuffer<wchar, std::wstring>::AsChar() const
{
	return pBuffer ? pBuffer->text.c_str() : L"";
}

/**
 * @ingroup parserlib
 * @brief Parser string buffer for ANSI
 */
typedef TParserStringBuffer<achar, std::string>					parserStringBufferANSI_t;

/**
 * @ingroup parserlib
 * @brief Parser string buffer for Unicode
 */
typedef TParserStringBuffer<wchar, std::wstring>				parserStringBufferWCHAR_t;

#endif // !STRINGBUFFER_H