#include "pch_studioapi_vk.h"
#include "tier1/math/math.h"
#include "studiorender/studioapi_vk/vk_studioapi_renderpipeline.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"
#include "studiorender/studioapi_vk/vk_studioapi_framebuffer.h"

/*
==================
VK_TranslateCullMode
==================
*/
static FORCEINLINE VkCullModeFlags VK_TranslateCullMode( studioAPIRasterizerCullMode_t cullMode )
{
	switch ( cullMode )
	{
	case STUDIOAPI_RASTERIZER_CULL_MODE_CW: return VK_CULL_MODE_BACK_BIT;
	case STUDIOAPI_RASTERIZER_CULL_MODE_CCW: return VK_CULL_MODE_FRONT_BIT;
	case STUDIOAPI_RASTERIZER_CULL_MODE_NONE:
	default: return VK_CULL_MODE_NONE;
	};
}

/*
==================
VK_TranslateFillMode
==================
*/
static FORCEINLINE VkPolygonMode VK_TranslateFillMode( studioAPIRasterizerFillMode_t fillMode )
{
	switch ( fillMode )
	{
	case STUDIOAPI_RASTERIZER_FILL_MODE_POINT: return VK_POLYGON_MODE_POINT;
	case STUDIOAPI_RASTERIZER_FILL_MODE_WIREFRAME: return VK_POLYGON_MODE_LINE;
	default: return VK_POLYGON_MODE_FILL;
	};
}

/*
==================
VK_TranslateCompareOp
==================
*/
static FORCEINLINE VkCompareOp VK_TranslateCompareOp( studioAPICompareOp_t compareOp )
{
	switch ( compareOp )
	{
	case STUDIOAPI_COMPARE_OP_LESS: return VK_COMPARE_OP_LESS;
	case STUDIOAPI_COMPARE_OP_LESS_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
	case STUDIOAPI_COMPARE_OP_GREATER: return VK_COMPARE_OP_GREATER;
	case STUDIOAPI_COMPARE_OP_GREATER_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case STUDIOAPI_COMPARE_OP_EQUAL: return VK_COMPARE_OP_EQUAL;
	case STUDIOAPI_COMPARE_OP_NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
	case STUDIOAPI_COMPARE_OP_NEVER: return VK_COMPARE_OP_NEVER;
	case STUDIOAPI_COMPARE_OP_ALWAYS:
	default: return VK_COMPARE_OP_ALWAYS;
	}
}

/*
==================
VK_TranslateStencilOp
==================
*/
static FORCEINLINE VkStencilOp VK_TranslateStencilOp( studioAPIStencilOp_t stencilOp )
{
	switch ( stencilOp )
	{
	case STUDIOAPI_STENCIL_OP_ZERO: return VK_STENCIL_OP_ZERO;
	case STUDIOAPI_STENCIL_OP_REPLACE: return VK_STENCIL_OP_REPLACE;
	case STUDIOAPI_STENCIL_OP_INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case STUDIOAPI_STENCIL_OP_DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	case STUDIOAPI_STENCIL_OP_INVERT: return VK_STENCIL_OP_INVERT;
	case STUDIOAPI_STENCIL_OP_INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
	case STUDIOAPI_STENCIL_OP_DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
	case STUDIOAPI_STENCIL_OP_KEEP:
	default: return VK_STENCIL_OP_KEEP;
	}
}

/*
==================
VK_TranslateBlendOp
==================
*/
static FORCEINLINE VkBlendOp VK_TranslateBlendOp( studioAPIBlendOp_t blendOp )
{
	switch ( blendOp )
	{
	case STUDIOAPI_BLEND_OP_SUBTRACT: return VK_BLEND_OP_SUBTRACT;
	case STUDIOAPI_BLEND_OP_REVERSE_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
	case STUDIOAPI_BLEND_OP_MIN: return VK_BLEND_OP_MIN;
	case STUDIOAPI_BLEND_OP_MAX: return VK_BLEND_OP_MAX;
	case STUDIOAPI_BLEND_OP_ADD:
	default: return VK_BLEND_OP_ADD;
	}
}

/*
==================
VK_TranslateBlendFactor
==================
*/
static FORCEINLINE VkBlendFactor VK_TranslateBlendFactor( studioAPIBlendFactor_t blendFactor )
{
	switch ( blendFactor )
	{
	case STUDIOAPI_BLEND_FACTOR_ONE: return VK_BLEND_FACTOR_ONE;
	case STUDIOAPI_BLEND_FACTOR_SRC_COLOR: return VK_BLEND_FACTOR_SRC_COLOR;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case STUDIOAPI_BLEND_FACTOR_DST_COLOR: return VK_BLEND_FACTOR_DST_COLOR;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case STUDIOAPI_BLEND_FACTOR_SRC_ALPHA: return VK_BLEND_FACTOR_SRC_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_DST_ALPHA: return VK_BLEND_FACTOR_DST_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_CONSTANT_COLOR: return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	case STUDIOAPI_BLEND_FACTOR_CONSTANT_ALPHA: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_SRC_ALPHA_SATURATE: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	case STUDIOAPI_BLEND_FACTOR_SRC1_COLOR: return VK_BLEND_FACTOR_SRC1_COLOR;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
	case STUDIOAPI_BLEND_FACTOR_SRC1_ALPHA: return VK_BLEND_FACTOR_SRC1_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
	case STUDIOAPI_BLEND_FACTOR_ZERO:
	default: return VK_BLEND_FACTOR_ZERO;
	}
}

/*
==================
VK_TranslateColorComponentFlags
==================
*/
static FORCEINLINE VkColorComponentFlags VK_TranslateColorComponentFlags( uint32 colorComponentFlags )
{
	VkColorComponentFlags vkColorComponentFlags = 0x0;
	if ( colorComponentFlags & STUDIOAPI_COLOR_COMPONENT_FLAG_R )
	{
		vkColorComponentFlags |= VK_COLOR_COMPONENT_R_BIT;
	}

	if ( colorComponentFlags & STUDIOAPI_COLOR_COMPONENT_FLAG_G )
	{
		vkColorComponentFlags |= VK_COLOR_COMPONENT_G_BIT;
	}

	if ( colorComponentFlags & STUDIOAPI_COLOR_COMPONENT_FLAG_B )
	{
		vkColorComponentFlags |= VK_COLOR_COMPONENT_B_BIT;
	}

	if ( colorComponentFlags & STUDIOAPI_COLOR_COMPONENT_FLAG_A )
	{
		vkColorComponentFlags |= VK_COLOR_COMPONENT_A_BIT;
	}
	return vkColorComponentFlags;
}

/*
==================
VK_TranslateColorComponentFlags
==================
*/
static FORCEINLINE VkPrimitiveTopology VK_TranslatePrimitiveTopology( studioAPIPrimitiveTopology_t primitiveTopology )
{
	switch ( primitiveTopology )
	{
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_LIST: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_LINE_STRIP: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case STUDIOAPI_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	default: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

/*
==================
CStudioAPIRenderPipelineVk::CStudioAPIRenderPipelineVk
==================
*/
CStudioAPIRenderPipelineVk::CStudioAPIRenderPipelineVk( const studioAPIRenderPipelineCreateInfo_t& createInfo, const char* pDebugName /* = "" */ )
	: vkPipeline( VK_NULL_HANDLE )
	, pStudioAPIVkShutdownDelegate( NULL )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( createInfo.pBoundShaderState && createInfo.pRenderPass && ( createInfo.colorBlendState.attachmentCount == 0 || createInfo.colorBlendState.pAttachments ) );

	// Viewport state sets the number of viewports and scissor used in this pipeline
	// NOTE: The actual viewport(s) and scissor rectangle(s) will then later be set up at drawing time
	VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo = {};
	vkPipelineViewportStateCreateInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkPipelineViewportStateCreateInfo.viewportCount						= 1;
	vkPipelineViewportStateCreateInfo.scissorCount						= 1;

	// Enable dynamic states
	// While most of the pipeline state needs to be baked into the pipeline state,
	// a limited amount of the state can actually be changed without recreating the pipeline at draw time.
	// Examples are the size of the viewport, line width and blend constants
	static VkDynamicState			 vkPipelineDynamicStates[]		  = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo vkPipelineDynamicStateCreateInfo = {};
	vkPipelineDynamicStateCreateInfo.sType							  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vkPipelineDynamicStateCreateInfo.dynamicStateCount				  = ARRAYSIZE( vkPipelineDynamicStates );
	vkPipelineDynamicStateCreateInfo.pDynamicStates					  = vkPipelineDynamicStates;

	// Rasterization state
	VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo = {};
	vkPipelineRasterizationStateCreateInfo.sType								  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable				  = VK_FALSE;
	vkPipelineRasterizationStateCreateInfo.polygonMode							  = VK_TranslateFillMode( createInfo.rasterizerState.fillMode );
	vkPipelineRasterizationStateCreateInfo.cullMode								  = VK_TranslateCullMode( createInfo.rasterizerState.cullMode );
	vkPipelineRasterizationStateCreateInfo.lineWidth							  = createInfo.rasterizerState.lineWidth;
	vkPipelineRasterizationStateCreateInfo.depthClampEnable						  = VK_FALSE;
	vkPipelineRasterizationStateCreateInfo.depthBiasEnable						  = createInfo.rasterizerState.bDepthBiasEnable;
	vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor				  = S_Floor( createInfo.rasterizerState.depthBiasConstantFactor * (float)( 1 << 24 ) );
	vkPipelineRasterizationStateCreateInfo.depthBiasClamp						  = createInfo.rasterizerState.depthBiasClamp;
	vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor					  = createInfo.rasterizerState.depthBiasSlopeFactor;
	if ( createInfo.rasterizerState.cullMode == STUDIOAPI_RASTERIZER_CULL_MODE_NONE_REVERSED )
	{
		vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}
	else
	{
		vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	}

	// Multi sampling state
	VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo = {};
	vkPipelineMultisampleStateCreateInfo.sType								  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkPipelineMultisampleStateCreateInfo.sampleShadingEnable				  = VK_FALSE;
	vkPipelineMultisampleStateCreateInfo.rasterizationSamples				  = VK_SAMPLE_COUNT_1_BIT;
	vkPipelineMultisampleStateCreateInfo.minSampleShading					  = 1.f;
	vkPipelineMultisampleStateCreateInfo.pSampleMask						  = NULL;
	vkPipelineMultisampleStateCreateInfo.alphaToCoverageEnable				  = VK_FALSE;
	vkPipelineMultisampleStateCreateInfo.alphaToOneEnable					  = VK_FALSE;

	// Depth and stencil state
	VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo = {};
	vkPipelineDepthStencilStateCreateInfo.sType									= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	vkPipelineDepthStencilStateCreateInfo.depthTestEnable						= createInfo.depthState.bTestEnable;
	vkPipelineDepthStencilStateCreateInfo.depthWriteEnable						= createInfo.depthState.bWriteEnable;
	vkPipelineDepthStencilStateCreateInfo.depthCompareOp						= VK_TranslateCompareOp( createInfo.depthState.compareOp );
	vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable					= createInfo.depthState.bBoundsTestEnable;
	vkPipelineDepthStencilStateCreateInfo.stencilTestEnable						= createInfo.stencilState.bTestEnable;
	vkPipelineDepthStencilStateCreateInfo.front.failOp							= VK_TranslateStencilOp( createInfo.stencilState.front.failOp );
	vkPipelineDepthStencilStateCreateInfo.front.passOp							= VK_TranslateStencilOp( createInfo.stencilState.front.passOp );
	vkPipelineDepthStencilStateCreateInfo.front.depthFailOp						= VK_TranslateStencilOp( createInfo.stencilState.front.depthFailOp );
	vkPipelineDepthStencilStateCreateInfo.front.compareOp						= VK_TranslateCompareOp( createInfo.stencilState.front.compareOp );
	vkPipelineDepthStencilStateCreateInfo.front.compareMask						= createInfo.stencilState.front.readMask;
	vkPipelineDepthStencilStateCreateInfo.front.writeMask						= createInfo.stencilState.front.writeMask;
	vkPipelineDepthStencilStateCreateInfo.front.reference						= createInfo.stencilState.front.reference;
	vkPipelineDepthStencilStateCreateInfo.back.failOp							= VK_TranslateStencilOp( createInfo.stencilState.back.failOp );
	vkPipelineDepthStencilStateCreateInfo.back.passOp							= VK_TranslateStencilOp( createInfo.stencilState.back.passOp );
	vkPipelineDepthStencilStateCreateInfo.back.depthFailOp						= VK_TranslateStencilOp( createInfo.stencilState.back.depthFailOp );
	vkPipelineDepthStencilStateCreateInfo.back.compareOp						= VK_TranslateCompareOp( createInfo.stencilState.back.compareOp );
	vkPipelineDepthStencilStateCreateInfo.back.compareMask						= createInfo.stencilState.back.readMask;
	vkPipelineDepthStencilStateCreateInfo.back.writeMask						= createInfo.stencilState.back.writeMask;
	vkPipelineDepthStencilStateCreateInfo.back.reference						= createInfo.stencilState.back.reference;

	// Color blend state
	eastl::vector<VkPipelineColorBlendAttachmentState> vkPipelineColorBlendAttachmentStates( createInfo.colorBlendState.attachmentCount );
	for ( uint32 index = 0; index < createInfo.colorBlendState.attachmentCount; ++index )
	{
		VkPipelineColorBlendAttachmentState&	 vkPipelineColorBlendAttachmentState	= vkPipelineColorBlendAttachmentStates[index];
		studioAPIColorBlendAttachmentStateInfo_t studioAPIColorBlendAttachmentStateInfo = createInfo.colorBlendState.pAttachments[index];
		vkPipelineColorBlendAttachmentState.blendEnable									= studioAPIColorBlendAttachmentStateInfo.bBlendEnable;
		vkPipelineColorBlendAttachmentState.srcColorBlendFactor							= VK_TranslateBlendFactor( studioAPIColorBlendAttachmentStateInfo.srcColorBlendFactor );
		vkPipelineColorBlendAttachmentState.dstColorBlendFactor							= VK_TranslateBlendFactor( studioAPIColorBlendAttachmentStateInfo.dstColorBlendFactor );
		vkPipelineColorBlendAttachmentState.colorBlendOp								= VK_TranslateBlendOp( studioAPIColorBlendAttachmentStateInfo.colorBlendOp );
		vkPipelineColorBlendAttachmentState.srcAlphaBlendFactor							= VK_TranslateBlendFactor( studioAPIColorBlendAttachmentStateInfo.srcAlphaBlendFactor );
		vkPipelineColorBlendAttachmentState.dstAlphaBlendFactor							= VK_TranslateBlendFactor( studioAPIColorBlendAttachmentStateInfo.dstAlphaBlendFactor );
		vkPipelineColorBlendAttachmentState.alphaBlendOp								= VK_TranslateBlendOp( studioAPIColorBlendAttachmentStateInfo.alphaBlendOp );
		vkPipelineColorBlendAttachmentState.colorWriteMask								= VK_TranslateColorComponentFlags( studioAPIColorBlendAttachmentStateInfo.colorWriteMask );
	}

	VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo = {};
	vkPipelineColorBlendStateCreateInfo.sType								= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkPipelineColorBlendStateCreateInfo.logicOpEnable						= VK_FALSE;
	vkPipelineColorBlendStateCreateInfo.logicOp								= VK_LOGIC_OP_COPY;
	vkPipelineColorBlendStateCreateInfo.attachmentCount						= (uint32)vkPipelineColorBlendAttachmentStates.size();
	vkPipelineColorBlendStateCreateInfo.pAttachments						= vkPipelineColorBlendAttachmentStates.data();
	vkPipelineColorBlendStateCreateInfo.blendConstants[0]					= createInfo.colorBlendState.blendConstants.x;
	vkPipelineColorBlendStateCreateInfo.blendConstants[1]					= createInfo.colorBlendState.blendConstants.y;
	vkPipelineColorBlendStateCreateInfo.blendConstants[2]					= createInfo.colorBlendState.blendConstants.z;
	vkPipelineColorBlendStateCreateInfo.blendConstants[3]					= createInfo.colorBlendState.blendConstants.w;

	// Input assembly state describes how primitives are assembled
	VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo = {};
	vkPipelineInputAssemblyStateCreateInfo.sType								  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkPipelineInputAssemblyStateCreateInfo.topology								  = VK_TranslatePrimitiveTopology( createInfo.inputAssemblyState.topology );
	vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable				  = VK_FALSE;

	// Create a render pipeline
	// Save fields
	pBoundShaderState												  = (CStudioAPIBoundShaderStateVk*)createInfo.pBoundShaderState;
	CStudioAPIRenderPassVk*						   pRenderPass		  = (CStudioAPIRenderPassVk*)createInfo.pRenderPass;
	CStudioAPIVertexDeclarationVk*				   pVertexDeclaration = (CStudioAPIVertexDeclarationVk*)pBoundShaderState->GetVertexDeclaration();
	eastl::vector<VkPipelineShaderStageCreateInfo> vkPipelineShaderStageCreateInfos;

	if ( pBoundShaderState->GetVertexShader() )
	{
		CStudioAPIShaderVk* pShader = (CStudioAPIShaderVk*)pBoundShaderState->GetVertexShader();
		vkPipelineShaderStageCreateInfos.emplace_back( pShader->GetVkPipelineShaderStageInfo() );
	}
	if ( pBoundShaderState->GetPixelShader() )
	{
		CStudioAPIShaderVk* pShader = (CStudioAPIShaderVk*)pBoundShaderState->GetPixelShader();
		vkPipelineShaderStageCreateInfos.emplace_back( pShader->GetVkPipelineShaderStageInfo() );
	}
	if ( pBoundShaderState->GetHullShader() )
	{
		CStudioAPIShaderVk* pShader = (CStudioAPIShaderVk*)pBoundShaderState->GetHullShader();
		vkPipelineShaderStageCreateInfos.emplace_back( pShader->GetVkPipelineShaderStageInfo() );
	}
	if ( pBoundShaderState->GetDomainShader() )
	{
		CStudioAPIShaderVk* pShader = (CStudioAPIShaderVk*)pBoundShaderState->GetDomainShader();
		vkPipelineShaderStageCreateInfos.emplace_back( pShader->GetVkPipelineShaderStageInfo() );
	}
	if ( pBoundShaderState->GetGeometryShader() )
	{
		CStudioAPIShaderVk* pShader = (CStudioAPIShaderVk*)pBoundShaderState->GetGeometryShader();
		vkPipelineShaderStageCreateInfos.emplace_back( pShader->GetVkPipelineShaderStageInfo() );
	}

	VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo = {};
	vkGraphicsPipelineCreateInfo.sType						  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkGraphicsPipelineCreateInfo.stageCount					  = (uint32)vkPipelineShaderStageCreateInfos.size();
	vkGraphicsPipelineCreateInfo.pStages					  = vkPipelineShaderStageCreateInfos.data();
	vkGraphicsPipelineCreateInfo.pVertexInputState			  = &pVertexDeclaration->GetVkPipelineVertexInputStateInfo();
	vkGraphicsPipelineCreateInfo.pInputAssemblyState		  = &vkPipelineInputAssemblyStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pViewportState				  = &vkPipelineViewportStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pRasterizationState		  = &vkPipelineRasterizationStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pMultisampleState			  = &vkPipelineMultisampleStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDepthStencilState			  = &vkPipelineDepthStencilStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pColorBlendState			  = &vkPipelineColorBlendStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDynamicState				  = &vkPipelineDynamicStateCreateInfo;
	vkGraphicsPipelineCreateInfo.layout						  = pBoundShaderState->GetDescriptorSetsLayout().GetVkPipelineLayout();
	vkGraphicsPipelineCreateInfo.renderPass					  = pRenderPass->GetVkRenderPass();
	vkGraphicsPipelineCreateInfo.subpass					  = 0;
	vkGraphicsPipelineCreateInfo.basePipelineHandle			  = VK_NULL_HANDLE;
	vkGraphicsPipelineCreateInfo.basePipelineIndex			  = -1;
	STUDIOAPI_VK_VERIFY_RESULT( vkCreateGraphicsPipelines( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), VK_NULL_HANDLE, 1, &vkGraphicsPipelineCreateInfo, NULL, &vkPipeline ) );

	// Register in 'onStudioAPIVkShutodwn' for destroy Vulkan objects when the one is shutdown
	pStudioAPIVkShutdownDelegate = g_StudioAPIVk.OnStudioAPIVkShutdown().AddFunc( &CStudioAPIRenderPipelineVk::OnStudioAPIVkShutdown, this );
}

/*
==================
CStudioAPIRenderPipelineVk::~CStudioAPIRenderPipelineVk
==================
*/
CStudioAPIRenderPipelineVk::~CStudioAPIRenderPipelineVk()
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );

	// Broadcast event about deletion of the pipeline
	onRenderPipelineDeleted.Broadcast( this );

	// Destroy the Vulkan pipeline
	if ( vkPipeline != VK_NULL_HANDLE )
	{
		g_StudioAPIVk.GetMemoryMgr().FreeResource( [vkPipeline = vkPipeline]()
												   { vkDestroyPipeline( g_StudioAPIVk.GetDevice().GetVkLogicalDevice(), vkPipeline, NULL ); } );
		vkPipeline = VK_NULL_HANDLE;
	}

	// Clear fields
	pBoundShaderState = NULL;

	// Remove CStudioAPIShaderVk::OnStudioAPIVkShutdown from event 'onStudioAPIVkShutodwn'
	if ( pStudioAPIVkShutdownDelegate )
	{
		g_StudioAPIVk.OnStudioAPIVkShutdown().RemoveFunc( pStudioAPIVkShutdownDelegate );
		pStudioAPIVkShutdownDelegate = NULL;
	}
}

/*
==================
CStudioAPIRenderPipelineVk::OnStudioAPIVkShutdown
==================
*/
void CStudioAPIRenderPipelineVk::OnStudioAPIVkShutdown( void* pUserData )
{
	Assert( pUserData );
	CStudioAPIRenderPipelineVk* pStudioAPIRenderPipeline   = (CStudioAPIRenderPipelineVk*)pUserData;
	pStudioAPIRenderPipeline->pStudioAPIVkShutdownDelegate = NULL;
	pStudioAPIRenderPipeline->~CStudioAPIRenderPipelineVk();
}
