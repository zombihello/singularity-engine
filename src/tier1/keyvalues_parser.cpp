#include "pch_tier1.h"
#include "tier1/keyvalues_parser.h"

/*
==================
CKeyValuesParser::Setup
==================
*/
void CKeyValuesParser::Setup( const char* pFile, const char* pBuffer, uint64 size )
{
	// Initialize buffer pointers
	lineRanges.clear();
	pCurFile = pFile;
	pCurPtr	 = pBuffer;
	pEndPtr	 = pBuffer + size;

	// Initialize line ranges
	const char* pStartLine = pCurPtr;
	const char* pEndLine   = pStartLine;
	while ( !IsEndOfBuffer( pEndLine ) )
	{
		if ( *pEndLine == '\n' )
		{
			lineRanges.emplace_back( eastl::string_view( pStartLine, (uint64)( pEndLine - pStartLine ) ) );
			pStartLine = pEndLine + 1;
		}
		++pEndLine;
	}
	lineRanges.emplace_back( eastl::string_view( pStartLine, (uint64)( pEndLine - pStartLine ) ) );
}

/*
==================
CKeyValuesParser::Parse
==================
*/
void CKeyValuesParser::Parse( const char* pFile, CKeyValues* pRootKeyValue, const char* pBuffer, uint64 size )
{
	Assert( pRootKeyValue );
	Setup( pFile, pBuffer, size );
	ReadKeyValues( pRootKeyValue );
}

/*
==================
CKeyValuesParser::ReadKeyValue
==================
*/
bool CKeyValuesParser::ReadKeyValues( CKeyValues* pKeyValue )
{
	// Keep parsing until we hit the end of the buffer or a parse error
	eastl::string valueBuffer;
	uint32		  blockScopeLevel = scopeLevel;
	while ( !IsEndOfBuffer( pCurPtr ) )
	{
		// Skip whitespaces and comments
		SkipSpacesAndComments();
		if ( IsEndOfBuffer( pCurPtr ) )
		{
			break;
		}

		// Read current token
		bool			   bQuotedToken = false;
		eastl::string_view token		= ReadToken( bQuotedToken );
		if ( token.empty() )
		{
			EmitError( token.data(), "Got empty token" );
			break;
		}

		// Check control symbols if the token isn't quoted
		if ( !bQuotedToken )
		{
			// Top level closed, stop reading
			if ( scopeLevel > 0 && *token.data() == '}' )
			{
				--scopeLevel;
				break;
			}

			// Read conditional block
			if ( *token.data() == '[' )
			{
				// TODO BS yehor.pohuliaka - [<CONDITION>]
				AssertUnimplemented();
				break;
			}

			// Read special macroses
			if ( *token.data() == '#' )
			{
				// TODO BS yehor.pohuliaka - #include <path> and #base <path>
				AssertUnimplemented();
				break;
			}
		}

		// Read a key-value
		// The current token it is a key name
		eastl::string_view name = token;
		if ( !bQuotedToken && IsControlSymbol( name.data() ) )
		{
			EmitError( token.data(), "Unexpected control symbol" );
			break;
		}

		// Create a new sub key
		CKeyValues* pSubKey = new CKeyValues( name.data(), name.size(), pKeyValue );

		// The next token it is a value
		token = ReadToken( bQuotedToken );

		// If the token it is '{' then read sub key-values
		if ( *token.data() == '{' )
		{
			++scopeLevel;
			if ( !ReadKeyValues( pSubKey ) )
			{
				delete pSubKey;
				break;
			}
		}
		// Otherwise it is a simple value
		else
		{
			eastl::string_view value = token;
			if ( !bQuotedToken && IsControlSymbol( value.data() ) )
			{
				EmitError( token.data(), "Unexpected control symbol" );
				delete pSubKey;
				break;
			}

			// We copy the token into a temporary buffer so that the string is null-terminated
			uint64 valueLength = value.size();
			if ( valueBuffer.size() < valueLength + 1 )
			{
				valueBuffer.resize( valueLength + 1 );
			}
			char* pValueStr = valueBuffer.data();
			Mem_Memcpy( pValueStr, value.data(), valueLength );
			valueBuffer[valueLength] = '\0';

			// Try to deduce type
			char*		pEndInt64	= NULL;	 // Position where int64 scan ended
			char*		pEndDouble	= NULL;	 // Position where double scan ended
			const char* pEndString	= pValueStr + valueLength;
			int64		valueInt64	= S_Atoi64( pValueStr, &pEndInt64 );
			double		valueDouble = S_Atod( pValueStr, &pEndDouble );

			// Set as string if the value length is zero
			if ( valueLength == 0 )
			{
				pSubKey->SetString( "" );
			}
			// Set as float/double if double scan ended at end of the value
			else if ( pEndDouble > pEndInt64 && pEndDouble == pEndString )
			{
				// The value must be a double if it is infinite, or outside the range of a float,
				// or has an exponent, or has more decimal places than a float can accept without loss
				bool bMustBeDouble = !S_IsFinite( valueDouble ) || valueDouble < S_MinValue<float>() || valueDouble > S_MaxValue<float>();
				if ( !bMustBeDouble )
				{
					bool   bHasExponent		 = false;
					bool   bSeenDot			 = false;
					uint32 numDigitsAfterDot = 0;
					for ( const char* pCurChar = pValueStr; pCurChar < pEndString; ++pCurChar )
					{
						// We are already sure that the value is a valid number, so to determine
						// that the value has exponent it is enough to simply mark and stop
						char c = *pCurChar;
						if ( c == 'e' || c == 'E' )
						{
							bHasExponent = true;
							break;
						}

						// If we find a point, note that we start counting the numbers
						if ( c == '.' )
						{
							bSeenDot = true;
							continue;
						}
						if ( !bSeenDot )
						{
							continue;
						}

						// If we encounter something non-digital exit from the loop
						if ( !S_IsDigit( c ) )
						{
							break;
						}

						// Otherwise count digits after a dot
						++numDigitsAfterDot;
					}

					bMustBeDouble = bHasExponent || numDigitsAfterDot > S_Digits10<float>();
				}

				if ( bMustBeDouble )
				{
					pSubKey->SetDouble( valueDouble );
				}
				else
				{
					pSubKey->SetFloat( (float)valueDouble );
				}
			}
			// Set as int32/int64 if int64 scan ended at end of the value
			else if ( pEndInt64 == pEndString )
			{
				if ( valueInt64 >= S_MinValue<int32>() && valueInt64 <= S_MaxValue<int32>() )
				{
					pSubKey->SetInt( (int32)valueInt64 );
				}
				else
				{
					pSubKey->SetInt64( valueInt64 );
				}
			}
			// Otherwise set as string
			else
			{
				pSubKey->SetString( valueBuffer.c_str() );
			}
		}
	}

	// All sub scopes must be closed
	if ( scopeLevel > 0 && scopeLevel >= blockScopeLevel && !HasErrors() )
	{
		EmitError( pCurPtr, "Got EOF instead of '}'" );
	}
	return !HasErrors();
}

/*
==================
CKeyValuesParser::SkipSpacesAndComments
==================
*/
void CKeyValuesParser::SkipSpacesAndComments()
{
	while ( !IsEndOfBuffer( pCurPtr ) )
	{
		// Skip spaces
		while ( pCurPtr < pEndPtr && S_IsSpace( *pCurPtr ) )
		{
			++pCurPtr;
		}

		// Stray multi-line comment terminator '*/' outside of comment
		if ( IsEndComment( pCurPtr, true ) )
		{
			EmitError( pCurPtr, "Unexpected '*/' comment terminator" );
		}

		// Skip line and block comments
		if ( IsBeginComment( pCurPtr ) )
		{
			// Line comment
			if ( pCurPtr[1] == '/' )
			{
				while ( pCurPtr < pEndPtr && !IsEndComment( pCurPtr ) )
				{
					++pCurPtr;
				}
				continue;
			}

			// Multi-line comment
			if ( pCurPtr[1] == '*' )
			{
				while ( pCurPtr < pEndPtr )
				{
					if ( IsEndComment( pCurPtr, true ) )
					{
						pCurPtr += 2;
						break;
					}
					++pCurPtr;
				}
				continue;
			}
		}
		break;
	}
}

/*
==================
CKeyValuesParser::ReadToken
==================
*/
eastl::string_view CKeyValuesParser::ReadToken( bool& bQuoted )
{
	// Before read skip whitespaces and comments
	SkipSpacesAndComments();
	if ( IsEndOfBuffer( pCurPtr ) )
	{
		return eastl::string_view( pEndPtr, 0 );
	}

	// Read as bare token if the one isn't begin on '"'
	const char* pStartToken = pCurPtr;
	const char* pEndToken	= pStartToken;
	bQuoted					= *pStartToken == '"';

	// Read as quoted token
	if ( bQuoted )
	{
		// Skip "
		++pStartToken;
		pEndToken = pStartToken;

		bool bIsEscapeSymbol = false;
		while ( !IsEndOfBuffer( pEndToken ) && ( bIsEscapeSymbol || *pEndToken != '"' ) )
		{
			bIsEscapeSymbol = *pEndToken == '\\';
			++pEndToken;
		}

		// Validate the token
		if ( IsEndOfBuffer( pEndToken ) )
		{
			EmitError( pEndToken, "Got EOF instead of '\"'" );
		}
	}
	// Read as control symbol
	else if ( IsControlSymbol( pStartToken ) )
	{
		pEndToken += GetControlSymbolSize( pStartToken );
	}
	// Read as bare token
	else
	{
		// Advance the end pointer until whitespaces or control tokens
		while ( !IsEndOfBuffer( pEndToken ) && !IsControlSymbol( pEndToken ) && !S_IsSpace( *pEndToken ) )
		{
			++pEndToken;
		}
	}

	// Skip " if was quoted
	pCurPtr = pEndToken;
	if ( bQuoted && !IsEndOfBuffer( pCurPtr ) )
	{
		++pCurPtr;
	}
	return eastl::string_view( pStartToken, (uint64)( pEndToken - pStartToken ) );
}

/*
==================
CKeyValuesParser::GetTokenPostion
==================
*/
void CKeyValuesParser::GetTokenPostion( const char* pToken, uint64& line, uint64& column ) const
{
	line   = 0;
	column = 0;
	for ( uint64 index = 0, count = lineRanges.size(); index < count; ++index )
	{
		const eastl::string_view& curLine	 = lineRanges[index];
		const char*				  pStartLine = curLine.data();
		const char*				  pEndLine	 = pStartLine + curLine.size();
		if ( pToken >= pStartLine && pToken <= pEndLine )
		{
			line   = index + 1;
			column = (uint64)( pToken - pStartLine ) + 1;
			break;
		}
	}
}
