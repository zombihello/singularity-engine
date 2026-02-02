#pragma once
#include <EASTL/string.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/debug.h"

//-----------------------------------------------------------------------------
// String tools
//-----------------------------------------------------------------------------
enum
{
	COPY_ALL_CHARACTERS = -1  // Used in S_Strncat for copy all characters from source string
};

int32		   S_Vsnprintf( wchar_t* pDest, uint32 maxLen, const wchar_t* pFormat, va_list params );
int32		   S_Vsnprintf( char* pDest, uint32 maxLen, const char* pFormat, va_list params );
int32		   S_Vsscanf( const wchar_t* pString, const wchar_t* pFormat, va_list params );
int32		   S_Sscanf( const wchar_t* pString, const wchar_t* pFormat, ... );
int32		   S_Vsscanf( const char* pString, const char* pFormat, va_list params );
int32		   S_Sscanf( const char* pString, const char* pFormat, ... );
uint32		   S_Strlen( const char* pString );
uint32		   S_Strlen( const wchar_t* pString );
void		   S_Strcpy( char* pDest, const char* pSrc );
void		   S_Strcpy( wchar_t* pDest, const wchar_t* pSrc );
void		   S_Strncpy( char* pDest, const char* pSrc, uint32 maxLen );
void		   S_Strncpy( wchar_t* pDest, const wchar_t* pSrc, uint32 maxLen );
const char*	   S_Strchr( const char* pString, char c );
const wchar_t* S_Strchr( const wchar_t* pString, wchar_t c );
const char*	   S_Strstr( const char* pString, const char* pFind );
const wchar_t* S_Strstr( const wchar_t* pString, const wchar_t* pFind );
uint32		   S_Strcmp( const char* pString1, const char* pString2 );
uint32		   S_Strcmp( const wchar_t* pString1, const wchar_t* pString2 );
uint32		   S_Strncmp( const char* pString1, const char* pString2, uint32 count );
uint32		   S_Strncmp( const wchar_t* pString1, const wchar_t* pString2, uint32 count );
uint32		   S_Stricmp( const char* pString1, const char* pString2 );
uint32		   S_Stricmp( const wchar_t* pString1, const wchar_t* pString2 );
uint32		   S_Strnicmp( const char* pString1, const char* pString2, uint32 count );
uint32		   S_Strnicmp( const wchar_t* pString1, const wchar_t* pString2, uint32 count );
char*		   S_Strncat( char* pDest, const char* pSrc, uint32 destBufferSize, int32 maxCharsToCopy = COPY_ALL_CHARACTERS );
wchar_t*	   S_Strncat( wchar_t* pDest, const wchar_t* pSrc, uint32 destBufferSize, int32 maxCharsToCopy = COPY_ALL_CHARACTERS );
int32		   S_Atoi( const char* pString, char** pEndPtr = NULL );
int32		   S_Atoi( const wchar_t* pString, wchar_t** pEndPtr = NULL );
int64		   S_Atoi64( const char* pString, char** pEndPtr = NULL );
int64		   S_Atoi64( const wchar_t* pString, wchar_t** pEndPtr = NULL );
float		   S_Atof( const char* pString, char** pEndPtr = NULL );
float		   S_Atof( const wchar_t* pString, wchar_t** pEndPtr = NULL );
double		   S_Atod( const char* pString, char** pEndPtr = NULL );
double		   S_Atod( const wchar_t* pString, wchar_t** pEndPtr = NULL );
int32		   S_Snprintf( wchar_t* pDest, uint32 maxLen, const wchar_t* pFormat, ... );
int32		   S_Snprintf( char* pDest, uint32 maxLen, const char* pFormat, ... );
eastl::string  S_Vsprintf( const char* pFormat, va_list params );
eastl::string  S_Sprintf( const char* pFormat, ... );
eastl::wstring S_Vsprintf( const wchar_t* pFormat, va_list params );
eastl::wstring S_Sprintf( const wchar_t* pFormat, ... );
char		   S_ToUpper( char c );
char		   S_ToLower( char c );
wchar_t		   S_ToUpper( wchar_t c );
wchar_t		   S_ToLower( wchar_t c );
void		   S_Strupr( char* pString );
void		   S_Strupr( eastl::string& string );
void		   S_Strlwr( char* pString );
void		   S_Strlwr( eastl::string& string );
void		   S_Strupr( wchar_t* pString );
void		   S_Strupr( eastl::wstring& string );
void		   S_Strlwr( wchar_t* pString );
void		   S_Strlwr( eastl::wstring& string );
bool		   S_IsSpace( char c );
bool		   S_IsSpace( wchar_t c );
bool		   S_IsDigit( char c );
bool		   S_IsDigit( wchar_t c );

//-----------------------------------------------------------------------------
// UTF-8 tools
//-----------------------------------------------------------------------------
uint32 S_Utf8_CharSize( byte b );
uint32 S_Utf8_Strlen( const char* pString, uint32 size );

//-----------------------------------------------------------------------------
// Class that handles the ANSI to WCHAR conversion
//-----------------------------------------------------------------------------
class CANSIToWCHAR_Convert
{
public:
	// Converts the string to the desired format. Allocates memory if the
	// specified destination buffer isn't large enough
	wchar_t* Convert( const char* pSrcData, wchar_t* pDstData, uint32 size ) const;
	uint32	 GetLength( wchar_t* pData ) const;
};

//-----------------------------------------------------------------------------
// Class that handles the WCHAR to ANSI conversion
//-----------------------------------------------------------------------------
class CWCHARToANSI_Convert
{
public:
	// Converts the string to the desired format. Allocates memory if the
	// specified destination buffer isn't large enough
	char*  Convert( const wchar_t* pSrcData, char* pDstData, uint32 size ) const;
	uint32 GetLength( char* pData );
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

		   operator TConverTo*() const;
	uint32 GetLength() const;

private:
	TStringConversion()
		: pConvertedString( NULL ) {};

	TConverTo  buffer[defaultConversionSize];  // Holds the converted data if the size is large enough
	TConverTo* pConvertedString;			   // Points to the converted data. If this pointer doesn't match Buffer, then memory was allocated and needs to be freed
};

//-----------------------------------------------------------------------------
// Class converts bool to string
//-----------------------------------------------------------------------------
class CBoolToString
{
public:
	template<typename TCharType>
	static const TCharType* Convert( bool bValue );
};

//-----------------------------------------------------------------------------
// Class converts string to bool
//-----------------------------------------------------------------------------
class CStringToBool
{
public:
	template<typename TCharType>
	static bool Convert( const TCharType* pValue );
};

//-----------------------------------------------------------------------------
// Converter types and macroses
//-----------------------------------------------------------------------------
typedef TStringConversion<wchar_t, char, CANSIToWCHAR_Convert> ansiToWchar_t;
typedef TStringConversion<char, wchar_t, CWCHARToANSI_Convert> wcharToAnsi_t;

#define ANSI_TO_WCHAR( String ) (wchar_t*)ansiToWchar_t( (const char*)String )
#define WCHAR_TO_ANSI( String ) (char*)wcharToAnsi_t( (const wchar_t*)String )

#include "tier1/strtools.inl"
#if PLATFORM_WINDOWS
	#include "tier1/platforms/windows/win_strtools.inl"
#else
	#error Unknown platform
#endif	// PLATFORM_WINDOWS
