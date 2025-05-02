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
FORCEINLINE bool CSENTEntityDescVar::GetBoolValue() const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return intValue > 0;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return floatValue > 0.f;
	default:								return false;
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
FORCEINLINE int32 CSENTEntityDescVar::GetIntValue() const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return ( int32 )boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return intValue;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return ( int32 )floatValue;
	default:								return 0;
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
FORCEINLINE float CSENTEntityDescVar::GetFloatValue() const
{
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:	return ( float )boolValue;
	case SENT_ENTITY_DESC_VAR_TYPE_INT:		return ( float )intValue;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:	return floatValue;
	default:								return 0.f;
	}
}

/*
==================
CSENTEntityDescVar::SetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVecValue( const float* pValue, uint32 numComps )
{
	switch ( numComps )
	{
	case 2: SetVecValue( *( vec2_t* ) pValue ); break;
	case 3: SetVecValue( *( vec3_t* ) pValue ); break;
	case 4: SetVecValue( *( vec4_t* ) pValue ); break;
	default:
		AssertMsg( false, "A material variable can take only in range from 2 to 4" );
		break;
	}
}

/*
==================
CSENTEntityDescVar::SetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVecValue( const vec2_t& value )
{
	vector2DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D;
}

/*
==================
CSENTEntityDescVar::SetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVecValue( const vec3_t& value )
{
	vector3DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D;
}

/*
==================
CSENTEntityDescVar::SetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::SetVecValue( const vec4_t& value )
{
	vector4DValue	= value;
	type			= SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D;
}

/*
==================
CSENTEntityDescVar::GetVecValue
==================
*/
FORCEINLINE void CSENTEntityDescVar::GetVecValue( float* pValue, uint32 numComps ) const
{
	Assert( numComps >= 2 && numComps <= 4 );
	switch ( type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
		Assert( numComps >= 2 );
		Mem_Memcpy( pValue, &vector2DValue, numComps * sizeof( float ) );
		break;

	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
		Assert( numComps >= 3 );
		Mem_Memcpy( pValue, &vector3DValue, numComps * sizeof( float ) );
		break;

	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
		Assert( numComps >= 4 );
		Mem_Memcpy( pValue, &vector4DValue, numComps * sizeof( float ) );
		break;

	default:
		Mem_Memset( pValue, 0, numComps * sizeof( float ) );
		break;
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
FORCEINLINE matrix_t CSENTEntityDescVar::GetMatrixValue() const
{
	return type == SENT_ENTITY_DESC_VAR_TYPE_MATRIX ? matrixValue : g_matrixIdentity;
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
FORCEINLINE const achar* CSENTEntityDescVar::GetStringValue() const
{
	return type == SENT_ENTITY_DESC_VAR_TYPE_STRING ? pStringValue : "";
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
