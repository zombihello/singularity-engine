#pragma once
#include <string>

#include "stdlib/defines.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// String tools
//-----------------------------------------------------------------------------
enum
{
	COPY_ALL_CHARACTERS = -1		// Used in S_Strncat for copy all characters from source string
};


int32 S_Vsnprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, va_list params );
int32 S_Vsnprintf( achar* pDest, uint32 maxLen, const achar* pFormat, va_list params );
int32 S_Vsscanf( const wchar* pString, const wchar* pFormat, va_list params );
int32 S_Sscanf( const wchar* pString, const wchar* pFormat, ... );
int32 S_Vsscanf( const achar* pString, const achar* pFormat, va_list params );
int32 S_Sscanf( const achar* pString, const achar* pFormat, ... );
uint32 S_Strlen( const achar* pString );
uint32 S_Strlen( const wchar* pString );
void S_Strcpy( achar* pDest, const achar* pSrc );
void S_Strcpy( wchar* pDest, const wchar* pSrc );
void S_Strncpy( achar* pDest, const achar* pSrc, uint32 maxLen );
void S_Strncpy( wchar* pDest, const wchar* pSrc, uint32 maxLen );
achar* S_Strstr( const achar* pString, const achar* pFind );
wchar* S_Strstr( const wchar* pString, const wchar* pFind );
uint32 S_Strcmp( const achar* pString1, const achar* pString2 );
uint32 S_Strcmp( const wchar* pString1, const wchar* pString2 );
uint32 S_Strncmp( const achar* pString1, const achar* pString2, uint32 count );
uint32 S_Strncmp( const wchar* pString1, const wchar* pString2, uint32 count );
uint32 S_Stricmp( const achar* pString1, const achar* pString2 );
uint32 S_Stricmp( const wchar* pString1, const wchar* pString2 );
uint32 S_Strnicmp( const achar* pString1, const achar* pString2, uint32 count );
uint32 S_Strnicmp( const wchar* pString1, const wchar* pString2, uint32 count );
achar* S_Strncat( achar* pDest, const achar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy = COPY_ALL_CHARACTERS );
wchar* S_Strncat( wchar* pDest, const wchar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy = COPY_ALL_CHARACTERS );
int32 S_Atoi( const achar* pString );
int32 S_Atoi( const wchar* pString );
float S_Atof( const achar* pString );
float S_Atof( const wchar* pString );
int32 S_Snprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, ... );
int32 S_Snprintf( achar* pDest, uint32 maxLen, const achar* pFormat, ... );
std::string S_Vsprintf( const achar* pFormat, va_list params );
std::string S_Sprintf( const achar* pFormat, ... );
std::wstring S_Vsprintf( const wchar* pFormat, va_list params );
std::wstring S_Sprintf( const wchar* pFormat, ... );
int32 S_ToUpper( int32 ch );
int32 S_ToLower( int32 ch );
achar* S_Strupr( achar* pString );
achar* S_Strlwr( achar* pString );
wchar* S_Strupr( wchar* pString );
wchar* S_Strlwr( wchar* pString );
bool S_IsSpace( achar c );
bool S_IsSpace( wchar c );


//-----------------------------------------------------------------------------
// Class that handles the ANSI to WCHAR conversion
//-----------------------------------------------------------------------------
class CANSIToWCHAR_Convert
{
public:
	// Converts the string to the desired format. Allocates memory if the
	// specified destination buffer isn't large enough
	wchar* Convert( const achar* pSrcData, wchar* pDstData, uint32 size ) const;
	uint32 GetLength( wchar* pData ) const;
};


//-----------------------------------------------------------------------------
// Class that handles the WCHAR to ANSI conversion
//-----------------------------------------------------------------------------
class CWCHARToANSI_Convert
{
public:
	// Converts the string to the desired format. Allocates memory if the
	// specified destination buffer isn't large enough
	achar* Convert( const wchar* pSrcData, achar* pDstData, uint32 size ) const;
	uint32 GetLength( achar* pData );
};


//-----------------------------------------------------------------------------
// Class takes one type of string and converts it to another
//-----------------------------------------------------------------------------
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize = 128>
class TStringConversion : public TBaseConverter
{
public:
	// Converts the data by using the Convert() method on the base class
	explicit TStringConversion( const TConvertFrom* pSrcData )
	{
		if ( pSrcData )
		{
			// Use base class convert method
			pConvertedString = TBaseConverter::Convert( pSrcData, buffer, defaultConversionSize );
		}
		else
		{
			pConvertedString = nullptr;
		}
	}
	~TStringConversion()
	{
		if ( !pConvertedString )
		{
			return;
		}

		// Make the string empty so people don't hold onto pointers
		*pConvertedString = 0;

		// Free the memory if it was allocated in order to do the conversion
		if ( pConvertedString != buffer )
		{
			delete[] pConvertedString;
		}
	}

	operator TConverTo* () const;
	uint32 GetLength() const;

private:
	TStringConversion() 
		: pConvertedString( NULL )
	{};

	TConverTo	buffer[defaultConversionSize];	// Holds the converted data if the size is large enough
	TConverTo*	pConvertedString;				// Points to the converted data. If this pointer doesn't match Buffer, then memory was allocated and needs to be freed
};


//-----------------------------------------------------------------------------
// Converter types and macroses
//-----------------------------------------------------------------------------
typedef TStringConversion<wchar, achar, CANSIToWCHAR_Convert>		ansiToWchar_t;
typedef TStringConversion<achar, wchar, CWCHARToANSI_Convert>		wcharToAnsi_t;

#define ANSI_TO_WCHAR( String )										( wchar* )ansiToWchar_t( ( const achar* )String )
#define WCHAR_TO_ANSI( String )										( achar* )wcharToAnsi_t( ( const wchar* )String )

#include "stdlib/strtools.inl"
#if PLATFORM_WINDOWS
	#include "stdlib/platforms/windows/win_strtools.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS