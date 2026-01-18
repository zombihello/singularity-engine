#pragma once
#include "stdlib/math/math.h"
#include "appframework/iappsystem.h"

//-----------------------------------------------------------------------------
// Entity descriptor tool interface
//-----------------------------------------------------------------------------
enum resourceToolEntityDescVarType_t
{
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_UNDEFINED,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_BOOL,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_INT,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_FLOAT,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_2D,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_3D,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_4D,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_MATRIX,
	RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_STRING
};

struct resourceToolEntityDescVar_t
{
	const char*					pName;
	resourceToolEntityDescVarType_t type;
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
	};
};

struct resourceToolEntityDescComponent_t
{
	const char*					   pType;
	const resourceToolEntityDescVar_t* pVars;
	uint32							   numVars;
};

struct resourceToolCompileEntityDescParams_t
{
	const char*							 pDestPath;
	const resourceToolEntityDescComponent_t* pComponents;
	uint32									 numComponents;
};

#define ENTITYDESC_TOOL_INTERFACE_VERSION "SEntityDescTool001"
class IEntityDescTool : public IAppSystem
{
public:
	virtual bool CompileEntityDesc( const resourceToolCompileEntityDescParams_t& compileParams ) const = 0;
};
