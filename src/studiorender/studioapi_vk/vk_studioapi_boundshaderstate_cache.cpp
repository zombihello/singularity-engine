#include "pch_studioapi_vk.h"
#include "studiorender/studioapi_vk/vk_studioapi_shader.h"
#include "studiorender/studioapi_vk/vk_studioapi_boundshaderstate_cache.h"

/*
==================
CStudioAPIBoundShaderStateKeyVk::CStudioAPIBoundShaderStateKeyVk
==================
*/
CStudioAPIBoundShaderStateKeyVk::CStudioAPIBoundShaderStateKeyVk( CStudioAPIVertexDeclarationVk* pVertexDeclaration, CStudioAPIVertexShaderVk* pVertexShader, CStudioAPIPixelShaderVk* pPixelShader, CStudioAPIHullShaderVk* pHullShader /* = NULL */, CStudioAPIDomainShaderVk* pDomainShader /* = NULL */, CStudioAPIGeometryShaderVk* pGeometryShader /* = NULL */ )
	: hash( 0 )
{
	Assert( pVertexDeclaration );
	hash = FastHash( pVertexDeclaration );
	hash = FastHash( pVertexShader, hash );
	hash = FastHash( pPixelShader, hash );
	hash = FastHash( pHullShader, hash );
	hash = FastHash( pDomainShader, hash );
	hash = FastHash( pGeometryShader, hash );
}