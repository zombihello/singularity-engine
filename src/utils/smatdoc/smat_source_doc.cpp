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
	eastl::string normalizedText = pText;
	S_Strlwr( (char*)normalizedText.c_str() );

	for ( uint32 index = 0; index < ARRAYSIZE( s_pVarTypeNames ); ++index )
	{
		if ( normalizedText == s_pVarTypeNames[index] )
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

	// Get a destination file
	bool bGotDefaultValue = false;
	outputDir			  = keyValues.GetString( "output_dir", "", NULL, &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "SMATDoc: Invalid SMAT, not found required field 'output_dir' (file: '%s')", pPath );
		return false;
	}
	if ( outputDir.empty() )
	{
		Error( "SMATDoc: Invalid SMAT, an output directory can't be empty (file: '%s')", pPath );
		return false;
	}

	// Get shader name
	shaderName = keyValues.GetString( "shader", "", NULL, &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "SMATDoc: Invalid SMAT, not found required field 'shader' (file: '%s')", pPath );
		return false;
	}
	if ( shaderName.empty() )
	{
		Error( "SMATDoc: Invalid SMAT, a shader name can't be empty (file: '%s')", pPath );
		return false;
	}

	// Get parameters
	CKeyValues* pParameters = keyValues.FindKey( "parameters" );
	if ( pParameters )
	{
		for ( CKeyValuesSubKeysIterator it( pParameters ); it; ++it )
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
					Error( "SMATDoc: Invalid SMAT, unknown schema '%s' in 'parameters/%s' (file: '%s')", pSchema, it->GetName(), pPath );
					return false;
				}
			}
			// Otherwise the value type get from the KeyValues type
			else
			{
				smatMaterialVarType = ConvKVDataTypeToSMTMaterialVarType( kvDataType );
				if ( smatMaterialVarType == SMAT_MATERIAL_VAR_TYPE_UNDEFINED )
				{
					Error( "SMATDoc: Invalid SMAT, unknown KeyValues type 0x%X in 'parameters/%s' (file: '%s')", kvDataType, it->GetName(), pPath );
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
				Error( "SMATDoc: Invalid SMAT, unknown SMAT value type '%s' in 'parameters/%s' (file: '%s')", ConvSMTMaterialVarTypeToText( smatMaterialVarType ), it->GetName(), pPath );
				Assert( false );
				return false;
			}

			// Add the SMAT var into an array
			vars.emplace_back( smatMaterialVar );
		}
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
	CKeyValues keyValues( "smat" );
	keyValues.SetString( "output_dir", outputDir.c_str() );
	keyValues.SetString( "shader", shaderName.c_str() );

	// Create a key value for each material variable
	if ( !vars.empty() )
	{
		CKeyValues* pParameters = new CKeyValues( "parameters", &keyValues );
		for ( uint32 index = 0, count = (uint32)vars.size(); index < count; ++index )
		{
			const CSMATMaterialVar& smatMaterialVar		= vars[index];
			smatMaterialVarType_t	smatMaterialVarType = smatMaterialVar.GetType();
			const char*				pSchema				= ConvSMTMaterialVarTypeToText( smatMaterialVarType );
			switch ( smatMaterialVarType )
			{
			case SMAT_MATERIAL_VAR_TYPE_UNDEFINED: Warning( "SMATDoc: Material variable '%s' is undefined, skipped", smatMaterialVar.GetName() ); continue;
			case SMAT_MATERIAL_VAR_TYPE_BOOL: pParameters->SetBool( smatMaterialVar.GetName(), smatMaterialVar.GetBoolValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_INT: pParameters->SetInt( smatMaterialVar.GetName(), smatMaterialVar.GetIntValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_FLOAT: pParameters->SetFloat( smatMaterialVar.GetName(), smatMaterialVar.GetFloatValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
			{
				vec2_t value;
				smatMaterialVar.GetVecValue( &value.x, 2 );
				pParameters->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
			{
				vec3_t value;
				smatMaterialVar.GetVecValue( &value.x, 3 );
				pParameters->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
			{
				vec4_t value;
				smatMaterialVar.GetVecValue( &value.x, 4 );
				pParameters->SetString( smatMaterialVar.GetName(), S_VectorToString( value ).c_str(), pSchema );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_MATRIX: pParameters->SetString( smatMaterialVar.GetName(), S_MatrixToString( smatMaterialVar.GetMatrixValue() ).c_str(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_STRING: pParameters->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetStringValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_TEXTURE: pParameters->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetTextureValue(), pSchema ); break;
			case SMAT_MATERIAL_VAR_TYPE_MATERIAL: pParameters->SetString( smatMaterialVar.GetName(), smatMaterialVar.GetMaterialValue(), pSchema ); break;
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
