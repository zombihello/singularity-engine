#pragma once
#include "tier1/defines.h"
#include "tier1/types.h"

//-----------------------------------------------------------------------------
// Functions to get chars number in a UTF-8 string
//-----------------------------------------------------------------------------
uint32 Parserlib_GetNumUtf8Bytes( char value );
uint32 Parserlib_GetNumChars( const char* pString, uint32 size );

#include "utils/parserlib/utils.inl"
