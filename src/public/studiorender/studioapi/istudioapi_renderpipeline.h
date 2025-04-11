/**
 * @file
 * @addtogroup studioapi studioapi
 */

#ifndef ISTUDIOAPI_RENDERPIPELINE_H
#define ISTUDIOAPI_RENDERPIPELINE_H

#include "stdlib/math/math.h"
#include "studiorender/studioapi/istudioapi_resource.h"

// Forward declarations
class IStudioAPIShader;
class IStudioAPIRenderPass;
class IStudioAPIBoundShaderState;

/**
 * @ingroup studioapi
 * @brief Enumeration studio api rasterizer fill mode
 */
enum studioAPIRasterizerFillMode_t
{
	STUDIOAPI_RASTERIZER_FILL_MODE_POINT,					/**< Point */
	STUDIOAPI_RASTERIZER_FILL_MODE_WIREFRAME,				/**< Wireframe */
	STUDIOAPI_RASTERIZER_FILL_MODE_SOLID					/**< Solid */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio rasterizer cull mode
 */
enum studioAPIRasterizerCullMode_t
{
	STUDIOAPI_RASTERIZER_CULL_MODE_NONE,					/**< Cull mode is disabled */
	STUDIOAPI_RASTERIZER_CULL_MODE_NONE_REVERSED,			/**< Cull mode is disabled, front face by counter clockwise */
	STUDIOAPI_RASTERIZER_CULL_MODE_CW,						/**< Cull back */
	STUDIOAPI_RASTERIZER_CULL_MODE_CCW						/**< Cull front */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api compare operation
 */
enum studioAPICompareOp_t
{
	STUDIOAPI_COMPARE_OP_LESS,							/**< Less */
	STUDIOAPI_COMPARE_OP_LESS_EQUAL,					/**< Less equal */
	STUDIOAPI_COMPARE_OP_GREATER,						/**< Greater */
	STUDIOAPI_COMPARE_OP_GREATER_EQUAL,					/**< Greater equal */
	STUDIOAPI_COMPARE_OP_EQUAL,							/**< Equal */
	STUDIOAPI_COMPARE_OP_NOT_EQUAL,						/**< Not equal */
	STUDIOAPI_COMPARE_OP_NEVER,							/**< Never */
	STUDIOAPI_COMPARE_OP_ALWAYS							/**< Always */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api stencil operation
 */
enum studioAPIStencilOp_t
{
	STUDIOAPI_STENCIL_OP_KEEP,								/**< Keep */
	STUDIOAPI_STENCIL_OP_ZERO,								/**< Zero */
	STUDIOAPI_STENCIL_OP_REPLACE,							/**< Replace */
	STUDIOAPI_STENCIL_OP_INCREMENT_AND_CLAMP,				/**< Increment and clamp */
	STUDIOAPI_STENCIL_OP_DECREMENT_AND_CLAMP,				/**< Decrement and clamp */
	STUDIOAPI_STENCIL_OP_INVERT,							/**< Invert */
	STUDIOAPI_STENCIL_OP_INCREMENT_AND_WRAP,				/**< Increment and wrap */
	STUDIOAPI_STENCIL_OP_DECREMENT_AND_WRAP					/**< Decrement and wrap */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api blend operation
 */
enum studioAPIBlendOp_t
{
	STUDIOAPI_BLEND_OP_ADD,								/**< Add */
	STUDIOAPI_BLEND_OP_SUBTRACT,						/**< Subtract */
	STUDIOAPI_BLEND_OP_REVERSE_SUBTRACT,				/**< Reverse subtract */
	STUDIOAPI_BLEND_OP_MIN,								/**< Min */
	STUDIOAPI_BLEND_OP_MAX								/**< Max */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api blend factor
 */
enum studioAPIBlendFactor_t
{
	STUDIOAPI_BLEND_FACTOR_ZERO,							/**< Zero */
	STUDIOAPI_BLEND_FACTOR_ONE,								/**< One */
	STUDIOAPI_BLEND_FACTOR_SRC_COLOR,						/**< Source color */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,				/**< Inverse source color */
	STUDIOAPI_BLEND_FACTOR_DST_COLOR,						/**< Destination color */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_COLOR,				/**< Inverse destination color */
	STUDIOAPI_BLEND_FACTOR_SRC_ALPHA,						/**< Source alpha */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,				/**< Inverse source alpha */
	STUDIOAPI_BLEND_FACTOR_DST_ALPHA,						/**< Destination alpha */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,				/**< Inverse destination alpha */
	STUDIOAPI_BLEND_FACTOR_CONSTANT_COLOR,					/**< Constant color */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,		/**< Inverse constant color */
	STUDIOAPI_BLEND_FACTOR_CONSTANT_ALPHA,					/**< Constant alpha */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,		/**< Inverse constant alpha */
	STUDIOAPI_BLEND_FACTOR_SRC_ALPHA_SATURATE,				/**< Source alpha saturate */
	STUDIOAPI_BLEND_FACTOR_SRC1_COLOR,						/**< Source 1 color */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,			/**< Inverse source 1 color */
	STUDIOAPI_BLEND_FACTOR_SRC1_ALPHA,						/**< Source 1 alpha */
	STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA				/**< Inverse source 1 alpha */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api blend factor
 */
enum studioAPIColorComponentFlag_t
{
	STUDIOAPI_COLOR_COMPONENT_FLAG_R		= 1 << 1,			/**< R channel */
	STUDIOAPI_COLOR_COMPONENT_FLAG_G		= 1 << 2,			/**< G channel */
	STUDIOAPI_COLOR_COMPONENT_FLAG_B		= 1 << 3,			/**< B channel */
	STUDIOAPI_COLOR_COMPONENT_FLAG_A		= 1 << 4			/**< A channel */
};

/**
 * @ingroup studioapi
 * @brief Enumeration studio api primitive topology
 */
enum studioAPIPrimitiveTopology_t
{
	STUDIOAPI_PRIMITIVE_TOPOLOGY_POINT_LIST,			/**< Point */
	STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_LIST,				/**< Line */
	STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_STRIP,			/**< Line strip */
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,			/**< Triangle */
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,		/**< Triangle strip */
	STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN			/**< Triangle fan */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI rasterizer state information
 */
struct studioAPIRasterizerStateInfo_t
{
	studioAPIRasterizerFillMode_t	fillMode;					/**< Fill mode */
	studioAPIRasterizerCullMode_t	cullMode;					/**< Cull mode */
	float							lineWidth;					/**< The width of rasterized line segments */
	bool							bDepthBiasEnable;			/**< Controls whether to bias fragment depth values */
	float							depthBiasConstantFactor;	/**< Scalar factor controlling the constant depth value added to each fragment */
	float							depthBiasSlopeFactor;		/**< Scalar factor applied to a fragment's slope in depth bias calculations */
	float							depthBiasClamp;				/**< The maximum (or minimum) depth bias of a fragment */;
};

/**
 * @ingroup studioapi
 * @brief StudioAPI depth state information
 */
struct studioAPIDepthStateInfo_t
{
	bool							bTestEnable;			/**< Is enabled depth test */
	bool							bWriteEnable;			/**< Is enabled depth write */
	bool							bBoundsTestEnable;		/**< Controls whether depth bounds testing is enabled */
	studioAPICompareOp_t			compareOp;				/**< Depth test compare operation */
	float							minBounds;				/**< The minimum depth bound used in the depth bounds test */
	float							maxBounds;				/**< The maximum depth bound used in the depth bounds test */
};

/**
 * @ingroup studioapi
 * @brief Information about studio api stencil operation state 
 */
struct studioAPIStencilOpStateInfo_t
{
	studioAPIStencilOp_t			failOp;						/**< Specifying the action performed on samples that fail the stencil test */
	studioAPIStencilOp_t			passOp;						/**< Specifying the action performed on samples that pass both the depth and stencil tests */
	studioAPIStencilOp_t			depthFailOp;				/**< Specifying the action performed on samples that pass the stencil test and fail the depth test */
	studioAPICompareOp_t			compareOp;					/**< Specifying the comparison operator used in the stencil test */
	uint32							readMask;					/**< Selects the bits of the unsigned integer stencil values participating in the stencil test */
	uint32							writeMask;					/**< Selects the bits of the unsigned integer stencil values updated by the stencil test in the stencil framebuffer attachment */
	uint32							reference;					/**< An integer stencil reference value that is used in the unsigned stencil comparison */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI stencil state information
 */
struct studioAPIStencilStateInfo_t
{
	bool							bTestEnable;				/**< Is enabled stencil test */
	studioAPIStencilOpStateInfo_t	front;						/**< The stencil test for front side geometry */
	studioAPIStencilOpStateInfo_t	back;						/**< The stencil test for back side geometry */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI color blend attachment information
 */
struct studioAPIColorBlendAttachmentStateInfo_t
{
	bool							bBlendEnable;				/**< Is enabled blend */
	studioAPIBlendFactor_t			srcColorBlendFactor;		/**< Which blend factor is used to determine the source factors (Sr,Sg,Sb) */
	studioAPIBlendFactor_t			dstColorBlendFactor;		/**< Which blend factor is used to determine the destination factors (Dr,Dg,Db) */
	studioAPIBlendOp_t				colorBlendOp;				/**< Which blend operation is used to calculate the RGB values to write to the color attachment */
	studioAPIBlendFactor_t			srcAlphaBlendFactor;		/**< Which blend factor is used to determine the source factor Sa */
	studioAPIBlendFactor_t			dstAlphaBlendFactor;		/**< Which blend factor is used to determine the destination factor Da */
	studioAPIBlendOp_t				alphaBlendOp;				/**< Which blend operation is used to calculate the alpha values to write to the color attachment */
	uint32							colorWriteMask;				/**< A bitmask specifying which of the R, G, B, and/or A components are enabled for writing (see studioAPIColorComponentFlag_t) */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI color blend state information
 */
struct studioAPIColorBlendStateInfo_t
{
	uint32												attachmentCount;		/**< The number of studioAPIColorBlendAttachmentStateInfo_t elements */
	const studioAPIColorBlendAttachmentStateInfo_t*		pAttachments;			/**< A pointer to an array of structures defining blend state for each color attachment */
	vec4_t												blendConstants;			/**< The blend constant that are used in blending */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI input assembly state information
 */
struct studioAPIInputAssemblyStateInfo_t
{
	studioAPIPrimitiveTopology_t						topology;				/**< Primitive topology */
};

/**
 * @ingroup studioapi
 *  @brief StudioAPI render pipeline create information
 */
struct studioAPIRenderPipelineCreateInfo_t
{
	const IStudioAPIBoundShaderState*		pBoundShaderState;		/**< Bound shader state */
	studioAPIInputAssemblyStateInfo_t		inputAssemblyState;		/**< Input assembly state */
	studioAPIRasterizerStateInfo_t			rasterizerState;		/**< Rasterizer state */
	studioAPIDepthStateInfo_t				depthState;				/**< Depth state */
	studioAPIStencilStateInfo_t				stencilState;			/**< Stencil state */
	studioAPIColorBlendStateInfo_t			colorBlendState;		/**< Color blend state */
	const IStudioAPIRenderPass*				pRenderPass;			/**< Render pass for the render pipeline */
};

/**
 * @ingroup studioapi
 * @brief StudioAPI render pipeline interface
 */
class IStudioAPIRenderPipeline : public IStudioAPIResource
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IStudioAPIRenderPipeline() {}
};

#endif // !ISTUDIOAPI_RENDERPIPELINE_H