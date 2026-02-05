#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier1/keyvalues.h"

//-----------------------------------------------------------------------------
// Key values parser
//-----------------------------------------------------------------------------
class CKeyValuesParser
{
public:
	CKeyValuesParser();
	void								Parse( const char* pFile, CKeyValues* pRootKeyValue, const char* pBuffer, uint64 size );
	bool								HasErrors() const;
	const eastl::vector<eastl::string>& GetErrorMsgs() const;

private:
	bool			   ReadKeyValues( CKeyValues* pKeyValue );
	bool			   ReadConditionalBlock( bool& bAccepted );
	void			   SkipSpacesAndComments();
	eastl::string_view ReadToken( bool& bQuoted );

	void Setup( const char* pFile, const char* pBuffer, uint64 size );
	void EmitError( const char* pToken, const char* pFormat, ... );

	bool   IsBeginComment( const char* pPtr ) const;
	bool   IsEndComment( const char* pPtr, bool bMultiLine = false ) const;
	bool   IsControlSymbol( const char* pPtr ) const;
	bool   IsEndOfBuffer( const char* pPtr ) const;
	void   GetTokenPostion( const char* pToken, uint64& line, uint64& column ) const;
	uint32 GetControlSymbolSize( const char* pPtr ) const;
	uint64 GetBufferSize( const char* pPtr ) const;

	const char*						  pCurFile;
	const char*						  pCurPtr;
	const char*						  pEndPtr;
	uint32							  scopeLevel;
	eastl::vector<eastl::string>	  errorMsgs;
	eastl::vector<eastl::string_view> lineRanges;
};

#include "tier1/keyvalues_parser.inl"
