#pragma once

/*
==================
CSMATSourceMaterialDoc::SetShaderName
==================
*/
FORCEINLINE void CSMATSourceMaterialDoc::SetShaderName( const achar* pShaderName )
{
	shaderName = pShaderName;
}

/*
==================
CSMATSourceMaterialDoc::AddVar
==================
*/
FORCEINLINE void CSMATSourceMaterialDoc::AddVar( const CSMATMaterialVar& var )
{
	vars.emplace_back( var );
}

/*
==================
CSMATSourceMaterialDoc::RemoveVar
==================
*/
FORCEINLINE void CSMATSourceMaterialDoc::RemoveVar( uint32 index )
{
	Assert( index < vars.size() );
	vars.erase( vars.begin() + index );
}

/*
==================
CSMATSourceMaterialDoc::Clear
==================
*/
FORCEINLINE void CSMATSourceMaterialDoc::Clear()
{
	outputDir = "";
	shaderName = "";
	vars.clear();
}

/*
==================
CSMATSourceMaterialDoc::GetNumVars
==================
*/
FORCEINLINE uint32 CSMATSourceMaterialDoc::GetNumVars() const
{
	return ( uint32 )vars.size();
}

/*
==================
CSMATSourceMaterialDoc::GetVars
==================
*/
FORCEINLINE const std::vector<CSMATMaterialVar>& CSMATSourceMaterialDoc::GetVars() const
{
	return vars;
}

/*
==================
CSMATSourceMaterialDoc::GetShaderName
==================
*/
FORCEINLINE const achar* CSMATSourceMaterialDoc::GetShaderName() const
{
	return shaderName.c_str();
}

/*
==================
CSMATSourceMaterialDoc::SetOutputDir
==================
*/
FORCEINLINE void CSMATSourceMaterialDoc::SetOutputDir( const achar* pOutputDir )
{
	outputDir = pOutputDir;
}

/*
==================
CSMATSourceMaterialDoc::GetOutputDir
==================
*/
FORCEINLINE const achar* CSMATSourceMaterialDoc::GetOutputDir() const
{
	return outputDir.c_str();
}
