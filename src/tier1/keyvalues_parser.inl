#pragma once

/*
==================
CKeyValuesParser::CKeyValuesParser
==================
*/
FORCEINLINE CKeyValuesParser::CKeyValuesParser()
	: scopeLevel( 0 )
{
}

/*
==================
CKeyValuesParser::HasErrors
==================
*/
FORCEINLINE bool CKeyValuesParser::HasErrors() const
{
	return !errorMsgs.empty();
}

/*
==================
CKeyValuesParser::GetErrorMsgs
==================
*/
FORCEINLINE const eastl::vector<eastl::string>& CKeyValuesParser::GetErrorMsgs() const
{
	return errorMsgs;
}

/*
==================
CKeyValuesParser::ReadToken
==================
*/
FORCEINLINE CKeyValuesParser::token_t CKeyValuesParser::ReadToken()
{
	token_t token;
	ReadToken( token );
	return token;
}

/*
==================
CKeyValuesParser::EmitError
==================
*/
FORCEINLINE void CKeyValuesParser::EmitError( uint64 streamOffset, const char* pFormat, ... )
{
	va_list params;
	uint64	line   = 0;
	uint64	column = 0;

	va_start( params, pFormat );
	GetPostionInCode( streamOffset, line, column );
	errorMsgs.emplace_back( S_Sprintf( "%s[%i:%i]: %s", buffer.GetStream()->GetPath(), line, column, S_Vsprintf( pFormat, params ).c_str() ) );
	va_end( params );
}

/*
==================
CKeyValuesParser::AppendIncludedKeys
==================
*/
FORCEINLINE void CKeyValuesParser::AppendIncludedKeys( CKeyValues* pKeyValues, CKeyValues* pIncludedKeyValues )
{
	Assert( pKeyValues );
	Assert( pIncludedKeyValues );
	for ( CKeyValuesSubKeysIterator it( pIncludedKeyValues, true, true ); it; ++it )
	{
		pKeyValues->AddSubKey( *it );
	}
}

/*
==================
CKeyValuesParser::IsBeginLineComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsBeginLineComment( uint32 offset /* = 0 */ ) const
{
	return buffer.Peek( offset ) == '/' && buffer.Peek( offset + 1 ) == '/';
}

/*
==================
CKeyValuesParser::IsBeginMultilineComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsBeginMultilineComment( uint32 offset /* = 0 */ ) const
{
	return buffer.Peek( offset ) == '/' && buffer.Peek( offset + 1 ) == '*';
}

/*
==================
CKeyValuesParser::IsBeginMultilineComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsEndLineComment( uint32 offset /* = 0 */ ) const
{
	return buffer.Peek( offset ) == '\n';
}

/*
==================
CKeyValuesParser::IsEndMultilineComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsEndMultilineComment( uint32 offset /* = 0 */ ) const
{
	return buffer.Peek( offset ) == '*' && buffer.Peek( offset + 1 ) == '/';
}

/*
==================
CKeyValuesParser::IsControlSymbol
==================
*/
FORCEINLINE bool CKeyValuesParser::IsControlSymbol( uint32 offset /* = 0 */ ) const
{
	const char c = buffer.Peek( offset );
	return IsBeginLineComment( offset )
		   || IsBeginMultilineComment( offset )
		   || IsEndLineComment( offset )
		   || IsEndMultilineComment( offset )
		   || c == '{' || c == '}'
		   || c == '[' || c == ']'
		   || c == '"' || c == '|';
}

/*
==================
CKeyValuesParser::GetControlSymbolSize
==================
*/
FORCEINLINE uint32 CKeyValuesParser::GetControlSymbolSize( uint32 offset /* = 0 */ ) const
{
	if ( IsControlSymbol( offset ) )
	{
		return ( IsBeginLineComment( offset ) || IsBeginMultilineComment( offset ) || IsEndLineComment( offset ) || IsEndMultilineComment( offset ) ) ? 2 : 1;
	}
	return 0;
}

/*
==================
CKeyValuesParser::CBuffer::CBuffer
==================
*/
FORCEINLINE CKeyValuesParser::CBuffer::CBuffer()
	: precachedSize( 0 )
	, bufferOffset( 0 )
	, streamOffset( 0 )
	, pBuffer( (char*)Mem_MallocZero( BUFFER_SIZE ) )
{
}

/*
==================
CKeyValuesParser::CBuffer::~CBuffer
==================
*/
FORCEINLINE CKeyValuesParser::CBuffer::~CBuffer()
{
	Mem_Free( pBuffer );
}

/*
==================
CKeyValuesParser::CBuffer::SetStream
==================
*/
FORCEINLINE void CKeyValuesParser::CBuffer::SetStream( IStreamDataReader* pStreamReader )
{
	CBuffer::pStreamReader = pStreamReader;
	ResetPrecacheState();
	if ( pStreamReader )
	{
		if ( !pStreamReader->IsEndOfStream() )
		{
			Precache( BUFFER_SIZE );
		}
		else
		{
			streamOffset = pStreamReader->Tell();
		}
	}
}

/*
==================
CKeyValuesParser::CBuffer::Peek
==================
*/
FORCEINLINE char CKeyValuesParser::CBuffer::Peek( uint32 offset /* = 0 */ )
{
	Precache( offset + 1 );
	if ( bufferOffset + offset >= precachedSize )
	{
		return '\0';
	}
	return pBuffer[bufferOffset + offset];
}

/*
==================
CKeyValuesParser::CBuffer::Seek
==================
*/
FORCEINLINE void CKeyValuesParser::CBuffer::Seek( uint64 position )
{
	if ( position < pStreamReader->GetSize() )
	{
		pStreamReader->Seek( position );
		ResetPrecacheState();
		Precache( BUFFER_SIZE );
	}
}

/*
==================
CKeyValuesParser::CBuffer::Advance
==================
*/
FORCEINLINE void CKeyValuesParser::CBuffer::Advance( uint32 offset )
{
	if ( !IsEndOfBuffer() )
	{
		bufferOffset += offset;
		if ( bufferOffset >= precachedSize )
		{
			Seek( Tell() );
		}
	}
}

/*
==================
CKeyValuesParser::CBuffer::ResetPrecacheState
==================
*/
FORCEINLINE void CKeyValuesParser::CBuffer::ResetPrecacheState()
{
	bufferOffset  = 0;
	precachedSize = 0;
	streamOffset  = 0;
}

/*
==================
CKeyValuesParser::CBuffer::IsEndOfBuffer
==================
*/
FORCEINLINE bool CKeyValuesParser::CBuffer::IsEndOfBuffer() const
{
	return pStreamReader->IsEndOfStream() && bufferOffset >= precachedSize;
}

/*
==================
CKeyValuesParser::CBuffer::Tell
==================
*/
FORCEINLINE uint64 CKeyValuesParser::CBuffer::Tell() const
{
	return streamOffset + bufferOffset;
}

/*
==================
CKeyValuesParser::CBuffer::GetStream
==================
*/
FORCEINLINE IStreamDataReader* CKeyValuesParser::CBuffer::GetStream() const
{
	return pStreamReader;
}
