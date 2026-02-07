#pragma once

/*
==================
CKeyValuesParser::CKeyValuesParser
==================
*/
FORCEINLINE CKeyValuesParser::CKeyValuesParser()
	: pCurFile( nullptr )
	, pCurPtr( nullptr )
	, pEndPtr( nullptr )
	, scopeLevel( 0 )
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
CKeyValuesParser::EmitError
==================
*/
FORCEINLINE void CKeyValuesParser::EmitError( const char* pToken, const char* pFormat, ... )
{
	va_list params;
	uint64	line   = 0;
	uint64	column = 0;

	va_start( params, pFormat );
	GetTokenPostion( pToken, line, column );
	errorMsgs.emplace_back( S_Sprintf( "%s[%i:%i]: %s", pCurFile ? pCurFile : "<NO_FILE>", line, column, S_Vsprintf( pFormat, params ).c_str() ) );
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
CKeyValuesParser::IsBeginComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsBeginComment( const char* pPtr ) const
{
	return GetBufferSize( pPtr ) >= 2 && pPtr[0] == '/' && ( pPtr[1] == '/' || pPtr[1] == '*' );
}

/*
==================
CKeyValuesParser::IsEndComment
==================
*/
FORCEINLINE bool CKeyValuesParser::IsEndComment( const char* pPtr, bool bMultiLine /* = false */ ) const
{
	uint64 bufferSize = GetBufferSize( pPtr );
	if ( !bMultiLine )
	{
		return bufferSize > 0 && *pPtr == '\n';
	}
	return bufferSize >= 2 && pPtr[0] == '*' && pPtr[1] == '/';
}

/*
==================
CKeyValuesParser::IsControlSymbol
==================
*/
FORCEINLINE bool CKeyValuesParser::IsControlSymbol( const char* pPtr ) const
{
	return !IsEndOfBuffer( pPtr ) && ( IsBeginComment( pPtr ) || IsEndComment( pPtr, true ) || *pPtr == '{' || *pPtr == '}' || *pPtr == '[' || *pPtr == ']' || *pPtr == '"' || *pPtr == '|' );
}

/*
==================
CKeyValuesParser::IsEndOfBuffer
==================
*/
FORCEINLINE bool CKeyValuesParser::IsEndOfBuffer( const char* pPtr ) const
{
	return pPtr >= pEndPtr;
}

/*
==================
CKeyValuesParser::GetControlSymbolSize
==================
*/
FORCEINLINE uint32 CKeyValuesParser::GetControlSymbolSize( const char* pPtr ) const
{
	if ( IsControlSymbol( pPtr ) )
	{
		return ( IsBeginComment( pPtr ) || IsEndComment( pPtr, true ) ) ? 2 : 1;
	}
	return 0;
}

/*
==================
CKeyValuesParser::GetBufferSize
==================
*/
FORCEINLINE uint64 CKeyValuesParser::GetBufferSize( const char* pPtr ) const
{
	return (uint64)( pEndPtr - pPtr );
}
