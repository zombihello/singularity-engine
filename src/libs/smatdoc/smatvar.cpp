#include "smatdoc/smatvar.h"

/*
==================
CSMATMaterialVar::CSMATMaterialVar
==================
*/
CSMATMaterialVar::CSMATMaterialVar()
	: type( SMAT_MATERIAL_VAR_TYPE_UNDEFINED )
{}

/*
==================
CSMATMaterialVar::CSMATMaterialVar
==================
*/
CSMATMaterialVar::CSMATMaterialVar( const CSMATMaterialVar& other )
	: name( other.name )
	, type( other.type )
{
	Copy( other );
}

/*
==================
CSMATMaterialVar::~CSMATMaterialVar
==================
*/
CSMATMaterialVar::~CSMATMaterialVar()
{}

/*
==================
CSMATMaterialVar::Copy
==================
*/
void CSMATMaterialVar::Copy( const CSMATMaterialVar& other )
{
	name		= other.name;
	type		= other.type;
	stringValue = other.stringValue;
	switch ( other.type )
	{
	case SMAT_MATERIAL_VAR_TYPE_BOOL:		boolValue = other.boolValue;			break;
	case SMAT_MATERIAL_VAR_TYPE_INT:		intValue = other.intValue;				break;
	case SMAT_MATERIAL_VAR_TYPE_FLOAT:		floatValue = other.floatValue;			break;
	case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:	vector2DValue = other.vector2DValue;	break;
	case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:	vector3DValue = other.vector3DValue;	break;
	case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:	vector4DValue = other.vector4DValue;	break;
	case SMAT_MATERIAL_VAR_TYPE_MATRIX:		matrixValue = other.matrixValue;		break;
	case SMAT_MATERIAL_VAR_TYPE_STRING:		pStringValue = stringValue.c_str();		break;
	case SMAT_MATERIAL_VAR_TYPE_TEXTURE:	pTextureValue = stringValue.c_str();	break;
	case SMAT_MATERIAL_VAR_TYPE_MATERIAL:	pMaterialValue = stringValue.c_str();	break;
	default:
		Warning( "SMATDoc: Unknown type 0x%X in variable '%s'", other.name.c_str() );
		Assert( false );
		break;
	}
}