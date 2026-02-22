#pragma once

/*
==================
CLogOutputFile::CLogOutputFile
==================
*/
FORCEINLINE CLogOutputFile::CLogOutputFile( const char* pPath )
{
	Assert( g_pFileSystem );
	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "Tier1: Failed to open log file '%s'", pPath );
	}
}

/*
==================
CLogOutputFile::Print
==================
*/
FORCEINLINE void CLogOutputFile::Print( const logContext_t& context, const char* pMessage )
{
	if ( pFile )
	{
		pFile->Write( (void*)pMessage, S_Strlen( pMessage ) * sizeof( char ) );
	}
}
