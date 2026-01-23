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
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void AddRenamedMaterial( const char* pOriginalName, const char* pNewName );
	void RemoveRenamedMaterial( const char* pOriginalName );
	void RemoveAllRenamedMaterials();

	void SetAxisUp( axisUp_t axisUp );
	void SetSourcePath( const char* pSourcePath );
	void SetMaterialsDir( const char* pMaterialsDir );
	void SetOutputDir( const char* pOutputDir );
	void SetCombineModels( bool bCombineModels );

	bool													  IsCombineModels() const;
	axisUp_t												  GetAxisUp() const;
	const eastl::string&									  GetSourcePath() const;
	const eastl::string&									  GetMaterialsDir() const;
	const eastl::string&									  GetOutputDir() const;
	const eastl::unordered_map<eastl::string, eastl::string>& GetRenamedMaterials() const;

private:
	bool											   bCombineModels;	// If it is TRUE, combines all models into a single model
	axisUp_t										   axisUp;
	eastl::string									   sourcePath;
	eastl::string									   materialsDir;  // Path to a directory where the model materials should be located
	eastl::string									   outputDir;
	eastl::unordered_map<eastl::string, eastl::string> renamedMaterialsDict;
};

#include "utils/smdldoc/smdl_source_doc.inl"
