#ifndef SHADERLIB_INL
#define SHADERLIB_INL

/*
==================
CShaderLib::InsertShader
==================
*/
FORCEINLINE void CShaderLib::InsertShader( CBaseShader* pShader )
{
	Assert( pShader );
	shaders.emplace_back( pShader );
}

#endif // !SHADERLIB_INL
