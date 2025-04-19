#ifndef SMDL_SOURCE_DOC_INL
#define SMDL_SOURCE_DOC_INL

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
FORCEINLINE void CSMDLSourceModelDoc::SetSourcePath( const achar* pSourcePath )
{
	CSMDLSourceModelDoc::sourcePath = pSourcePath;
}

/*
==================
CSMDLSourceModelDoc::GetSourcePath
==================
*/
FORCEINLINE const std::string& CSMDLSourceModelDoc::GetSourcePath() const
{
	return sourcePath;
}

/*
==================
CSMDLSourceModelDoc::SetMaterialsDir
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetMaterialsDir( const achar* pMaterialsDir )
{
	CSMDLSourceModelDoc::materialsDir = pMaterialsDir;
}

/*
==================
CSMDLSourceModelDoc::GetMaterialsDir
==================
*/
FORCEINLINE const std::string& CSMDLSourceModelDoc::GetMaterialsDir() const
{
	return materialsDir;
}

/*
==================
CSMDLSourceModelDoc::SetOutputDir
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::SetOutputDir( const achar* pOutputDir )
{
	CSMDLSourceModelDoc::outputDir = pOutputDir;
}

/*
==================
CSMDLSourceModelDoc::GetOutputDir
==================
*/
FORCEINLINE const std::string& CSMDLSourceModelDoc::GetOutputDir() const
{
	return outputDir;
}

/*
==================
CSMDLSourceModelDoc::AddRenamedMaterial
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::AddRenamedMaterial( const achar* pOriginalName, const achar* pNewName )
{
	renamedMaterialsDict[pOriginalName] = pNewName;
}

/*
==================
CSMDLSourceModelDoc::RemoveRenamedMaterial
==================
*/
FORCEINLINE void CSMDLSourceModelDoc::RemoveRenamedMaterial( const achar* pOriginalName )
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
FORCEINLINE const std::unordered_map<std::string, std::string>& CSMDLSourceModelDoc::GetRenamedMaterials() const
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
FORCEINLINE bool CSMDLSourceModelDoc::IsCombineModels()  const
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
	bCombineModels	= false;
	axisUp			= AXIS_UP_DEFAULT;
	sourcePath		= "";
	materialsDir	= "materials/";
	outputDir		= "";
	renamedMaterialsDict.clear();
}

#endif // !SMDL_SOURCE_DOC_INL
