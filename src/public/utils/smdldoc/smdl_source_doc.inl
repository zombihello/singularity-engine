#pragma once

/*
==================
CSMDLSourceModelDoc::SetAxisUp
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetAxisUp( axisUp_t axisUp )
{
	CSMDLSourceModelDoc::axisUp = axisUp;
}

/*
==================
CSMDLSourceModelDoc::GetAxisUp
==================
*/
FORCEINLINE axisUp_t CSMDLSourceModelDoc::GetAxisUp() const
{
	return axisUp;
}

/*
==================
CSMDLSourceModelDoc::SetSourcePath
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetSourcePath( const char* pSourcePath )
{
	CSMDLSourceModelDoc::srcPath = pSourcePath;
}

/*
==================
CSMDLSourceModelDoc::GetSourcePath
==================
*/
FORCEINLINE const eastl::string& CSMDLSourceModelDoc::GetSourcePath() const
{
	return srcPath;
}

/*
==================
CSMDLSourceModelDoc::SetMaterialsDir
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetMaterialsDir( const char* pMaterialsDir )
{
	CSMDLSourceModelDoc::materialsDir = pMaterialsDir;
}

/*
==================
CSMDLSourceModelDoc::GetMaterialsDir
==================
*/
FORCEINLINE const eastl::string& CSMDLSourceModelDoc::GetMaterialsDir() const
{
	return materialsDir;
}

/*
==================
CSMDLSourceModelDoc::AddRenamedMaterial
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::AddRenamedMaterial( const char* pOriginalName, const char* pNewName )
{
	renamedMaterialsDict[pOriginalName] = pNewName;
}

/*
==================
CSMDLSourceModelDoc::RemoveRenamedMaterial
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::RemoveRenamedMaterial( const char* pOriginalName )
{
	renamedMaterialsDict.erase( pOriginalName );
}

/*
==================
CSMDLSourceModelDoc::RemoveAllRenamedMaterials
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::RemoveAllRenamedMaterials()
{
	renamedMaterialsDict.clear();
}

/*
==================
CSMDLSourceModelDoc::GetRenamedMaterials
==================
*/
FORCEINLINE const eastl::unordered_map<eastl::string, eastl::string>& CSMDLSourceModelDoc::GetRenamedMaterials() const
{
	return renamedMaterialsDict;
}

/*
==================
CSMDLSourceModelDoc::SetCombineModels
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetCombineModels( bool bCombineModels )
{
	CSMDLSourceModelDoc::bCombineModels = bCombineModels;
}

/*
==================
CSMDLSourceModelDoc::IsCombineModels
==================
*/
FORCEINLINE bool CSMDLSourceModelDoc::IsCombineModels() const
{
	return bCombineModels;
}

/*
==================
CSMDLSourceModelDoc::Clear
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::Clear()
{
	bCombineModels = false;
	axisUp		   = AXIS_UP_DEFAULT;
	srcPath		   = "";
	materialsDir   = "materials/";
	renamedMaterialsDict.clear();
}
