#pragma once

/*
==================
CSENTSourceEntityDescDoc::SetOutputDir
==================
*/
FORCEINLINE void CSENTSourceEntityDescDoc::SetOutputDir( const char* pOutputDir )
{
	outputDir = pOutputDir;
}

/*
==================
CSENTSourceEntityDescDoc::AddComponent
==================
*/
FORCEINLINE void CSENTSourceEntityDescDoc::AddComponent( const CSENTEntityDescComponent& component )
{
	components.emplace_back( component );
}

/*
==================
CSENTSourceEntityDescDoc::RemoveComponent
==================
*/
FORCEINLINE void CSENTSourceEntityDescDoc::RemoveComponent( uint32 index )
{
	Assert( index < components.size() );
	components.erase( components.begin() + index );
}

/*
==================
CSENTSourceEntityDescDoc::Clear
==================
*/
FORCEINLINE void CSENTSourceEntityDescDoc::Clear()
{
	outputDir = "";
	components.clear();
}

/*
==================
CSENTSourceEntityDescDoc::GetOutputDir
==================
*/
FORCEINLINE const char* CSENTSourceEntityDescDoc::GetOutputDir() const
{
	return outputDir.c_str();
}

/*
==================
CSENTSourceEntityDescDoc::GetNumComponents
==================
*/
FORCEINLINE uint32 CSENTSourceEntityDescDoc::GetNumComponents() const
{
	return (uint32)components.size();
}

/*
==================
CSENTSourceEntityDescDoc::GetComponents
==================
*/
FORCEINLINE const std::vector<CSENTEntityDescComponent>& CSENTSourceEntityDescDoc::GetComponents() const
{
	return components;
}
