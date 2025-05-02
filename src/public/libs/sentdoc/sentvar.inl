#ifndef SENTVAR_INL
#define SENTVAR_INL

/*
==================
CSENTEntityDescVar::SetName
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetName( const achar* pName )
{
	name = pName;
}

/*
==================
CSENTEntityDescVar::SetBoolValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetBoolValue( bool bValue )
{
	boolValue	= bValue;
	type		= SENT_ENTITY_DESC_VAR_TYPE_BOOL;
}

/*
==================
CSENTEntityDescVar::GetBoolValue
==================
*/
FORCEINLINE bool CSENTEntityDescVar::GetBoolValue( bool defaultValue /* = false */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return intValue > 0;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return floatValue > 0.f;
	default:								return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::SetIntValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetIntValue( int32 value )
{
	intValue	= value;
	type		= SENT_ENTITY_DESC_VAR_TYPE_INT;
}

/*
==================
CSENTEntityDescVar::GetIntValue
==================
*/
FORCEINLINE int32 CSENTEntityDescVar::GetIntValue( int32 defaultValue /* = 0 */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return ( int32 )boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return intValue;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return ( int32 )floatValue;
	default:								return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::SetFloatValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetFloatValue( float value )
{
	floatValue	= value;
	type		= SENT_ENTITY_DESC_VAR_TYPE_FLOAT;
}

/*
==================
CSENTEntityDescVar::GetFloatValue
==================
*/
FORCEINLINE float CSENTEntityDescVar::GetFloatValue( float defaultValue /* = 0.f */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return ( float )boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return ( float )intValue;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return floatValue;
	default:								return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::SetVec2Value
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVec2Value( const vec2_t& value )
{
	vector2DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D;
}

/*
==================
CSENTEntityDescVar::SetVec3Value
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVec3Value( const vec3_t& value )
{
	vector3DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D;
}

/*
==================
CSENTEntityDescVar::SetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVec4Value( const vec4_t& value )
{
	vector4DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D;
}

/*
==================
CSENTEntityDescVar::GetVec2Value
==================
*/
FORCEINLINE vec2_t CSENTEntityDescVar::GetVec2Value( const vec2_t& defaultValue /* = vec2_t( 0.f, 0.f ) */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:		return vector2DValue;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:		return vector3DValue;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:		return vector4DValue;
	default:										return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::GetVec3Value
==================
*/
FORCEINLINE vec3_t CSENTEntityDescVar::GetVec3Value( const vec3_t& defaultValue /* = vec3_t( 0.f, 0.f, 0.f ) */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:		return vec3_t( vector2DValue.x, vector2DValue.y, defaultValue.z );
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:		return vector3DValue;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:		return vector4DValue;
	default:										return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::GetVec4Value
==================
*/
FORCEINLINE vec4_t CSENTEntityDescVar::GetVec4Value( const vec4_t& defaultValue /* = vec4_t( 0.f, 0.f, 0.f, 0.f ) */ ) const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:		return vec4_t( vector2DValue.x, vector2DValue.y, defaultValue.z, defaultValue.w );
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:		return vec4_t( vector3DValue.x, vector3DValue.y, vector3DValue.z, defaultValue.w );
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:		return vector4DValue;
	default:										return defaultValue;
	}
}

/*
==================
CSENTEntityDescVar::SetMatrixValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetMatrixValue( const matrix_t& value )
{
	matrixValue = value;
	type		= SENT_ENTITY_DESC_VAR_TYPE_MATRIX;
}

/*
==================
CSENTEntityDescVar::GetMatrixValue
==================
*/
FORCEINLINE matrix_t CSENTEntityDescVar::GetMatrixValue( const matrix_t& defaultValue /* = g_matrixIdentity */ ) const
{
	return type == SENT_ENTITY_DESC_VAR_TYPE_MATRIX ? matrixValue : defaultValue;
}

/*
==================
CSENTEntityDescVar::SetStringValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetStringValue( const achar* pValue )
{
	stringValue		= pValue;
	pStringValue	= stringValue.c_str();
	type			= SENT_ENTITY_DESC_VAR_TYPE_STRING;
}

/*
==================
CSENTEntityDescVar::GetStringValue
==================
*/
FORCEINLINE const achar* CSENTEntityDescVar::GetStringValue( const achar* pDefaultValue /* = "" */ ) const
{
	return type == SENT_ENTITY_DESC_VAR_TYPE_STRING ? pStringValue : pDefaultValue;
}

/*
==================
CSENTEntityDescVar::IsDefined
==================
*/
FORCEINLINE bool CSENTEntityDescVar::IsDefined() const
{
	return type != SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
}

/*
==================
CSENTEntityDescVar::SetUndefined
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetUndefined()
{
	type = SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
}

/*
==================
CSENTEntityDescVar::GetName
==================
*/
FORCEINLINE const achar* CSENTEntityDescVar::GetName() const
{
	return name.c_str();
}

/*
==================
CSENTEntityDescVar::GetType
==================
*/
FORCEINLINE sentEntityDescVarType_t CSENTEntityDescVar::GetType() const
{
	return type;
}

/*
==================
CSENTEntityDescVar::operator=
==================
*/
FORCEINLINE CSENTEntityDescVar& CSENTEntityDescVar::operator=( const CSENTEntityDescVar& right )
{
	Copy( right );
	return *this;
}

#endif // !SENTVAR_INL
