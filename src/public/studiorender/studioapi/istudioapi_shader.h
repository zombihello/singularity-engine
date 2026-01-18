#pragma once
#include "studiorender/studioapi/istudioapi_resource.h"

//-----------------------------------------------------------------------------
// Shader enums
//-----------------------------------------------------------------------------
enum studioAPIShaderType_t
{
	STUDIOAPI_SHADER_TYPE_VERTEX,
	STUDIOAPI_SHADER_TYPE_HULL,
	STUDIOAPI_SHADER_TYPE_DOMAIN,
	STUDIOAPI_SHADER_TYPE_GEOMETRY,
	STUDIOAPI_SHADER_TYPE_PIXEL,
	STUDIOAPI_SHADER_TYPE_COMPUTE,

	STUDIOAPI_SHADER_NUM_TYPES,
	STUDIOAPI_SHADER_NUM_DRAW_TYPES = STUDIOAPI_SHADER_NUM_TYPES - 1,
	STUDIOAPI_SHADER_TYPE_UNKNOWN
};

enum studioAPIVertexAttributeType_t
{
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_NONE,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT1,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4U,
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN,
	STUDIOAPI_VERTEX_ATTRIBUTE_NUM_TYPES
};

enum studioAPIVertexInputRate_t
{
	STUDIOAPI_VERTEX_INPUT_RATE_VERTEX,
	STUDIOAPI_VERTEX_INPUT_RATE_INSTANCE
};

enum studioAPIShaderDimensionType_t
{
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_1D,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_2D,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_3D,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_CUBE,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_RECT,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_BUFFER,
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_SUBPASS_DATA
};

//-----------------------------------------------------------------------------
// Shader structs
//-----------------------------------------------------------------------------
struct studioAPIVertexInputBuffer_t
{
	uint32					   slot;
	uint32					   stride;
	studioAPIVertexInputRate_t inputRate;
};

struct studioAPIVertexInputAttribute_t
{
	uint32 bufferSlot;
	uint32 offset;
	uint8  type;
	uint8  attributeIndex;
};

//-----------------------------------------------------------------------------
// StudioAPI vertex declaration interface
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIVertexDeclaration() {}
};

//-----------------------------------------------------------------------------
// StudioAPI shader interface
//-----------------------------------------------------------------------------
class IStudioAPIShader : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIShader() {}
	virtual studioAPIShaderType_t GetType() const = 0;
};

//-----------------------------------------------------------------------------
// StudioAPI bound shader state interface
//-----------------------------------------------------------------------------
class IStudioAPIBoundShaderState : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIBoundShaderState() {}

	virtual IStudioAPIVertexDeclaration* GetVertexDeclaration() const = 0;
	virtual IStudioAPIShader*			 GetVertexShader() const	  = 0;
	virtual IStudioAPIShader*			 GetPixelShader() const		  = 0;
	virtual IStudioAPIShader*			 GetHullShader() const		  = 0;
	virtual IStudioAPIShader*			 GetDomainShader() const	  = 0;
	virtual IStudioAPIShader*			 GetGeometryShader() const	  = 0;
};