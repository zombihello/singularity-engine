#pragma once
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include "tier1/keyvalues.h"
#include "tier1/istreamdata.h"

//-----------------------------------------------------------------------------
// Key values parser
//-----------------------------------------------------------------------------
class CKeyValuesParser
{
public:
	CKeyValuesParser();
	void								Parse( CKeyValues* pKeyValues, IStreamDataReader* pStreamReader );
	bool								HasErrors() const;
	const eastl::vector<eastl::string>& GetErrorMsgs() const;

private:
	enum tokenType_t
	{
		TOKEN_TYPE_BARE,
		TOKEN_TYPE_QUOTED,
		TOKEN_TYPE_CONTROL
	};

	struct token_t
	{
		tokenType_t	  type;
		uint64		  position;
		eastl::string string;
	};

	class CBuffer
	{
	public:
		CBuffer();
		~CBuffer();

		void SetStream( IStreamDataReader* pStreamReader );

		char Peek( uint32 offset = 0 );
		void Advance( uint32 offset );
		void Seek( uint64 position );

		bool			   IsEndOfBuffer() const;
		uint64			   Tell() const;
		IStreamDataReader* GetStream() const;

	private:
		enum
		{
			BUFFER_SIZE = 1024
		};

		void ResetPrecacheState();
		void Precache( uint32 requestedSize );

		uint32					   precachedSize;
		uint32					   bufferOffset;
		uint64					   streamOffset;
		char*					   pBuffer;
		CRefPtr<IStreamDataReader> pStreamReader;
	};

	bool	ReadKeyValues( CKeyValues* pKeyValues );
	bool	ReadConditionalBlock( bool& bAccepted );
	bool	ReadIncludeKeys( eastl::vector<CKeyValues*>& includedKeys );
	void	SkipSpacesAndComments();
	token_t ReadToken();
	void	ReadToken( token_t& token );

	void Setup( IStreamDataReader* pStreamReader );
	void EmitError( uint64 streamOffset, const char* pFormat, ... );
	void AppendIncludedKeys( CKeyValues* pKeyValues, CKeyValues* pIncludedKeyValues );
	void MergeBaseKeys( CKeyValues* pNewKeyValues, CKeyValues* pBaseKeyValues );  // Merge in another CKeyValues, keeping "our" settings

	bool   IsBeginLineComment( uint32 offset = 0 ) const;
	bool   IsBeginMultilineComment( uint32 offset = 0 ) const;
	bool   IsEndLineComment( uint32 offset = 0 ) const;
	bool   IsEndMultilineComment( uint32 offset = 0 ) const;
	bool   IsControlSymbol( uint32 offset = 0 ) const;
	uint32 GetControlSymbolSize( uint32 offset = 0 ) const;
	void   GetPostionInCode( uint64 streamOffset, uint64& line, uint64& column ) const;

	mutable CBuffer							   buffer;
	uint32									   scopeLevel;
	eastl::vector<eastl::string>			   errorMsgs;
	eastl::vector<eastl::pair<uint64, uint64>> lineRanges;
};

#include "tier1/keyvalues_parser.inl"
