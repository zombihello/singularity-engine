#pragma once
#include "tier0/tier0.h"

//-----------------------------------------------------------------------------
// Helper for work with source SMDL (Singularity Model) files
//-----------------------------------------------------------------------------
class CSMDLSourceModelDoc
{
public:
	CSMDLSourceModelDoc();

	// Load and save a file
	// NOTE: For use Tier1 must be connected by ConnectTier1
	bool LoadFromFile( const char* pPath );
	bool SaveFile( const char* pPath );
	void Clear();

	void AddRenamedMaterial( const char* pOriginalName, const char* pNewName );
	void RemoveRenamedMaterial( const char* pOriginalName );
	void RemoveAllRenamedMaterials();

	void SetAxisUp( axisUp_t axisUp );
	void SetSourcePath( const char* pSourcePath );
	void SetMaterialsDir( const char* pMaterialsDir );
	void SetCombineModels( bool bCombineModels );

	bool													  IsCombineModels() const;
	axisUp_t												  GetAxisUp() const;
	const eastl::string&									  GetSourcePath() const;
	const eastl::string&									  GetMaterialsDir() const;
	const eastl::unordered_map<eastl::string, eastl::string>& GetRenamedMaterials() const;

private:
	bool											   bCombineModels;	// If it is TRUE, combines all models into a single model
	axisUp_t										   axisUp;
	eastl::string									   srcPath;
	eastl::string									   materialsDir;  // Path to a directory where the model materials should be located
	eastl::unordered_map<eastl::string, eastl::string> renamedMaterialsDict;
};

#include "utils/smdldoc/smdl_source_doc.inl"
