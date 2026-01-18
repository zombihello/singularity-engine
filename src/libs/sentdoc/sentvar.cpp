#include "sentdoc/sentvar.h"

/*
==================
CSENTEntityDescVar::CSENTEntityDescVar
==================
*/
CSENTEntityDescVar::CSENTEntityDescVar()
	: type( SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED )
{
}

/*
==================
CSENTEntityDescVar::CSENTEntityDescVar
==================
*/
CSENTEntityDescVar::CSENTEntityDescVar( const CSENTEntityDescVar& other )
	: name( other.name )
	, type( other.type )
{
	Copy( other );
}

/*
==================
CSENTEntityDescVar::~CSENTEntityDescVar
==================
*/
CSENTEntityDescVar::~CSENTEntityDescVar()
{
}

/*
==================
CSENTEntityDescVar::Copy
==================
*/
void CSENTEntityDescVar::Copy( const CSENTEntityDescVar& other )
{
	name		= other.name;
	type		= other.type;
	stringValue = other.stringValue;
	switch ( other.type )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL: boolValue = other.boolValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_INT: intValue = other.intValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT: floatValue = other.floatValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D: vector2DValue = other.vector2DValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D: vector3DValue = other.vector3DValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D: vector4DValue = other.vector4DValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_MATRIX: matrixValue = other.matrixValue; break;
	case SENT_ENTITY_DESC_VAR_TYPE_STRING: pStringValue = stringValue.c_str(); break;
	default:
		Warning( "SENTDoc: Unknown type 0x%X in variable '%s'", other.name.c_str() );
		Assert( false );
		break;
	}
}