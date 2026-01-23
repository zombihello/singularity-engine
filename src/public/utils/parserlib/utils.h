#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Functions to get chars number in a UTF-8 string
//-----------------------------------------------------------------------------
uint32 Parserlib_GetNumUtf8Bytes( char value );
uint32 Parserlib_GetNumChars( const char* pString, uint32 size );

#include "utils/parserlib/utils.inl"
