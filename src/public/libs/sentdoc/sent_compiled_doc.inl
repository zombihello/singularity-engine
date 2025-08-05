#pragma once

/*
==================
CSENTCompiledEntityDescDoc::AddComponent
==================
*/
FORCEINLINE void CSENTCompiledEntityDescDoc::AddComponent( const CSENTEntityDescComponent& component )
{
	components.emplace_back( component );
}

/*
==================
CSENTCompiledEntityDescDoc::RemoveComponent
==================
*/
FORCEINLINE void CSENTCompiledEntityDescDoc::RemoveComponent( uint32 index )
{
	Assert( index < components.size() );
	components.erase( components.begin() + index );
}

/*
==================
CSENTCompiledEntityDescDoc::Clear
==================
*/
FORCEINLINE void CSENTCompiledEntityDescDoc::Clear()
{
	components.clear();
}

/*
==================
CSENTCompiledEntityDescDoc::GetNumComponents
==================
*/
FORCEINLINE uint32 CSENTCompiledEntityDescDoc::GetNumComponents() const
{
	return ( uint32 )components.size();
}

/*
==================
CSENTCompiledEntityDescDoc::GetComponents
==================
*/
FORCEINLINE const std::vector<CSENTEntityDescComponent>& CSENTCompiledEntityDescDoc::GetComponents() const
{
	return components;
}
