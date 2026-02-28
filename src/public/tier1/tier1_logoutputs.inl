#pragma once

/*
==================
CLogOutputFile::BeginLoggingToFile
==================
*/
FORCEINLINE TRefPtr<IStreamDataWriter> CLogOutputFile::BeginLoggingToFile( const char* pPath )
{
	EndLoggingToFile();
	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "Tier1: Failed to open log file '%s'", pPath );
	}
	return pFile;
}

/*
==================
CLogOutputFile::EndLoggingToFile
==================
*/
FORCEINLINE void CLogOutputFile::EndLoggingToFile()
{
	pFile = NULL;
}

/*
==================
CLogOutputFile::GetFile
==================
*/
FORCEINLINE TRefPtr<IStreamDataWriter> CLogOutputFile::GetFile() const
{
	return pFile;
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
