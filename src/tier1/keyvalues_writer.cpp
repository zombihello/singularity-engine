#include "pch_tier1.h"
#include "tier1/keyvalues_writer.h"

/*
==================
CKeyValuesWriter::Write
==================
*/
void CKeyValuesWriter::Write( CKeyValues* pKeyValues, eastl::string& buffer ) const
{
	PROFILE_SCOPE();
	Assert( pKeyValues );

	// If the key values has subkeys save only they
	if ( pKeyValues->HasSubKeys() )
	{
		Assert( !pKeyValues->HasData() );
		CKeyValues*						pPrevSubKey = NULL;
		const eastl::list<CKeyValues*>& subKeys		= pKeyValues->GetSubKeys();
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			CKeyValues* pSubKey = *it;
			if ( pPrevSubKey && pPrevSubKey->HasSubKeys() )
			{
				buffer += LINE_TERMINATOR;
			}
			WriteKeyToBuffer( pSubKey, buffer, 0 );
			pPrevSubKey = pSubKey;
		}
		return;
	}

	// Otherwise save ourselves into the buffer
	WriteKeyToBuffer( pKeyValues, buffer, 0 );
}

/*
==================
CKeyValuesWriter::WriteKeyToBuffer
==================
*/
void CKeyValuesWriter::WriteKeyToBuffer( CKeyValues* pKeyValues, eastl::string& buffer, uint32 indentLevel ) const
{
	PROFILE_SCOPE();
	Assert( pKeyValues );

	// Write name, and write empty block if the key hasn't neither subKeys nor a data
	WriteIndents( buffer, indentLevel );
	buffer += S_Sprintf( "\"%s\"", pKeyValues->GetName() );
	if ( pKeyValues->IsEmpty() )
	{
		buffer += " {}" LINE_TERMINATOR_STRING;
		return;
	}

	// Write subKeys
	if ( pKeyValues->HasSubKeys() )
	{
		Assert( !pKeyValues->HasData() );
		buffer += LINE_TERMINATOR;
		WriteIndents( buffer, indentLevel );
		buffer += "{" LINE_TERMINATOR_STRING;

		CKeyValues*						pPrevSubKey = NULL;
		const eastl::list<CKeyValues*>& subKeys		= pKeyValues->GetSubKeys();
		for ( auto it = subKeys.begin(), itEnd = subKeys.end(); it != itEnd; ++it )
		{
			CKeyValues* pSubKey = *it;
			if ( pPrevSubKey && pPrevSubKey->HasSubKeys() )
			{
				buffer += LINE_TERMINATOR;
			}
			WriteKeyToBuffer( pSubKey, buffer, indentLevel + 1 );
			pPrevSubKey = pSubKey;
		}

		WriteIndents( buffer, indentLevel );
		buffer += "}" LINE_TERMINATOR_STRING;
	}

	// Write a data
	if ( pKeyValues->HasData() )
	{
		Assert( !pKeyValues->HasSubKeys() );
		buffer += " \"";
		WriteConvertedString( buffer, pKeyValues->GetString() );
		buffer += "\"" LINE_TERMINATOR_STRING;
	}
}

/*
==================
CKeyValuesWriter::WriteConvertedString
==================
*/
void CKeyValuesWriter::WriteConvertedString( eastl::string& buffer, const char* pValue ) const
{
	// Handle double quote chars within the string
	// The worst possible case is that the whole string is quotes
	uint32 length			= S_Strlen( pValue );
	char*  pConvertedString = (char*)Mem_Alloca( ( length + 1 ) * sizeof( char ) * 2 );
	uint32 destIndex		= 0;
	for ( uint32 srcIndex = 0; srcIndex <= length; ++srcIndex )
	{
		switch ( pValue[srcIndex] )
		{
		case '\n':
			pConvertedString[destIndex]		= '\\';
			pConvertedString[destIndex + 1] = 'n';
			destIndex += 2;
			break;

		case '\r':
			pConvertedString[destIndex]		= '\\';
			pConvertedString[destIndex + 1] = 'r';
			destIndex += 2;
			break;

		case '\t':
			pConvertedString[destIndex]		= '\\';
			pConvertedString[destIndex + 1] = 't';
			destIndex += 2;
			break;

		case '\"':
			pConvertedString[destIndex]		= '\\';
			pConvertedString[destIndex + 1] = '\"';
			destIndex += 2;
			break;

		default:
			pConvertedString[destIndex] = pValue[srcIndex];
			++destIndex;
			break;
		}
	}

	buffer += pConvertedString;
}
