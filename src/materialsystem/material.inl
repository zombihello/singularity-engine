#pragma once

/*
==================
CMaterial::insensitiveStringHash_t::operator()
==================
*/
FORCEINLINE size CMaterial::insensitiveStringHash_t::operator()( const char* pString ) const
{
	return FastHashStringInsensitive( pString );
}

/*
==================
CMaterial::insensitiveCompareString_t::operator()
==================
*/
FORCEINLINE bool CMaterial::insensitiveCompareString_t::operator()( const char* pLeft, const char* pRight ) const
{
	return !S_Stricmp( pLeft, pRight );
}

/*
==================
CMaterial::DestroyBuffers
==================
*/
FORCEINLINE void CMaterial::DestroyBuffers()
{
	PROFILER_SCOPE_FUNC();

	// Remove buffers, do they on the render thread to make sure that it will be destroyed at the render thread
	if ( !studioAPIBuffers.empty() )
	{
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveStudioAPIBuffersCmd,
											eastl::vector<CRefPtr<IStudioAPIBuffer>>, studioAPIBuffers, eastl::move( studioAPIBuffers ),
											{
												studioAPIBuffers.clear();
											} );
	}
}

/*
==================
CMaterial::IsResourceVarType
==================
*/
FORCEINLINE bool CMaterial::IsResourceVarType( materialVarType_t varType )
{
	return varType == MATERIALVAR_TYPE_TEXTURE || varType == MATERIALVAR_TYPE_MATERIAL;
}
/*
==================
CMaterial::IsResourceVarType
==================
*/
FORCEINLINE bool CMaterial::IsResourceVarType( shaderParamType_t varType )
{
	return varType == SHADER_PARAM_TYPE_TEXTURE || varType == SHADER_PARAM_TYPE_MATERIAL;
}
