/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_SHADER_H
#define ISTUDIOAPI_SHADER_H

#include "studiorender/studioapi/istudioapi_resource.h"

/**
 * @ingroup studioapi
 * @brief Enumeration studio api shader type
 */
enum studioAPIShaderType_t
{
	STUDIOAPI_SHADER_TYPE_VERTEX,										/**< Vertex shader */
	STUDIOAPI_SHADER_TYPE_HULL,											/**< Hull shader */
	STUDIOAPI_SHADER_TYPE_DOMAIN,										/**< Domain shader */
	STUDIOAPI_SHADER_TYPE_GEOMETRY,										/**< Geometry shader */
	STUDIOAPI_SHADER_TYPE_PIXEL,										/**< Pixel shader */
	STUDIOAPI_SHADER_TYPE_COMPUTE,										/**< Compute shader */
	
	STUDIOAPI_SHADER_NUM_TYPES,											/**< Number of shader types */
	STUDIOAPI_SHADER_NUM_DRAW_TYPES = STUDIOAPI_SHADER_NUM_TYPES - 1,	/**< Number of shader types that are part of pipeline (without compute shader) */
	STUDIOAPI_SHADER_TYPE_UNKNOWN										/**< Unknown shader type */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api vertex element type
 */
enum studioAPIVertexAttributeType_t
{
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_NONE,					/**< Unknown type */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT1,					/**< Float 32-bit */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT2,					/**< 2 32-bit floats */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT3,					/**< 3 32-bit floats */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_FLOAT4,					/**< 4 32-bit floats */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4U,					/**< Unsigned 4 bytes */
	STUDIOAPI_VERTEX_ATTRIBUTE_TYPE_BYTE4UN,				/**< Unsigned 4 normalized bytes */
	STUDIOAPI_VERTEX_ATTRIBUTE_NUM_TYPES					/**< Count vertex element types */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI vertex input rate
 */
enum studioAPIVertexInputRate_t
{
	STUDIOAPI_VERTEX_INPUT_RATE_VERTEX,						/**< Specifies that vertex attribute addressing is a function of the vertex index */
	STUDIOAPI_VERTEX_INPUT_RATE_INSTANCE					/**< Specifies that vertex attribute addressing is a function of the instance index */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI shader dimension type
 */
enum studioAPIShaderDimensionType_t
{
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_1D,			/**< 1D */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_2D,			/**< 2D */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_3D,			/**< 3D */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_CUBE,			/**< Cube */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_RECT,			/**< Rect */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_BUFFER,		/**< Buffer */
	STUDIOAPI_SHADER_IMAGE_DIMENSION_TYPE_SUBPASS_DATA	/**< Subpass data */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI vertex input buffer information
 */
struct studioAPIVertexInputBuffer_t
{
	uint32						slot;		/**< Buffer slot */
	uint32						stride;		/**< Byte stride between consecutive elements within the buffer */
	studioAPIVertexInputRate_t	inputRate;	/**< Specifying whether vertex attribute addressing is a function of the vertex index or of the instance index */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI vertex input attribute
 */
struct studioAPIVertexInputAttribute_t
{
	uint32	bufferSlot;			/**< Buffer slot which this attribute takes its data from */
	uint32	offset;				/**< Byte offset of this attribute relative to the start of an element in the stream buffer */
	uint8	type;				/**< Type element (see studioAPIVertexAttributeType_t) */
	uint8	attributeIndex;		/**< Attribute index */
};

/**
 * @ingroup studioapi
 * @breif StudioAPI vertex declaration interface
 */
class IStudioAPIVertexDeclaration : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIVertexDeclaration() {}
};

/**
 * @ingroup studioapi
 * @brief StudioAPI shader interface
 */
class IStudioAPIShader : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIShader() {}

	/**
	 * @brief Get shader type
	 * @return Return shader type (see studioAPIShaderType_t)
	 */
	virtual studioAPIShaderType_t GetType() const = 0;
};

/**
 * @ingroup studioapi
 * @brief StudioAPI bound shader state interface
 */
class IStudioAPIBoundShaderState : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIBoundShaderState() {}

	/**
	 * @brief Get vertex declaration
	 * @return Return vertex declaration. If the state hasn't one return NULL
	 */
	virtual IStudioAPIVertexDeclaration* GetVertexDeclaration() const = 0;

	/**
	 * @brief Get vertex shader
	 * @return Return vertex shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetVertexShader() const = 0;

	/**
	 * @brief Get pixel shader
	 * @return Return pixel shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetPixelShader() const = 0;

	/**
	 * @brief Get hull shader
	 * @return Return hull shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetHullShader() const = 0;

	/**
	 * @brief Get domain shader
	 * @return Return domain shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetDomainShader() const = 0;

	/**
	 * @brief Get geometry shader
	 * @return Return geometry shader. If the state hasn't one return NULL
	 */
	virtual IStudioAPIShader* GetGeometryShader() const = 0;
};

#endif // !ISTUDIOAPI_SHADER_H