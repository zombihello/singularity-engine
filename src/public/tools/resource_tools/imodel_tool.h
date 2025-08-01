#pragma once
#include "appframework/iappsystem.h"
#include "stdlib/math/axis.h"

//-----------------------------------------------------------------------------
// Model tool interface
//-----------------------------------------------------------------------------
struct resourceToolRenamedMaterial_t
{
	const achar*	pOriginalName;
	const achar*	pNewName;
};


struct resourceToolCompileModelParams_t
{
	bool							bCombineModels;			// If it is TRUE, combines all models into a single model
	axisUp_t						axisUp;
	const achar*					pMaterialsDir;			// Path to a directory where the model materials should be located
	const achar*					pSrcPath;
	const achar*					pDestPath;				// Path to a destination model (without file extension)
	resourceToolRenamedMaterial_t*	pRenamedMaterials;
	uint32							numRenamedMaterials;
};


#define MODEL_TOOL_INTERFACE_VERSION		"SModelTool001"
class IModelTool : public IAppSystem
{
public:
	virtual bool CompileModel( const resourceToolCompileModelParams_t& compileParams ) const = 0;
};