#ifndef MATERIAL_INL
#define MATERIAL_INL

/*
==================
CMaterial::insensitiveStringHash_t::operator()
==================
*/
FORCEINLINE std::size_t CMaterial::insensitiveStringHash_t::operator()( const achar* pString ) const
{
	return FastHashFromString( pString );
}


/*
==================
CMaterial::insensitiveCompareString_t::operator()
==================
*/
FORCEINLINE bool CMaterial::insensitiveCompareString_t::operator()( const achar* pLeft, const achar* pRight ) const
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
	PROFILE_SCOPE();

	// Remove buffers, do they on the render thread to make sure that it will be destroyed at the render thread
	if ( !studioAPIBuffers.empty() )
	{
		UNIQUE_RENDER_COMMAND_ONEPARAMETER( CRemoveStudioAPIBuffersCmd,
											std::vector<TRefPtr<IStudioAPIBuffer>>, studioAPIBuffers, std::move( studioAPIBuffers ),
											{
												studioAPIBuffers.clear();
											} );
	}
}

#endif // !MATERIAL_INL
