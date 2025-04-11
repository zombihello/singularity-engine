#ifndef COMMON_HLSL
#define COMMON_HLSL 0

// Platform specific definitions and functions
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
	// STUDIOAPI_CONSTANT_BUFFER( MyStruct, myCBuffer, 1, 0 )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_CONSTANT_BUFFER( valueType, valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] ConstantBuffer<valueType> valueName : register( b##bindingIndex, space##descriptorSetIndex )
	
	// Macro for declaring texture value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_TEXTURE_2D( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_TEXTURE_2D( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] Texture2D valueName : register( t##bindingIndex, space##descriptorSetIndex )

	// Macro for declaring texture array value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_TEXTURE_2D_ARRAY( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_TEXTURE_2D_ARRAY( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] Texture2DArray valueName : register( t##bindingIndex, space##descriptorSetIndex )

	// Macro for declaring sampler value with correct binding and descriptor set
	// Usage example:
	// STUDIOAPI_SAMPLER_STATE( [Value name], [Binding index], [Descriptor set index] )
	// -------------------------------------------------------------------------- 
	#define STUDIOAPI_SAMPLER_STATE( valueName, bindingIndex, descriptorSetIndex ) \
		[[vk::binding(bindingIndex, descriptorSetIndex)]] SamplerState valueName : register( s##bindingIndex, space##descriptorSetIndex )
#else
	#error Unknown platform
#endif // PLATFORM_VK

#endif // COMMON_HLSL