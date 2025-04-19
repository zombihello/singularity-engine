#ifndef SHADER_BASE_INL
#define SHADER_BASE_INL

/*
==================
CBaseShader::GetPipelineIndex
==================
*/
FORCEINLINE uint64 CBaseShader::GetPipelineIndex( const uint64* pCacheIndices ) const
{
	uint64		pipelineIndex = 0;
	for ( uint32 shaderTypeIdx = 0; shaderTypeIdx < STUDIOAPI_SHADER_NUM_DRAW_TYPES; ++shaderTypeIdx )
	{
		pipelineIndex += cacheInfos[shaderTypeIdx].scale * pCacheIndices[shaderTypeIdx];
	}
	return pipelineIndex;
}

#endif // !SHADER_BASE_INL
