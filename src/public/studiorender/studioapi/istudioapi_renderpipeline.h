#pragma once
#include "stdlib/math/math.h"
#include "studiorender/studioapi/istudioapi_resource.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIShader;
class IStudioAPIRenderPass;
class IStudioAPIBoundShaderState;

//-----------------------------------------------------------------------------
// Render pipeline enums
//-----------------------------------------------------------------------------
enum studioAPIRasterizerFillMode_t
{
	STUDIOAPI_RASTERIZER_FILL_MODE_POINT,
	STUDIOAPI_RASTERIZER_FILL_MODE_WIREFRAME,
	STUDIOAPI_RASTERIZER_FILL_MODE_SOLID
};

enum studioAPIRasterizerCullMode_t
{
	STUDIOAPI_RASTERIZER_CULL_MODE_NONE,
	STUDIOAPI_RASTERIZER_CULL_MODE_NONE_REVERSED,
	STUDIOAPI_RASTERIZER_CULL_MODE_CW,
	STUDIOAPI_RASTERIZER_CULL_MODE_CCW
};

enum studioAPICompareOp_t
{
	STUDIOAPI_COMPARE_OP_LESS,
	STUDIOAPI_COMPARE_OP_LESS_EQUAL,
	STUDIOAPI_COMPARE_OP_GREATER,
	STUDIOAPI_COMPARE_OP_GREATER_EQUAL,
	STUDIOAPI_COMPARE_OP_EQUAL,
	STUDIOAPI_COMPARE_OP_NOT_EQUAL,
	STUDIOAPI_COMPARE_OP_NEVER,
	STUDIOAPI_COMPARE_OP_ALWAYS
};

enum studioAPIStencilOp_t
{
	STUDIOAPI_STENCIL_OP_KEEP,
	STUDIOAPI_STENCIL_OP_ZERO,
	STUDIOAPI_STENCIL_OP_REPLACE,
	STUDIOAPI_STENCIL_OP_INCREMENT_AND_CLAMP,
	STUDIOAPI_STENCIL_OP_DECREMENT_AND_CLAMP,
	STUDIOAPI_STENCIL_OP_INVERT,
	STUDIOAPI_STENCIL_OP_INCREMENT_AND_WRAP,
	STUDIOAPI_STENCIL_OP_DECREMENT_AND_WRAP
};

enum studioAPIBlendOp_t
{
	STUDIOAPI_BLEND_OP_ADD,
	STUDIOAPI_BLEND_OP_SUBTRACT,
	STUDIOAPI_BLEND_OP_REVERSE_SUBTRACT,
	STUDIOAPI_BLEND_OP_MIN,
	STUDIOAPI_BLEND_OP_MAX
};

enum studioAPIBlendFactor_t
{
	STUDIOAPI_BLEND_FACTOR_ZERO,
	STUDIOAPI_BLEND_FACTOR_ONE,
	STUDIOAPI_BLEND_FACTOR_SRC_COLOR,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	STUDIOAPI_BLEND_FACTOR_DST_COLOR,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	STUDIOAPI_BLEND_FACTOR_SRC_ALPHA,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	STUDIOAPI_BLEND_FACTOR_DST_ALPHA,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	STUDIOAPI_BLEND_FACTOR_CONSTANT_COLOR,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
	STUDIOAPI_BLEND_FACTOR_CONSTANT_ALPHA,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	STUDIOAPI_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	STUDIOAPI_BLEND_FACTOR_SRC1_COLOR,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
	STUDIOAPI_BLEND_FACTOR_SRC1_ALPHA,
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
};

enum studioAPIColorComponentFlag_t
{
	STUDIOAPI_COLOR_COMPONENT_FLAG_R = 1 << 1,
	STUDIOAPI_COLOR_COMPONENT_FLAG_G = 1 << 2,
	STUDIOAPI_COLOR_COMPONENT_FLAG_B = 1 << 3,
	STUDIOAPI_COLOR_COMPONENT_FLAG_A = 1 << 4
};

enum studioAPIPrimitiveTopology_t
{
	STUDIOAPI_PRIMITIVE_TOPOLOGY_POINT_LIST,
	STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_LIST,
	STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN
};

//-----------------------------------------------------------------------------
// Render pipeline structs
//-----------------------------------------------------------------------------
struct studioAPIRasterizerStateInfo_t
{
	studioAPIRasterizerFillMode_t fillMode;
	studioAPIRasterizerCullMode_t cullMode;
	float						  lineWidth;
	bool						  bDepthBiasEnable;
	float						  depthBiasConstantFactor;
	float						  depthBiasSlopeFactor;
	float						  depthBiasClamp;
};

struct studioAPIDepthStateInfo_t
{
	bool				 bTestEnable;
	bool				 bWriteEnable;
	bool				 bBoundsTestEnable;
	studioAPICompareOp_t compareOp;
	float				 minBounds;
	float				 maxBounds;
};

struct studioAPIStencilOpStateInfo_t
{
	studioAPIStencilOp_t failOp;
	studioAPIStencilOp_t passOp;
	studioAPIStencilOp_t depthFailOp;
	studioAPICompareOp_t compareOp;
	uint32				 readMask;
	uint32				 writeMask;
	uint32				 reference;
};

struct studioAPIStencilStateInfo_t
{
	bool						  bTestEnable;
	studioAPIStencilOpStateInfo_t front;
	studioAPIStencilOpStateInfo_t back;
};

struct studioAPIColorBlendAttachmentStateInfo_t
{
	bool				   bBlendEnable;
	studioAPIBlendFactor_t srcColorBlendFactor;
	studioAPIBlendFactor_t dstColorBlendFactor;
	studioAPIBlendOp_t	   colorBlendOp;
	studioAPIBlendFactor_t srcAlphaBlendFactor;
	studioAPIBlendFactor_t dstAlphaBlendFactor;
	studioAPIBlendOp_t	   alphaBlendOp;
	uint32				   colorWriteMask;
};

struct studioAPIColorBlendStateInfo_t
{
	uint32											attachmentCount;
	const studioAPIColorBlendAttachmentStateInfo_t* pAttachments;
	vec4_t											blendConstants;
};

struct studioAPIInputAssemblyStateInfo_t
{
	studioAPIPrimitiveTopology_t topology;
};

struct studioAPIRenderPipelineCreateInfo_t
{
	const IStudioAPIBoundShaderState* pBoundShaderState;
	studioAPIInputAssemblyStateInfo_t inputAssemblyState;
	studioAPIRasterizerStateInfo_t	  rasterizerState;
	studioAPIDepthStateInfo_t		  depthState;
	studioAPIStencilStateInfo_t		  stencilState;
	studioAPIColorBlendStateInfo_t	  colorBlendState;
	const IStudioAPIRenderPass*		  pRenderPass;
};

//-----------------------------------------------------------------------------
// Render pipeline interface
//-----------------------------------------------------------------------------
class IStudioAPIRenderPipeline : public IStudioAPIResource
{
public:
	virtual ~IStudioAPIRenderPipeline() {}
};