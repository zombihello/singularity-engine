#pragma once

/*
==================
EcsReadData
==================
*/
template<typename TType>
FORCEINLINE void EcsReadData( TType& data, const CSENTEntityDescVar& sentVar, const TType& defaultValue /*= TType()*/ )
{
	static_assert( false, "Unknown type to read from a SENT variable" );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<bool>( bool& data, const CSENTEntityDescVar& sentVar, const bool& defaultValue /*= false*/ )
{
	data = sentVar.GetBoolValue( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<int32>( int32& data, const CSENTEntityDescVar& sentVar, const int32& defaultValue /*= 0*/ )
{
	data = sentVar.GetIntValue( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<float>( float& data, const CSENTEntityDescVar& sentVar, const float& defaultValue /*= 0.f*/ )
{
	data = sentVar.GetFloatValue( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<vec2_t>( vec2_t& data, const CSENTEntityDescVar& sentVar, const vec2_t& defaultValue /*= vec2_t( 0.f, 0.f )*/ )
{
	data = sentVar.GetVec2Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<vec3_t>( vec3_t& data, const CSENTEntityDescVar& sentVar, const vec3_t& defaultValue /*= vec3_t( 0.f, 0.f, 0.f )*/ )
{
	data = sentVar.GetVec3Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<vec4_t>( vec4_t& data, const CSENTEntityDescVar& sentVar, const vec4_t& defaultValue /*= vec4_t( 0.f, 0.f, 0.f, 0.f )*/ )
{
	data = sentVar.GetVec4Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<matrix_t>( matrix_t& data, const CSENTEntityDescVar& sentVar, const matrix_t& defaultValue /*= matrix_t( 0.f )*/ )
{
	data = sentVar.GetMatrixValue( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<eastl::string>( eastl::string& data, const CSENTEntityDescVar& sentVar, const eastl::string& defaultValue /*= ""*/ )
{
	data = sentVar.GetStringValue( defaultValue.c_str() );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<TResourcePtr<ITexture>>( TResourcePtr<ITexture>& data, const CSENTEntityDescVar& sentVar, const TResourcePtr<ITexture>& pDefaultValue )
{
	data = NULL;
	if ( sentVar.IsA( SENT_ENTITY_DESC_VAR_TYPE_STRING ) )
	{
		data = g_pResourceSystem->FindOrLoadResource( sentVar.GetStringValue(), RESOURCE_TYPE_TEXTURE, RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT );
	}

	if ( !data )
	{
		data = pDefaultValue;
	}
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<TResourcePtr<IMaterial>>( TResourcePtr<IMaterial>& data, const CSENTEntityDescVar& sentVar, const TResourcePtr<IMaterial>& pDefaultValue )
{
	data = NULL;
	if ( sentVar.IsA( SENT_ENTITY_DESC_VAR_TYPE_STRING ) )
	{
		data = g_pResourceSystem->FindOrLoadResource( sentVar.GetStringValue(), RESOURCE_TYPE_MATERIAL, RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT );
	}

	if ( !data )
	{
		data = pDefaultValue;
	}
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<TResourcePtr<IEntityDesc>>( TResourcePtr<IEntityDesc>& data, const CSENTEntityDescVar& sentVar, const TResourcePtr<IEntityDesc>& pDefaultValue )
{
	if ( sentVar.IsA( SENT_ENTITY_DESC_VAR_TYPE_STRING ) )
	{
		data = g_pResourceSystem->FindOrLoadResource( sentVar.GetStringValue(), RESOURCE_TYPE_ENTITY_DESC, RESOURCE_LOAD_FLAG_WITHOUT_DEFAULT );
	}

	if ( !data )
	{
		data = pDefaultValue;
	}
}