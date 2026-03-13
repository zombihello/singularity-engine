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
FORCEINLINE void EcsReadData<vector2_t>( vector2_t& data, const CSENTEntityDescVar& sentVar, const vector2_t& defaultValue /*= vector2_t( 0.f, 0.f )*/ )
{
	data = sentVar.GetVec2Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<vector3_t>( vector3_t& data, const CSENTEntityDescVar& sentVar, const vector3_t& defaultValue /*= vector3_t( 0.f, 0.f, 0.f )*/ )
{
	data = sentVar.GetVec3Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<vector4_t>( vector4_t& data, const CSENTEntityDescVar& sentVar, const vector4_t& defaultValue /*= vector4_t( 0.f, 0.f, 0.f, 0.f )*/ )
{
	data = sentVar.GetVec4Value( defaultValue );
}

/*
==================
EcsReadData
==================
*/
template<>
FORCEINLINE void EcsReadData<matrix4x4_t>( matrix4x4_t& data, const CSENTEntityDescVar& sentVar, const matrix4x4_t& defaultValue /*= matrix4x4_t( 0.f )*/ )
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
FORCEINLINE void EcsReadData<CResourcePtr<ITexture>>( CResourcePtr<ITexture>& data, const CSENTEntityDescVar& sentVar, const CResourcePtr<ITexture>& pDefaultValue )
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
FORCEINLINE void EcsReadData<CResourcePtr<IMaterial>>( CResourcePtr<IMaterial>& data, const CSENTEntityDescVar& sentVar, const CResourcePtr<IMaterial>& pDefaultValue )
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
FORCEINLINE void EcsReadData<CResourcePtr<IEntityDesc>>( CResourcePtr<IEntityDesc>& data, const CSENTEntityDescVar& sentVar, const CResourcePtr<IEntityDesc>& pDefaultValue )
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
