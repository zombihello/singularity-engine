#ifndef SENT_COMPONENT_INL
#define SENT_COMPONENT_INL

/*
==================
CSENTEntityDescComponent::Copy
==================
*/
FORCEINLINE void CSENTEntityDescComponent::Copy( const CSENTEntityDescComponent& other )
{
	type = other.type;
	vars = other.vars;
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
	vars.emplace_back( var );
}

/*
==================
CSENTEntityDescComponent::RemoveVar
==================
*/
FORCEINLINE void CSENTEntityDescComponent::RemoveVar( uint32 index )
{
	Assert( index < vars.size() );
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
CSENTEntityDescComponent::operator=
==================
*/
FORCEINLINE CSENTEntityDescComponent& CSENTEntityDescComponent::operator=( const CSENTEntityDescComponent& right )
{
	Copy( right );
	return *this;
}

#endif // !SENT_COMPONENT_INL
