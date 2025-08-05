#pragma once

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
