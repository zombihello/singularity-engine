#pragma once

/*
==================
CSMATCompiledMaterialDoc::SetShaderName
==================
*/
FORCEINLINE void CSMATCompiledMaterialDoc::SetShaderName( const char* pShaderName )
{
	shaderName = pShaderName;
}

/*
==================
CSMATCompiledMaterialDoc::AddVar
==================
*/
FORCEINLINE void CSMATCompiledMaterialDoc::AddVar( const CSMATMaterialVar& var )
{
	vars.emplace_back( var );
}

/*
==================
CSMATCompiledMaterialDoc::RemoveVar
==================
*/
FORCEINLINE void CSMATCompiledMaterialDoc::RemoveVar( uint32 index )
{
	Assert( index < vars.size() );
	vars.erase( vars.begin() + index );
}

/*
==================
CSMATCompiledMaterialDoc::Clear
==================
*/
FORCEINLINE void CSMATCompiledMaterialDoc::Clear()
{
	shaderName = "";
	vars.clear();
}

/*
==================
CSMATCompiledMaterialDoc::GetNumVars
==================
*/
FORCEINLINE uint32 CSMATCompiledMaterialDoc::GetNumVars() const
{
	return (uint32)vars.size();
}

/*
==================
CSMATCompiledMaterialDoc::GetVars
==================
*/
FORCEINLINE const std::vector<CSMATMaterialVar>& CSMATCompiledMaterialDoc::GetVars() const
{
	return vars;
}

/*
==================
CSMATCompiledMaterialDoc::GetShaderName
==================
*/
FORCEINLINE const char* CSMATCompiledMaterialDoc::GetShaderName() const
{
	return shaderName.c_str();
}
