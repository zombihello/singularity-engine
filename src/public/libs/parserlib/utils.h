#ifndef UTILS_H
#define UTILS_H

#include "core/types.h"
#include "core/platform.h"

//-----------------------------------------------------------------------------
// Functions to get chars number in a UTF-8 string
//-----------------------------------------------------------------------------
uint32 Parserlib_GetNumUtf8Bytes( achar value );
uint32 Parserlib_GetNumChars( const achar* pString, uint32 size );

#include "parserlib/utils.inl"

#endif // !UTILS_H