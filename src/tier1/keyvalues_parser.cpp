#include "pch_tier1.h"
#include "utils/interfaces/interfaces.h"
#include "tier1/filetools.h"
#include "cvar/icvar.h"
#include "tier1/keyvalues_parser.h"

/*
==================
CKeyValuesParser::Setup
==================
*/
void CKeyValuesParser::Setup( IStreamDataReader* pStreamReader )
{
	// Save the stream reader
	PROFILE_SCOPE();
	Assert( pStreamReader );
	buffer.SetStream( pStreamReader );

	// Initialize line ranges
	lineRanges.clear();
	uint64 lineStart = buffer.Tell();
	while ( !buffer.IsEndOfBuffer() )
	{
		const char c	   = buffer.Peek();
		uint64	   lineEnd = buffer.Tell();
		if ( c == '\n' )
		{
			lineRanges.emplace_back( eastl::make_pair( lineStart, lineEnd ) );
			lineStart = lineEnd + 1;
		}

		buffer.Advance( 1 );
	}
	lineRanges.emplace_back( eastl::make_pair( lineStart, buffer.Tell() ) );
	buffer.Seek( 0 );
}

/*
==================
CKeyValuesParser::Parse
==================
*/
void CKeyValuesParser::Parse( CKeyValues* pKeyValues, IStreamDataReader* pStreamReader )
{
	PROFILE_SCOPE();
	Assert( pKeyValues );
	Assert( pStreamReader );
	Setup( pStreamReader );
	ReadKeyValues( pKeyValues );
}

/*
==================
CKeyValuesParser::ReadKeyValue
==================
*/
bool CKeyValuesParser::ReadKeyValues( CKeyValues* pKeyValues )
{
	// Keep parsing until we hit the end of the buffer or a parse error
	PROFILE_SCOPE();
	eastl::vector<CKeyValues*> includedKeys;
	eastl::vector<CKeyValues*> baseKeys;
	uint32					   blockScopeLevel = scopeLevel;
	while ( !buffer.IsEndOfBuffer() )
	{
		// Skip whitespaces and comments
		SkipSpacesAndComments();
		if ( buffer.IsEndOfBuffer() )
		{
			break;
		}

		// Read current token
		token_t token = ReadToken();
		if ( token.string.empty() )
		{
			EmitError( token.position, "Got empty token" );
			break;
		}

		// Check control symbols if the token isn't quoted
		if ( token.type != TOKEN_TYPE_QUOTED )
		{
			// Top level closed, stop reading
			if ( scopeLevel > 0 && token.string[0] == '}' )
			{
				--scopeLevel;
				break;
			}

			// Read special macroses
			if ( token.string[0] == '#' )
			{
				// Include macro
				if ( token.string == "#include" )
				{
					if ( !ReadIncludeKeys( includedKeys ) )
					{
						break;
					}
				}
				// Base macro
				else if ( token.string == "#base" )
				{
					if ( !ReadIncludeKeys( baseKeys ) )
					{
						break;
					}
				}
				// Otherwise it is an unknown macros
				else
				{
					EmitError( token.position, "Unknown macros '%s'", token.string.c_str() );
					break;
				}

				continue;
			}
		}

		// Read a key-value
		// The current token it is a key name
		eastl::string name = eastl::move( token.string );
		if ( token.type == TOKEN_TYPE_CONTROL )
		{
			EmitError( token.position, "Unexpected control symbol" );
			break;
		}

		// Create a new sub key
		bool		bAccepted = true;
		CKeyValues* pSubKey	  = new CKeyValues( name.c_str(), pKeyValues );

		// Read conditional block if exists
		if ( !ReadConditionalBlock( bAccepted ) )
		{
			delete pSubKey;
			break;
		}

		// The next token it is a value
		token = ReadToken();

		// If the token it is '{' then read sub key-values
		if ( token.string[0] == '{' )
		{
			// Read sub key-values
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
			eastl::string value = eastl::move( token.string );
			if ( token.type == TOKEN_TYPE_CONTROL )
			{
				EmitError( token.position, "Unexpected control symbol" );
				delete pSubKey;
				break;
			}

			// Try to deduce type
			char*		pEndInt64	= NULL;	 // Position where int64 scan ended
			char*		pEndDouble	= NULL;	 // Position where double scan ended
			const char* pValueStr	= value.c_str();
			const char* pEndString	= pValueStr + value.size();
			int64		valueInt64	= S_Atoi64( pValueStr, &pEndInt64 );
			double		valueDouble = S_Atod( pValueStr, &pEndDouble );

			// Set as string if the value length is zero
			if ( value.size() == 0 )
			{
				pSubKey->SetString( NULL, "" );
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
					pSubKey->SetDouble( NULL, valueDouble );
				}
				else
				{
					pSubKey->SetFloat( NULL, (float)valueDouble );
				}
			}
			// Set as int32/int64 if int64 scan ended at end of the value
			else if ( pEndInt64 == pEndString )
			{
				if ( valueInt64 >= S_MinValue<int32>() && valueInt64 <= S_MaxValue<int32>() )
				{
					pSubKey->SetInt( NULL, (int32)valueInt64 );
				}
				else
				{
					pSubKey->SetInt64( NULL, valueInt64 );
				}
			}
			// Otherwise set as string
			else
			{
				eastl::string convertedValueBuffer;
				S_ConvertUnescapeToEscapeSymbols( convertedValueBuffer, value.c_str() );
				pSubKey->SetString( NULL, convertedValueBuffer.c_str() );
			}

			// Read conditional block if exists
			if ( !ReadConditionalBlock( bAccepted ) )
			{
				delete pSubKey;
				break;
			}
		}

		// Delete the subKey if it isn't accepted
		if ( !bAccepted )
		{
			delete pSubKey;
		}
	}

	// All sub scopes must be closed
	if ( scopeLevel > 0 && scopeLevel >= blockScopeLevel )
	{
		EmitError( buffer.Tell(), "Got EOF instead of '}'" );
	}

	// Append include keys into the key values
	for ( uint32 index = 0, count = (uint32)includedKeys.size(); index < count; ++index )
	{
		CKeyValues* pIncludedKeyValues = includedKeys[index];
		AppendIncludedKeys( pKeyValues, pIncludedKeyValues );
		delete pIncludedKeyValues;
	}

	// Merge base keys
	for ( uint32 index = 0, count = (uint32)baseKeys.size(); index < count; ++index )
	{
		CKeyValues* pBaseKeyValues = baseKeys[index];
		MergeBaseKeys( pKeyValues, pBaseKeyValues );
		delete pBaseKeyValues;
	}

	return !HasErrors();
}

/*
==================
CKeyValuesParser::ReadConditionalBlock
==================
*/
bool CKeyValuesParser::ReadConditionalBlock( bool& bAccepted )
{
	PROFILE_SCOPE();
	Assert( g_pCvar );
	static eastl::pair<const char*, bool> s_constantVars[] = {
		eastl::make_pair( "$WINDOWS", PLATFORM_WINDOWS )

		// Here you can add your constant variables
	};

	bAccepted			 = false;
	uint64	prevPosition = buffer.Tell();
	token_t token		 = ReadToken();
	if ( token.string.empty() || token.type == TOKEN_TYPE_QUOTED || token.string[0] != '[' )
	{
		bAccepted = true;
		buffer.Seek( prevPosition );
		return true;
	}

	bool				bExpectVar			 = true;
	bool				bEndConditionalBlock = false;
	eastl::vector<bool> bConditionalGroups;
	uint32				curConditionalGroupId = 0;
	bConditionalGroups.emplace_back( true );
	while ( !buffer.IsEndOfBuffer() )
	{
		// Skip whitespaces and comments
		SkipSpacesAndComments();
		if ( buffer.IsEndOfBuffer() )
		{
			break;
		}

		// Read current token
		token = ReadToken();
		if ( token.string.empty() )
		{
			EmitError( token.position, "Got empty token" );
			break;
		}

		if ( token.type != TOKEN_TYPE_QUOTED )
		{
			// Check if we reach end of the conditional block
			if ( token.string[0] == ']' )
			{
				bEndConditionalBlock = true;
				break;
			}
			// Should we go to a new conditional group
			else if ( token.string[0] == '|' )
			{
				bExpectVar = true;
				++curConditionalGroupId;
				bConditionalGroups.emplace_back( true );
				continue;
			}
		}

		// Evaluate the conditional variable
		bExpectVar		= false;
		bool bEvaluated = false;
		bool bNot		= false;  // Should we negate this command?
		if ( token.string[0] == '!' )
		{
			token.string.erase( token.string.begin() );
			bNot = true;
		}

		// First, let's see if it's a constant variable
		for ( uint32 index = 0, count = ARRAYSIZE( s_constantVars ); index < count; ++index )
		{
			const eastl::pair<const char*, bool>& constantVar = s_constantVars[index];
			if ( token.string == constantVar.first )
			{
				bEvaluated = true;
				bConditionalGroups[curConditionalGroupId] &= constantVar.second ^ bNot;
				break;
			}
		}

		// Otherwise, it must be a cvar
		if ( !bEvaluated )
		{
			IConVar* pConVar = g_pCvar->FindVar( token.string.c_str() );
			if ( pConVar )
			{
				bEvaluated = true;
				bConditionalGroups[curConditionalGroupId] &= pConVar->GetBool() ^ bNot;
			}
			else
			{
				EmitError( token.position, "Unknown cvar '%s'", token.string.c_str() );
			}
		}
	}

	// A conditional block must be closed by ']'
	if ( !bEndConditionalBlock )
	{
		EmitError( token.position, "Got EOF instead of ']'" );
	}
	// A conditional block must have a var
	else if ( bExpectVar )
	{
		EmitError( token.position, "Got ']' instead of a var" );
	}
	// Otherwise everything is ok, we calculate the final result of the condition
	else
	{
		for ( uint32 index = 0, count = (uint32)bConditionalGroups.size(); index < count && !bAccepted; ++index )
		{
			bAccepted = bConditionalGroups[index];
		}
	}

	return !HasErrors();
}

/*
==================
CKeyValuesParser::ReadIncludeKeys
==================
*/
bool CKeyValuesParser::ReadIncludeKeys( eastl::vector<CKeyValues*>& includedKeys )
{
	// Read a file path
	PROFILE_SCOPE();
	Assert( g_pFileSystem );

	token_t token = ReadToken();
	if ( token.string.empty() )
	{
		EmitError( token.position, "Got empty token" );
		return false;
	}

	// Get relative subdirectory
	eastl::string fullPath;
	S_GetFilePath( buffer.GetStream()->GetPath(), fullPath, false );
	size_t offset = fullPath.size();
	fullPath.resize( fullPath.size() + token.string.size() );
	Mem_Memcpy( fullPath.data() + offset, token.string.data(), token.string.size() );

	// Try open the file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( fullPath.c_str() );
	if ( !pFile )
	{
		EmitError( token.position, "Failed to load keyvalue file '%s'", fullPath.c_str() );
		return false;
	}

	// Create a new key values and parse it
	CKeyValues*		 pKeyValues = new CKeyValues( fullPath.c_str() );
	CKeyValuesParser keyValuesParser;
	keyValuesParser.Parse( pKeyValues, pFile );
	if ( keyValuesParser.HasErrors() )
	{
		const eastl::vector<eastl::string>& errorMsgs = keyValuesParser.GetErrorMsgs();
		CKeyValuesParser::errorMsgs.reserve( CKeyValuesParser::errorMsgs.size() + errorMsgs.size() );
		CKeyValuesParser::errorMsgs.insert( CKeyValuesParser::errorMsgs.end(), errorMsgs.begin(), errorMsgs.end() );

		delete pKeyValues;
		return false;
	}

	includedKeys.emplace_back( pKeyValues );
	return true;
}

/*
==================
CKeyValuesParser::MergeBaseKeys
==================
*/
void CKeyValuesParser::MergeBaseKeys( CKeyValues* pNewKeyValues, CKeyValues* pBaseKeyValues )
{
	PROFILE_SCOPE();
	Assert( pNewKeyValues );
	Assert( pBaseKeyValues );

	// Merge ourselves
	// We always want to keep our value in the new key values, so nothing to do here

	// Now merge base children
	eastl::list<CKeyValues*> unmergedBaseChildren;
	for ( CKeyValuesSubKeysIterator baseIt( pBaseKeyValues, true, true ); baseIt; ++baseIt )
	{
		CKeyValues* pBaseChild = *baseIt;
		CKeyValues* pNewChild  = pNewKeyValues->FindKey( pBaseChild->GetName() );
		if ( pNewChild )
		{
			MergeBaseKeys( pNewChild, pBaseChild );
		}
		else
		{
			unmergedBaseChildren.emplace_back( pBaseChild );
		}
	}

	// Append unmerged base children
	for ( auto it = unmergedBaseChildren.begin(), itEnd = unmergedBaseChildren.end(); it != itEnd; ++it )
	{
		pNewKeyValues->AddSubKey( *it );
	}
}

/*
==================
CKeyValuesParser::SkipSpacesAndComments
==================
*/
void CKeyValuesParser::SkipSpacesAndComments()
{
	PROFILE_SCOPE();
	while ( !buffer.IsEndOfBuffer() )
	{
		// Skip spaces
		while ( !buffer.IsEndOfBuffer() && S_IsSpace( buffer.Peek() ) )
		{
			buffer.Advance( 1 );
		}

		// Stray multi-line comment terminator '*/' outside of comment
		if ( IsEndMultilineComment() )
		{
			EmitError( buffer.Tell(), "Unexpected '*/' comment terminator" );
		}

		// Skip line comments
		if ( IsBeginLineComment() )
		{
			while ( !buffer.IsEndOfBuffer() && !IsEndLineComment() )
			{
				buffer.Advance( 1 );
			}
			continue;
		}

		// Skip block comments
		if ( IsBeginMultilineComment() )
		{
			while ( !buffer.IsEndOfBuffer() && !IsEndMultilineComment() )
			{
				buffer.Advance( 1 );
			}

			if ( IsEndMultilineComment() )
			{
				buffer.Advance( 2 );
			}
			continue;
		}

		// We are done
		break;
	}
}

/*
==================
CKeyValuesParser::ReadToken
==================
*/
void CKeyValuesParser::ReadToken( token_t& token )
{
	// Before read skip whitespaces and comments
	PROFILE_SCOPE();
	SkipSpacesAndComments();
	token.string.clear();
	token.position = buffer.Tell();
	if ( buffer.IsEndOfBuffer() )
	{
		return;
	}
	token.string.reserve( 256 );

	// Read as quoted token
	if ( buffer.Peek() == '"' )
	{
		// Skip "
		token.type = TOKEN_TYPE_QUOTED;
		buffer.Advance( 1 );
		++token.position;

		bool bIsEscapeSymbol = false;
		while ( !buffer.IsEndOfBuffer() && ( bIsEscapeSymbol || buffer.Peek() != '"' ) )
		{
			const char c	= buffer.Peek();
			bIsEscapeSymbol = c == '\\';

			token.string += c;
			buffer.Advance( 1 );
		}

		// Validate the token
		if ( buffer.IsEndOfBuffer() )
		{
			EmitError( buffer.Tell(), "Got EOF instead of '\"'" );
		}
	}
	// Read as control symbol
	else if ( IsControlSymbol() )
	{
		token.type = TOKEN_TYPE_CONTROL;
		for ( uint32 index = 0, count = GetControlSymbolSize(); index < count; ++index )
		{
			token.string += buffer.Peek();
			buffer.Advance( 1 );
		}
	}
	// Read as bare token
	else
	{
		// Advance the end pointer until whitespaces or control tokens
		token.type			 = TOKEN_TYPE_BARE;
		bool bIsEscapeSymbol = false;
		while ( !buffer.IsEndOfBuffer() && ( bIsEscapeSymbol || !IsControlSymbol() ) && !S_IsSpace( buffer.Peek() ) )
		{
			const char c	= buffer.Peek();
			bIsEscapeSymbol = c == '\\';

			token.string += c;
			buffer.Advance( 1 );
		}
	}

	// Skip " if was quoted
	if ( token.type == TOKEN_TYPE_QUOTED && !buffer.IsEndOfBuffer() )
	{
		buffer.Advance( 1 );
	}
}

/*
==================
CKeyValuesParser::GetPostionInCode
==================
*/
void CKeyValuesParser::GetPostionInCode( uint64 streamOffset, uint64& line, uint64& column ) const
{
	line   = 0;
	column = 0;
	for ( uint64 index = 0, count = lineRanges.size(); index < count; ++index )
	{
		const eastl::pair<uint64, uint64>& lineRange = lineRanges[index];
		if ( streamOffset >= lineRange.first && streamOffset <= lineRange.second )
		{
			line   = index + 1;
			column = ( streamOffset - lineRange.first ) + 1;
			break;
		}
	}
}

/*
==================
CKeyValuesParser::CBuffer::Precache
==================
*/
void CKeyValuesParser::CBuffer::Precache( uint32 requestedSize )
{
	// We don't do precache if we already have enough precached data size
	PROFILE_SCOPE();
	Assert( requestedSize <= BUFFER_SIZE );
	uint32 unusedPrecachedSize = precachedSize - bufferOffset;
	if ( unusedPrecachedSize >= requestedSize )
	{
		return;
	}

	// Do nothing if we reach to the end of the stream
	if ( pStreamReader->IsEndOfStream() )
	{
		return;
	}

	// If there might not be enough space at the end of the buffer, we move the tail to the beginning
	// Idea: we make it so that the data [bufferOffset, requestedSize) ends up at the beginning [0, unusedPrecachedSize)
	// and the bufferOffset becomes 0
	if ( unusedPrecachedSize != 0 )
	{
		Mem_Memmove( pBuffer, pBuffer + bufferOffset, unusedPrecachedSize );
		streamOffset += bufferOffset;
		precachedSize = unusedPrecachedSize;
		bufferOffset  = 0;
	}
	else
	{
		streamOffset = pStreamReader->Tell();
	}

	// Now we have the tail at the beginning, and there's free space at the end
	// We load exactly as much as we need
	uint32 restRequestedSize = Min( BUFFER_SIZE - precachedSize, (uint32)( pStreamReader->GetSize() - streamOffset ) );
	pStreamReader->Read( pBuffer + precachedSize, restRequestedSize );
	precachedSize += restRequestedSize;
}
