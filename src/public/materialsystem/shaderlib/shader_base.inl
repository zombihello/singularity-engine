#pragma once

/*
==================
CBaseShader::GetDefaultRenderState
==================
*/
FORCEINLINE const studioRenderState_t& CBaseShader::GetDefaultRenderState() const
{
	return renderStates[0];
}

/*
==================
CBaseShader::GetShaderComboIndex
==================
*/
FORCEINLINE uint64 CBaseShader::GetShaderComboIndex( const uint64* pCacheIds ) const
{
	uint64 shaderComboIndex = 0;
	for ( uint32 shaderTypeIdx = 0; shaderTypeIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderTypeIdx )
	{
		shaderComboIndex += cacheInfos[shaderTypeIdx].scale * pCacheIds[shaderTypeIdx];
	}
	return shaderComboIndex;
}
