#pragma once
#include <EASTL/vector.h>

#include "tier0/defines.h"
#include "tier0/types.h"
#include "tier0/memory.h"

//-----------------------------------------------------------------------------
// Command arguments
//-----------------------------------------------------------------------------
class CCmdArgs
{
public:
	CCmdArgs();
	CCmdArgs( const char* pArgs );
	CCmdArgs( uint32 argc, const char** argv );
	CCmdArgs( const CCmdArgs& other );

	// Takes a null terminated string and breaks the string up into arg tokens
	void TokenizeString( const char* pArgs );

	// The functions that execute commands get their parameters with these functions
	uint32 Argc() const;

	// Argv() will return an empty string, not NULL if arg >= Argc()
	const char* Argv( uint32 arg ) const;

	// Returns a single string containing argv(start) to argv(end)
	// bAscapeArgs is a fugly way to put the string back into a state ready to tokenize again
	eastl::string	   Args( uint32 start = INVALID_INDEX, uint32 end = INVALID_INDEX, bool bEscapeArgs = false ) const;
	void			   Args( eastl::string& dest, uint32 start = INVALID_INDEX, uint32 end = INVALID_INDEX, bool bEscapeArgs = false ) const;
	const char* const* Args( uint32& argc ) const;

	void AppendArg( const char* pArg );
	void Clear();
	void operator=( const CCmdArgs& args );

private:
	void BuildArgv();

	eastl::vector<char*> argv;		 // Points into tokenized string
	eastl::vector<char>	 tokenized;	 // Will have 0 bytes inserted
};

#include "tier1/cmdargs.inl"
