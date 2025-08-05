#pragma once

/*
==================
CSENTEntityDescComponent::Copy
==================
*/
FORCEINLINE void CSENTEntityDescComponent::Copy( const CSENTEntityDescComponent& other )
{
	varsDict.clear();
	type = other.type;
	vars = other.vars;
	for ( uint32 varIdx = 0, numVars = ( uint32 )vars.size(); varIdx < numVars; ++varIdx )
	{
		const CSENTEntityDescVar&	sentVar = vars[varIdx];
		varsDict[sentVar.GetName()]	= varIdx;
	}
}

/*
==================
CSENTEntityDescComponent::SetType
==================
*/
FORCEINLINE void CSENTEntityDescComponent::SetType( const achar* pType )
{
	type = pType;
}

/*
==================
CSENTEntityDescComponent::AddVar
==================
*/
FORCEINLINE void CSENTEntityDescComponent::AddVar( const CSENTEntityDescVar& var )
{
	uint32						varIdx	= ( uint32 )vars.size();
	const CSENTEntityDescVar&	newVar	= vars.emplace_back( var );
	varsDict[newVar.GetName()]	= varIdx;
}

/*
==================
CSENTEntityDescComponent::RemoveVar
==================
*/
FORCEINLINE void CSENTEntityDescComponent::RemoveVar( uint32 index )
{
	Assert( index < vars.size() );
	varsDict.erase( vars[index].GetName() );
	vars.erase( vars.begin() + index );
}

/*
==================
CSENTEntityDescComponent::Clear
==================
*/
FORCEINLINE void CSENTEntityDescComponent::Clear()
{
	type = "";
	vars.clear();
	varsDict.clear();
}

/*
==================
CSENTEntityDescComponent::GetType
==================
*/
FORCEINLINE const achar* CSENTEntityDescComponent::GetType() const
{
	return type.c_str();
}

/*
==================
CSENTEntityDescComponent::GetNumVars
==================
*/
FORCEINLINE uint32 CSENTEntityDescComponent::GetNumVars() const
{
	return ( uint32 )vars.size();
}

/*
==================
CSENTEntityDescComponent::GetVars
==================
*/
FORCEINLINE const std::vector<CSENTEntityDescVar>& CSENTEntityDescComponent::GetVars() const
{
	return vars;
}

/*
==================
CSENTEntityDescComponent::GetVar
==================
*/
FORCEINLINE const CSENTEntityDescVar& CSENTEntityDescComponent::GetVar( uint32 index ) const
{
	Assert( index < vars.size() );
	return vars[index];
}

/*
==================
CSENTEntityDescComponent::GetVar
==================
*/
FORCEINLINE CSENTEntityDescVar* CSENTEntityDescComponent::GetVar( const achar* pName ) const
{
	auto	itFind = varsDict.find( pName );
	if ( itFind == varsDict.end() )
	{
		return NULL;
	}
	return ( CSENTEntityDescVar* )&vars[itFind->second];
}

/*
==================
CSENTEntityDescComponent::operator=
==================
*/
FORCEINLINE CSENTEntityDescComponent& CSENTEntityDescComponent::operator=( const CSENTEntityDescComponent& right )
{
	Copy( right );
	return *this;
}
