#pragma once
#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Functions to get chars number in a UTF-8 string
//-----------------------------------------------------------------------------
uint32 Parserlib_GetNumUtf8Bytes( achar value );
uint32 Parserlib_GetNumChars( const achar* pString, uint32 size );

#include "parserlib/utils.inl"