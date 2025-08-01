#pragma once

/*
==================
parserToken_t::operator=
==================
*/
FORCEINLINE parserToken_t& parserToken_t::operator=( const parserToken_t& other )
{
	tokenString		= other.tokenString;
	tokenID			= other.tokenID;
	scopeLevel		= other.scopeLevel;
	context			= other.context;
	return *this;
}

/*
==================
parserToken_t::ToString
==================
*/
FORCEINLINE std::string parserToken_t::ToString() const
{
	return S_Sprintf( "Token %i \"%hs\", #%i at %hs", tokenID, !tokenString.empty() ? tokenString.data() : "<TEXT_EMPTY>", scopeLevel, context.ToString().c_str() );
}
