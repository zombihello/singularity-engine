#pragma once
#include "appframework/iappsystem.h"
#include "tier1/math/axis.h"

//-----------------------------------------------------------------------------
// Model tool interface
//-----------------------------------------------------------------------------
struct resourceToolRenamedMaterial_t
{
	const char* pOriginalName;
	const char* pNewName;
};

struct resourceToolCompileModelParams_t
{
	bool						   bCombineModels;	// If it is TRUE, combines all models into a single model
	axisUp_t					   axisUp;
	const char*				   pMaterialsDir;  // Path to a directory where the model materials should be located
	const char*				   pSrcPath;
	const char*				   pDestPath;  // Path to a destination model (without file extension)
	resourceToolRenamedMaterial_t* pRenamedMaterials;
	uint32						   numRenamedMaterials;
};

#define MODEL_TOOL_INTERFACE_VERSION "SModelTool001"
class IModelTool : public IAppSystem
{
public:
	virtual bool CompileModel( const resourceToolCompileModelParams_t& compileParams ) const = 0;
};