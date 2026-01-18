#pragma once
#include "stdlib/math/math.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Material tool interface
//-----------------------------------------------------------------------------
enum resourceToolMaterialVarType_t
{
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_UNDEFINED,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_BOOL,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_INT,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_FLOAT,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_2D,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_3D,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_4D,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATRIX,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_STRING,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_TEXTURE,
	RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATERIAL
};

struct resourceToolMaterialVar_t
{
	const char*				  pName;
	resourceToolMaterialVarType_t type;
	union
	{
		bool		 boolValue;
		int32		 intValue;
		float		 floatValue;
		vec2_t		 vector2DValue;
		vec3_t		 vector3DValue;
		vec4_t		 vector4DValue;
		matrix_t	 matrixValue;
		const char* pStringValue;
		const char* pTextureValue;
		const char* pMaterialValue;
	};
};

struct resourceToolCompileMaterialParams_t
{
	const char*					 pDestPath;
	const char*					 pShaderName;
	const resourceToolMaterialVar_t* pVars;
	uint32							 numVars;
};

#define MATERIAL_TOOL_INTERFACE_VERSION "SMaterialTool001"
class IMaterialTool : public IAppSystem
{
public:
	virtual bool CompileMaterial( const resourceToolCompileMaterialParams_t& compileParams ) const = 0;
};