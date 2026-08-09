#ifndef COMMON_HLSL
#define COMMON_HLSL 0
#include "definitions.hlsl"

//-----------------------------------------------------------------------------
// Platform specific definitions and functions
//-----------------------------------------------------------------------------
#if PLATFORM_VK
	// Macro for declaring vertex input with correct location
	// Usage example:
	// STUDIOAPI_VERTEX_INPUT( [Vertex input declaration], [Attribute index] )
	// STUDIOAPI_VERTEX_INPUT( float2 texCoord, 1 )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_VERTEX_INPUT( decl, attributeIndex ) \
		[[vk::location(attributeIndex)]] decl : ATTRIBUTE##attributeIndex
	
	// Macro for declaring constant buffer value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_CONSTANT_BUFFER( [Value type], [Value name], [Binding index], [Descriptor set index] )
	// STUDIOAPI_CONSTANT_BUFFER( MyStruct, myCBuffer, STUDIO_RESOURCE_BINDING_SLOT_USER_BEGIN, 0 )
	// --------------------------------------------------------------------------
	#define STUDIOAPI_CONSTANT_BUFFER( valueType, valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] ConstantBuffer<valueType> valueName

	// Macro for declaring a push constant block
	// Usage example:
	// STUDIOAPI_PUSH_CONSTANT( [Value type], [Value name] )
	// STUDIOAPI_PUSH_CONSTANT( MyStruct, myPushConstants )
	// --------------------------------------------------------------------------
	#define STUDIOAPI_PUSH_CONSTANT( valueType, valueName ) \
		[[vk::push_constant]] ConstantBuffer<valueType> valueName

	// Macro for declaring texture value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_TEXTURE_2D( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_TEXTURE_2D( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] Texture2D valueName

	// Macro for declaring texture array value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_TEXTURE_2D_ARRAY( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_TEXTURE_2D_ARRAY( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] Texture2DArray valueName

	// Macro for declaring sampler value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_SAMPLER_STATE( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_SAMPLER_STATE( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] SamplerState valueName
#else
	#error Unknown platform
#endif // PLATFORM_VK

//-----------------------------------------------------------------------------
// Shader binding slots that are reserved by the engine
//
// NOTES:
//	* All slots below live in descriptor set 0
//	* Must stay in sync with studioResourceBindingSlot_t in src/public/studiorender/studio_resourcebindingslots.h
//-----------------------------------------------------------------------------
#define STUDIO_RESOURCE_BINDING_SLOT_GLOBAL_CB		 	0  // Global shader parameters (see studioGlobalShaderParams_t)
#define STUDIO_RESOURCE_BINDING_SLOT_VERTEXFACTORY_CB 	1  // Vertex factory shader parameters
#define STUDIO_RESOURCE_BINDING_SLOT_FREE_BEGIN	 		2  // Free slots start here

// Include global constants now
#include "globalconstants.hlsl"
#endif // COMMON_HLSL