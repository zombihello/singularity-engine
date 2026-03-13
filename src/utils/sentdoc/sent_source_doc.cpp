#include "utils/interfaces/interfaces.h"
#include "tier0/iprofiler.h"
#include "tier1/keyvalues.h"
#include "filesystem/ifilesystem.h"
#include "utils/sentdoc/sent_source_doc.h"

// Table of variable type names
static const char* s_pVarTypeNames[] = {
	"undefined",  // SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED
	"bool",		  // SENT_ENTITY_DESC_VAR_TYPE_BOOL
	"int",		  // SENT_ENTITY_DESC_VAR_TYPE_INT
	"float",	  // SENT_ENTITY_DESC_VAR_TYPE_FLOAT
	"vec2",		  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D
	"vec3",		  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D
	"vec4",		  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D
	"mat4",		  // SENT_ENTITY_DESC_VAR_TYPE_MATRIX
	"string"	  // SENT_ENTITY_DESC_VAR_TYPE_STRING
};
static_assert( ARRAYSIZE( s_pVarTypeNames ) == SENT_ENTITY_DESC_VAR_NUM_TYPES, "Array size 's_pVarTypeNames' must be equal to SENT_ENTITY_DESC_VAR_NUM_TYPES" );

/*
==================
ConvTextToSENTEntityVarType
==================
*/
static sentEntityDescVarType_t ConvTextToSENTEntityVarType( const char* pText )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pVarTypeNames ); ++index )
	{
		if ( !S_Stricmp( pText, s_pVarTypeNames[index] ) )
		{
			return (sentEntityDescVarType_t)index;
		}
	}
	return SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
}

/*
==================
ConvSENTEntityVarTypeToText
==================
*/
static const char* ConvSENTEntityVarTypeToText( sentEntityDescVarType_t varType )
{
	return s_pVarTypeNames[varType];
}

/*
==================
ConvKVDataTypeToSENTEntityVarType
==================
*/
static sentEntityDescVarType_t ConvKVDataTypeToSENTEntityVarType( keyValuesDataType_t kvDataType )
{
	switch ( kvDataType )
	{
	case KEYVALUES_DATA_TYPE_INT:
	case KEYVALUES_DATA_TYPE_INT64:
		return SENT_ENTITY_DESC_VAR_TYPE_INT;

	case KEYVALUES_DATA_TYPE_FLOAT:
	case KEYVALUES_DATA_TYPE_DOUBLE:
		return SENT_ENTITY_DESC_VAR_TYPE_FLOAT;

	case KEYVALUES_DATA_TYPE_STRING:
		return SENT_ENTITY_DESC_VAR_TYPE_STRING;

	default:
		AssertMsg( false, "Unknown KeyValues type 0x%X", kvDataType );
		return SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
	}
}

/*
==================
CSENTSourceEntityDescDoc::LoadFromFile
==================
*/
bool CSENTSourceEntityDescDoc::LoadFromFile( const char* pPath )
{
	// Load key values file
	PROFILER_SCOPE_FUNC();
	CKeyValues keyValues( "sent" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}
	Clear();

	// Get each component
	for ( CKeyValuesSubKeysIterator itComponent( &keyValues, false, true, true ); itComponent; ++itComponent )
	{
		CSENTEntityDescComponent sentEntityComponent;
		sentEntityComponent.SetType( itComponent->GetName() );

		// Get all parameters
		for ( CKeyValuesSubKeysIterator itParameter( *itComponent ); itParameter; ++itParameter )
		{
			CSENTEntityDescVar		sentEntityDescVar;
			sentEntityDescVarType_t sentEntityDescVarType = SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
			keyValuesDataType_t		kvDataType			  = itParameter->GetDataType();
			sentEntityDescVar.SetName( itParameter->GetName() );

			// If the key has schema get a value type from them
			const char* pSchema = itParameter->GetSchema( NULL );
			if ( pSchema )
			{
				sentEntityDescVarType = ConvTextToSENTEntityVarType( pSchema );
				if ( sentEntityDescVarType == SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED )
				{
					Error( "SENTTDoc: Invalid SENT, unknown schema '%s' in '%s/%s' (file: '%s')", pSchema, itComponent->GetName(), itParameter->GetName(), pPath );
					return false;
				}
			}
			// Otherwise the value type get from the KeyValues type
			else
			{
				sentEntityDescVarType = ConvKVDataTypeToSENTEntityVarType( kvDataType );
				if ( sentEntityDescVarType == SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED )
				{
					Error( "SENTTDoc: Invalid SENT, unknown KeyValues type 0x%X in '%s/%s' (file: '%s')", kvDataType, itComponent->GetName(), itParameter->GetName(), pPath );
					return false;
				}
			}

			// Set a value
			switch ( sentEntityDescVarType )
			{
			case SENT_ENTITY_DESC_VAR_TYPE_BOOL: sentEntityDescVar.SetBoolValue( itParameter->GetBool( NULL ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_INT: sentEntityDescVar.SetIntValue( itParameter->GetInt( NULL ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_FLOAT: sentEntityDescVar.SetFloatValue( itParameter->GetFloat( NULL ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D: sentEntityDescVar.SetVec2Value( S_VectorFromString<vector2_t>( itParameter->GetString( NULL ) ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D: sentEntityDescVar.SetVec3Value( S_VectorFromString<vector3_t>( itParameter->GetString( NULL ) ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D: sentEntityDescVar.SetVec4Value( S_VectorFromString<vector4_t>( itParameter->GetString( NULL ) ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_MATRIX: sentEntityDescVar.SetMatrixValue( S_MatrixFromString( itParameter->GetString( NULL ) ) ); break;
			case SENT_ENTITY_DESC_VAR_TYPE_STRING: sentEntityDescVar.SetStringValue( itParameter->GetString( NULL ) ); break;
			default:
				Error( "SENTTDoc: Invalid SENT, unknown SENT value type '%s' in '%s/%s' (file: '%s')", ConvSENTEntityVarTypeToText( sentEntityDescVarType ), itComponent->GetName(), itParameter->GetName(), pPath );
				Assert( false );
				return false;
			}

			// Add the var into the component
			sentEntityComponent.AddVar( sentEntityDescVar );
		}

		// Add the component into an array
		components.emplace_back( sentEntityComponent );
	}

	// We are done
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::SaveFile
==================
*/
bool CSENTSourceEntityDescDoc::SaveFile( const char* pPath )
{
	// Create key values
	PROFILER_SCOPE_FUNC();
	CKeyValues keyValues( "sent" );
	if ( !components.empty() )
	{
		for ( uint32 componentIdx = 0, numComponents = (uint32)components.size(); componentIdx < numComponents; ++componentIdx )
		{
			// Create a key value for each component property
			const CSENTEntityDescComponent& sentEntityComponent = components[componentIdx];
			CKeyValues*						pComponent			= new CKeyValues( sentEntityComponent.GetType(), &keyValues );
			for ( uint32 propertyIdx = 0, numProperties = sentEntityComponent.GetNumVars(); propertyIdx < numProperties; ++propertyIdx )
			{
				const CSENTEntityDescVar& sentEntityDescVar		= sentEntityComponent.GetVar( propertyIdx );
				sentEntityDescVarType_t	  sentEntityDescVarType = sentEntityDescVar.GetType();
				const char*				  pSchema				= ConvSENTEntityVarTypeToText( sentEntityDescVarType );
				switch ( sentEntityDescVarType )
				{
				case SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED: Warning( "SENTTDoc: Component variable '%s/%s' is undefined, skipped", sentEntityComponent.GetType(), sentEntityDescVar.GetName() ); continue;
				case SENT_ENTITY_DESC_VAR_TYPE_BOOL: pComponent->SetBool( sentEntityDescVar.GetName(), sentEntityDescVar.GetBoolValue(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_INT: pComponent->SetInt( sentEntityDescVar.GetName(), sentEntityDescVar.GetIntValue(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_FLOAT: pComponent->SetFloat( sentEntityDescVar.GetName(), sentEntityDescVar.GetFloatValue(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D: pComponent->SetString( sentEntityDescVar.GetName(), S_VectorToString( sentEntityDescVar.GetVec2Value() ).c_str(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D: pComponent->SetString( sentEntityDescVar.GetName(), S_VectorToString( sentEntityDescVar.GetVec3Value() ).c_str(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D: pComponent->SetString( sentEntityDescVar.GetName(), S_VectorToString( sentEntityDescVar.GetVec4Value() ).c_str(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_MATRIX: pComponent->SetString( sentEntityDescVar.GetName(), S_MatrixToString( sentEntityDescVar.GetMatrixValue() ).c_str(), pSchema ); break;
				case SENT_ENTITY_DESC_VAR_TYPE_STRING: pComponent->SetString( sentEntityDescVar.GetName(), sentEntityDescVar.GetStringValue(), pSchema ); break;
				default:
					Warning( "SENTTDoc: Unknown type 0x%X in component variable '%s/%s'", sentEntityDescVarType, sentEntityComponent.GetType(), sentEntityDescVar.GetName() );
					Assert( false );
					break;
				}
			}
		}
	}

	// Save the key values to a file
	return keyValues.SaveToFile( pPath );
}
