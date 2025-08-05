#pragma once
#include "core/core.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMDL (Singularity Model) files
//-----------------------------------------------------------------------------
class CSMDLSourceModelDoc
{
public:
	CSMDLSourceModelDoc();

	// Load and save a file
	// NOTE: For use StdLib must be connected by ConnectStdLib
	bool LoadFromFile( const achar* pPath );
	bool SaveFile( const achar* pPath );
	void Clear();

	void AddRenamedMaterial( const achar* pOriginalName, const achar* pNewName );
	void RemoveRenamedMaterial( const achar* pOriginalName );
	void RemoveAllRenamedMaterials();
	
	void SetAxisUp( axisUp_t axisUp );
	void SetSourcePath( const achar* pSourcePath );
	void SetMaterialsDir( const achar* pMaterialsDir );
	void SetOutputDir( const achar* pOutputDir );
	void SetCombineModels( bool bCombineModels );

	bool IsCombineModels()  const;
	axisUp_t GetAxisUp() const;
	const std::string& GetSourcePath() const;
	const std::string& GetMaterialsDir() const;
	const std::string& GetOutputDir() const;
	const std::unordered_map<std::string, std::string>& GetRenamedMaterials() const;

private:
	bool											bCombineModels;			// If it is TRUE, combines all models into a single model
	axisUp_t										axisUp;
	std::string										sourcePath;
	std::string										materialsDir;			// Path to a directory where the model materials should be located
	std::string										outputDir;
	std::unordered_map<std::string, std::string>	renamedMaterialsDict;
};

#include "smdldoc/smdl_source_doc.inl"