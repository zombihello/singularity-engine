#include "pch_core.h"
#include <vector>
#include <unordered_map>

#include "core/debug.h"
#include "core/icommandline.h"

class CCommandLine : public ICommandLine
{
public:
	virtual ~CCommandLine();

	// ICommandLine interface
	virtual void Init( const char* pCommandLine ) override;
	virtual void Shutdown() override;

	virtual bool		  HasParam( const char* pParam ) const override;
	virtual bool		  HasParam( const char* pParam, const char* pValue ) const override;
	virtual const char*  GetFirstValue( const char* pParam ) const override;
	virtual const char** GetValues( const char* pParam, uint32& size ) const override;

private:
	typedef std::vector<const char*>				  values_t;
	typedef std::unordered_map<std::string, values_t> paramDict_t;

	void Parse( const char* pCommandLine );

	paramDict_t paramsDict;
};

/*
==================
CommandLine
==================
*/
ICommandLine* CommandLine()
{
	static CCommandLine s_CommandLine;
	return &s_CommandLine;
}

/*
==================
ParseToken
==================
*/
static bool ParseToken( const char*& pStr, std::string& result, bool bUseEscape )
{
	//
	// Grab the next space-delimited string from the input stream.
	// If quoted, gets entire quoted string.
	//
	result.clear();

	// Skip preceeding spaces and tabs.
	while ( S_IsSpace( *pStr ) || *pStr == '=' )
	{
		pStr++;
	}

	if ( *pStr == '"' )
	{
		// Get quoted string
		pStr++;

		while ( *pStr && *pStr != '"' )
		{
			char c = *pStr++;
			if ( c == '\\' && bUseEscape )
			{
				// Get escape
				c = *pStr++;
				if ( !c )
				{
					break;
				}
			}

			result += c;
		}

		if ( *pStr == '"' )
		{
			pStr++;
		}
	}
	else
	{
		// Get unquoted string (that might contain a quoted part, which will be left intact).
		// For example, -ARG="foo bar baz", will be treated as one token, with quotes intact
		bool bInQuote = false;

		while ( true )
		{
			char character = *pStr;
			if ( character == 0 || ( ( S_IsSpace( character ) || character == '=' ) && !bInQuote ) )
			{
				break;
			}
			pStr++;

			// Preserve escapes if they're in a quoted string (the check for " is in the else to let \" work as expected)
			if ( character == '\\' && bUseEscape && bInQuote )
			{
				result += character;

				character = *pStr;
				if ( !character )
				{
					break;
				}
				pStr++;
			}
			else if ( character == '"' )
			{
				bInQuote = !bInQuote;
			}

			result += character;
		}
	}

	return !result.empty();
}

/*
==================
CCommandLine::~CCommandLine
==================
*/
CCommandLine::~CCommandLine()
{
	Shutdown();
}

/*
==================
CCommandLine::Init
==================
*/
void CCommandLine::Init( const char* pCommandLine )
{
	Shutdown();
	Parse( pCommandLine );
	Msg( "Core: Command line: '%s'", pCommandLine );
}

/*
==================
CCommandLine::Shutdown
==================
*/
void CCommandLine::Shutdown()
{
	// Free memory
	for ( paramDict_t::iterator it = paramsDict.begin(), itEnd = paramsDict.end(); it != itEnd; ++it )
	{
		const values_t& valueArray = it->second;
		for ( uint32 index = 0, count = (uint32)valueArray.size(); index < count; ++index )
		{
			free( (char*)valueArray[index] );
		}
	}
	paramsDict.clear();
}

/*
==================
CCommandLine::Parse
==================
*/
void CCommandLine::Parse( const char* pCommandLine )
{
	std::string			  nextToken;
	paramDict_t::iterator itCurrentParam = paramsDict.end();

	while ( ParseToken( pCommandLine, nextToken, false ) )
	{
		S_Strlwr( nextToken.data() );
		if ( nextToken[0] == '-' || nextToken[0] == '/' )
		{
			std::string token( &nextToken[1], nextToken.size() - 1 );
			itCurrentParam = paramsDict.find( token );
			if ( itCurrentParam == paramsDict.end() )
			{
				itCurrentParam = paramsDict.insert( std::make_pair( token, values_t() ) ).first;
			}
		}
		else if ( itCurrentParam != paramsDict.end() )
		{
			uint32 size	 = (uint32)nextToken.size();
			char* pData = (char*)Mem_Malloc( ( size + 1 ) * sizeof( char ) );
			S_Strcpy( pData, nextToken.data() );
			pData[size] = '\0';
			itCurrentParam->second.push_back( pData );
		}
	}
}

/*
==================
CCommandLine::HasParam
==================
*/
bool CCommandLine::HasParam( const char* pParam ) const
{
	std::string param = pParam;
	S_Strlwr( param.data() );
	return paramsDict.find( param ) != paramsDict.end();
}

/*
==================
CCommandLine::HasParam
==================
*/
bool CCommandLine::HasParam( const char* pParam, const char* pValue ) const
{
	std::string param = pParam;
	S_Strlwr( param.data() );
	paramDict_t::const_iterator itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() )
	{
		return false;
	}

	std::string value = pValue;
	S_Strlwr( value.data() );
	bool			bResult	   = false;
	const values_t& valueArray = itParam->second;
	for ( uint32 index = 0, count = (uint32)valueArray.size(); index < count; ++index )
	{
		if ( !S_Strcmp( valueArray[index], value.c_str() ) )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/*
==================
CCommandLine::GetFirstValue
==================
*/
const char* CCommandLine::GetFirstValue( const char* pParam ) const
{
	std::string param = pParam;
	S_Strlwr( param.data() );
	paramDict_t::const_iterator itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() || itParam->second.empty() )
	{
		return "";
	}

	return itParam->second[0];
}

/*
==================
CCommandLine::GetValues
==================
*/
const char** CCommandLine::GetValues( const char* pParam, uint32& size ) const
{
	std::string param = pParam;
	S_Strlwr( param.data() );
	paramDict_t::const_iterator itParam = paramsDict.find( param );
	if ( itParam == paramsDict.end() )
	{
		size = 0;
		return nullptr;
	}

	size = (uint32)itParam->second.size();
	return (const char**)itParam->second.data();
}
