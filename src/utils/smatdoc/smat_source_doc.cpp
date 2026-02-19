#include "utils/interfaces/interfaces.h"
#include "tier0/profile.h"
#include "tier1/keyvalues.h"
#include "filesystem/ifilesystem.h"
#include "utils/smatdoc/smat_source_doc.h"

// Table of variable type names
static const char* s_pVarTypeNames[] = {
	"undefined",  // SMAT_MATERIAL_VAR_TYPE_UNDEFINED
	"bool",		  // SMAT_MATERIAL_VAR_TYPE_BOOL
	"int",		  // SMAT_MATERIAL_VAR_TYPE_INT
	"float",	  // SMAT_MATERIAL_VAR_TYPE_FLOAT
	"vec2",		  // SMAT_MATERIAL_VAR_TYPE_VECTOR_2D
	"vec3",		  // SMAT_MATERIAL_VAR_TYPE_VECTOR_3D
	"vec4",		  // SMAT_MATERIAL_VAR_TYPE_VECTOR_4D
	"mat4",		  // SMAT_MATERIAL_VAR_TYPE_MATRIX
	"string",	  // SMAT_MATERIAL_VAR_TYPE_STRING
	"texture",	  // SMAT_MATERIAL_VAR_TYPE_TEXTURE
	"material"	  // SMAT_MATERIAL_VAR_TYPE_MATERIAL
};
static_assert( ARRAYSIZE( s_pVarTypeNames ) == SMAT_MATERIAL_VAR_NUM_TYPES, "Array size 's_pVarTypeNames' must be equal to SMAT_MATERIAL_VAR_NUM_TYPES" );

/*
==================
ConvTextToSMTMaterialVarType
==================
*/
static smatMaterialVarType_t ConvTextToSMTMaterialVarType( const char* pText )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pVarTypeNames ); ++index )
	{
		if ( !S_Stricmp( pText, s_pVarTypeNames[index] ) )
		{
			return (smatMaterialVarType_t)index;
		}
	}
	return SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
}

/*
==================
ConvSMTMaterialVarTypeToText
==================
*/
static const char* ConvSMTMaterialVarTypeToText( smatMaterialVarType_t varType )
{
	return s_pVarTypeNames[varType];
}

/*
==================
ConvKVDataTypeToSMTMaterialVarType
==================
*/
static smatMaterialVarType_t ConvKVDataTypeToSMTMaterialVarType( keyValuesDataType_t kvDataType )
{
	switch ( kvDataType )
	{
	case KEYVALUES_DATA_TYPE_INT:
	case KEYVALUES_DATA_TYPE_INT64:
		return SMAT_MATERIAL_VAR_TYPE_INT;

	case KEYVALUES_DATA_TYPE_FLOAT:
	case KEYVALUES_DATA_TYPE_DOUBLE:
		return SMAT_MATERIAL_VAR_TYPE_FLOAT;

	case KEYVALUES_DATA_TYPE_STRING:
		return SMAT_MATERIAL_VAR_TYPE_STRING;

	default:
		AssertMsg( false, "Unknown KeyValues type 0x%X", kvDataType );
		return SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
	}
}

/*
==================
CSMATSourceMaterialDoc::LoadFromFile
==================
*/
bool CSMATSourceMaterialDoc::LoadFromFile( const char* pPath )
{
	// Load key values file
	PROFILE_SCOPE();
	CKeyValues keyValues( "smat" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}
	Clear();

	// Make sure that we have only one a subkey
	const eastl::list<CKeyValues*>& subKeys = keyValues.GetSubKeys();
	if ( subKeys.empty() )
	{
		Error( "SMATDoc: Invalid SMAT, not found required a shader subkey (file: '%s')", pPath );
		return false;
	}
	if ( subKeys.size() > 1 )
	{
		Error( "SMATDoc: Invalid SMAT, a material can only have one shader subkey (file: '%s')", pPath );
		return false;
	}

	// Get shader
	CKeyValues* pShader = subKeys.front();
	shaderName			= pShader->GetName();
	for ( CKeyValuesSubKeysIterator it( pShader ); it; ++it )
	{
		CSMATMaterialVar	  smatMaterialVar;
		smatMaterialVarType_t smatMaterialVarType = SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
		keyValuesDataType_t	  kvDataType		  = it->GetDataType();
		smatMaterialVar.SetName( it->GetName() );

		// If the key has schema get a value type from them
		const char* pSchema = it->GetSchema( NULL );
		if ( pSchema )
		{
			smatMaterialVarType = ConvTextToSMTMaterialVarType( pSchema );
			if ( smatMaterialVarType == SMAT_MATERIAL_VAR_TYPE_UNDEFINED )
			{
				Error( "SMATDoc: Invalid SMAT, unknown schema '%s' in '%s/%s' (file: '%s')", pSchema, shaderName.c_str(), it->GetName(), pPath );
				return false;
			}
		}
		// Otherwise the value type get from the KeyValues type
		else
		{
			smatMaterialVarType = ConvKVDataTypeToSMTMaterialVarType( kvDataType );
			if ( smatMaterialVarType == SMAT_MATERIAL_VAR_TYPE_UNDEFINED )
			{
				Error( "SMATDoc: Invalid SMAT, unknown KeyValues type 0x%X in '%s/%s' (file: '%s')", kvDataType, shaderName.c_str(), it->GetName(), pPath );
				return false;
			}
		}

		// Set SMAT value
		switch ( smatMaterialVarType )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL: smatMaterialVar.SetBoolValue( it->GetBool( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_INT: smatMaterialVar.SetIntValue( it->GetInt( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_FLOAT: smatMaterialVar.SetFloatValue( it->GetFloat( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_STRING: smatMaterialVar.SetStringValue( it->GetString( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_TEXTURE: smatMaterialVar.SetTextureValue( it->GetString( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_MATERIAL: smatMaterialVar.SetMaterialValue( it->GetString( NULL ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D: smatMaterialVar.SetVecValue( S_VectorCreate<vec2_t>( it->GetString( NULL ) ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D: smatMaterialVar.SetVecValue( S_VectorCreate<vec3_t>( it->GetString( NULL ) ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D: smatMaterialVar.SetVecValue( S_VectorCreate<vec4_t>( it->GetString( NULL ) ) ); break;
		case SMAT_MATERIAL_VAR_TYPE_MATRIX: smatMaterialVar.SetMatrixValue( S_MatrixCreate( it->GetString( NULL ) ) ); break;
		default:
			Error( "SMATDoc: Invalid SMAT, unknown SMAT value type '%s' in '%s/%s' (file: '%s')", ConvSMTMaterialVarTypeToText( smatMaterialVarType ), shaderName.c_str(), it->GetName(), pPath );
			Assert( false );
			return false;
		}

		// Add the SMAT var into an array
		vars.emplace_back( smatMaterialVar );
	}

	// We are done
	return true;
}

/*
==================
CSMATSourceMaterialDoc::SaveFile
==================
*/
bool CSMATSourceMaterialDoc::SaveFile( const char* pPath )
{
	// Create key values
	PROFILE_SCOPE();
	CKeyValues	keyValues( "smat" );
	CKeyValues* pShader = new CKeyValues( shaderName.c_str(), &keyValues );

	// Create a key value for each material variable
	if ( !vars.empty() )
	{
		for ( uint32 index = 0, count = (uint32)vars.size(); index < count; ++index )
		{
			const CSMATMaterialVar& smatMaterialVar		= vars[index];
			smatMaterialVarType_t	smatMaterialVarType = smatMaterialVar.GetType();
			const char*				pSchema				= ConvSMTMaterialVarTypeToText( smatMaterialVarType );
			switch ( smatMaterialVarType )
			{
			case SMAT_MATERIAL_VAR_TYPE_UNDEFINED: Warning( "SMATDoc: Material variable '%s' is undefined, skipped", smatMaterialVar.GetName() ); continue;
			case SMAT_MATERIAL_VAR_TYPE_BOOL: pShader->SetBool( smatMaterialVar.GetName(), smatMaterialVar.GetBoolValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_INT: pShader->SetInt( smatMaterialVar.GetName(), smatMaterialVar.GetIntValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_FLOAT: pShader->SetFloat( smatMaterialVar.GetName(), smatMaterialVar.GetFloatValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
			{
				vec2_t value;
				smatMaterialVar.GetVecValue( &value.x, 2 );
				pShader->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
			{
				vec3_t value;
				smatMaterialVar.GetVecValue( &value.x, 3 );
				pShader->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
			{
				vec4_t value;
				smatMaterialVar.GetVecValue( &value.x, 4 );
				pShader->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_MATRIX: pShader->SetString( smatMaterialVar.GetName(), S_MatrixToString( smatMaterialVar.GetMatrixValue() ).c_str(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_STRING: pShader->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetStringValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_TEXTURE: pShader->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetTextureValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_MATERIAL: pShader->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetMaterialValue(), pSchema ); break;
			default:
				Warning( "SMATDoc: Unknown type 0x%X in variable '%s'", smatMaterialVar.GetType(), smatMaterialVar.GetName() );
				Assert( false );
				break;
			}
		}
	}

	// Save the key values to a file
	return keyValues.SaveToFile( pPath );
}
