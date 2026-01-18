#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "sentdoc/sent_source_doc.h"

//-----------------------------------------------------------------------------
// Table of variable type names
//-----------------------------------------------------------------------------
static const char* s_pVarTypeNames[] = {
	"undefined",  // SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED
	"bool",		  // SENT_ENTITY_DESC_VAR_TYPE_BOOL
	"int",		  // SENT_ENTITY_DESC_VAR_TYPE_INT
	"float",	  // SENT_ENTITY_DESC_VAR_TYPE_FLOAT
	"vector2d",	  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D
	"vector3d",	  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D
	"vector4d",	  // SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D
	"matrix",	  // SENT_ENTITY_DESC_VAR_TYPE_MATRIX
	"string"	  // SENT_ENTITY_DESC_VAR_TYPE_STRING
};
static_assert( ARRAYSIZE( s_pVarTypeNames ) == SENT_ENTITY_DESC_VAR_NUM_TYPES, "Array size 's_pVarTypeNames' must be equal to SENT_ENTITY_DESC_VAR_NUM_TYPES" );

//-----------------------------------------------------------------------------
// Functions to convert entity var type <-> text
//-----------------------------------------------------------------------------
/*
==================
ConvTextToSENTEntityVarType
==================
*/
static sentEntityDescVarType_t ConvTextToSENTEntityVarType( const char* pText )
{
	std::string normalizedText = pText;
	S_Strlwr( (char*)normalizedText.c_str() );

	for ( uint32 index = 0; index < ARRAYSIZE( s_pVarTypeNames ); ++index )
	{
		if ( normalizedText == s_pVarTypeNames[index] )
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
CSENTSourceEntityDescDoc::SaveFile
==================
*/
bool CSENTSourceEntityDescDoc::SaveFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SENTTDoc: Failed to open file '%s' for save a SENT entity", pPath );
		return false;
	}

	std::string buffer;
	buffer += "{\n";

	// Write an output directory
	buffer += S_Sprintf( "\t\"output-dir\": \"%s\",\n", outputDir.c_str() );

	// Write components
	if ( !components.empty() )
	{
		buffer += "\t\"components\": [\n";
		for ( uint32 componentIdx = 0, numComponents = (uint32)components.size(); componentIdx < numComponents; ++componentIdx )
		{
			const CSENTEntityDescComponent&		   component = components[componentIdx];
			const std::vector<CSENTEntityDescVar>& vars		 = component.GetVars();

			buffer += "\t\t{\n";
			buffer += S_Sprintf( "\t\t\t\"type\": \"%s\",\n", component.GetType() );
			if ( !vars.empty() )
			{
				// Write properties of the component
				buffer += "\t\t\t\"properties\": [\n";
				for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
				{
					const CSENTEntityDescVar& var = vars[varIdx];
					buffer += "\t\t\t\t{\n";
					buffer += S_Sprintf( "\t\t\t\t\"name\": \"%s\",\n", var.GetName() );
					buffer += S_Sprintf( "\t\t\t\t\"type\": \"%s\",\n", ConvSENTEntityVarTypeToText( var.GetType() ) );
					buffer += "\t\t\t\t\"value\": ";
					switch ( var.GetType() )
					{
					case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
						buffer += var.GetBoolValue() ? "true\n" : "false\n";
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_INT:
						buffer += S_Sprintf( "%i\n", var.GetIntValue() );
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
						buffer += S_Sprintf( "%f\n", var.GetFloatValue() );
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
					{
						vec2_t value = var.GetVec2Value();
						buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f }\n", value.x, value.y );
						break;
					}

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
					{
						vec3_t value = var.GetVec3Value();
						buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f, \"z\": %f }\n", value.x, value.y, value.z );
						break;
					}

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
					{
						vec4_t value = var.GetVec4Value();
						buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f }\n", value.x, value.y, value.z, value.w );
						break;
					}

					case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
					{
						matrix_t value = var.GetMatrixValue();
						buffer += "{\n";
						buffer += S_Sprintf( "\t\t\t\t\t\"row0\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[0].x, value[0].y, value[0].z, value[0].w );
						buffer += S_Sprintf( "\t\t\t\t\t\"row1\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[1].x, value[1].y, value[1].z, value[1].w );
						buffer += S_Sprintf( "\t\t\t\t\t\"row2\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[2].x, value[2].y, value[2].z, value[2].w );
						buffer += S_Sprintf( "\t\t\t\t\t\"row3\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f }\n", value[3].x, value[3].y, value[3].z, value[3].w );
						buffer += "\t\t\t\t}\n";
						break;
					}

					case SENT_ENTITY_DESC_VAR_TYPE_STRING:
						buffer += S_Sprintf( "\"%s\"\n", var.GetStringValue() );
						break;

					default:
						Warning( "SENTDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
						Assert( false );
						break;
					}

					if ( varIdx + 1 < numVars )
					{
						buffer += "\t\t\t\t},\n";
					}
					else
					{
						buffer += "\t\t\t\t}\n";
					}
				}
				buffer += "\t\t\t]\n";
			}

			if ( componentIdx + 1 < numComponents )
			{
				buffer += "\t\t},\n";
			}
			else
			{
				buffer += "\t\t}\n";
			}
		}
		buffer += "\t]\n";
	}

	buffer += "}\n";
	pFile->Write( buffer.data(), buffer.size() * sizeof( char ) );
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::LoadFromFile
==================
*/
bool CSENTSourceEntityDescDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64 fileSize = pFile->GetSize() + 1;
	byte*  pBuffer	= (byte*)Mem_MallocZero( fileSize );

	// Serialize data to the buffer
	pFile->Read( pBuffer, fileSize );

	// Load the JSON file and free allocated memory for the buffer
	CJsonDoc jsonEntity;
	bool	 bResult = jsonEntity.LoadFromBuffer( (const char*)pBuffer );
	Mem_Free( pBuffer );
	if ( !bResult )
	{
		return false;
	}
	Clear();

	// If all ok grab data from JSON
	return GrabData( jsonEntity );
}

/*
==================
CSENTSourceEntityDescDoc::LoadFromBuffer
==================
*/
bool CSENTSourceEntityDescDoc::LoadFromBuffer( const char* pBuffer )
{
	// Clear the entity
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load JSON from buffer
	CJsonDoc jsonEntity;
	if ( !jsonEntity.LoadFromBuffer( pBuffer ) )
	{
		return false;
	}
	Clear();

	// If all ok grab data from JSON
	return GrabData( jsonEntity );
}

/*
==================
CSENTSourceEntityDescDoc::GrabData
==================
*/
bool CSENTSourceEntityDescDoc::GrabData( const CJsonDoc& jsonDoc )
{
	PROFILE_SCOPE();
	bool bResult = true;

	// Get output directory
	{
		CJsonValue jsonOutputDir = jsonDoc.GetValue( "output-dir" );
		if ( jsonOutputDir.IsValid() )
		{
			if ( jsonOutputDir.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string outputDir = jsonOutputDir.GetString();
				if ( outputDir.empty() )
				{
					Error( "SENTDoc: Invalid 'output-dir', an output directory can't be empty" );
					bResult = false;
				}

				CSENTSourceEntityDescDoc::outputDir = outputDir;
			}
			else
			{
				Error( "SENTDoc: Invalid 'output-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SENTDoc: A source material must have 'output-dir' field" );
			bResult = false;
		}
	}

	// Get components
	{
		CJsonValue jsonComponentsVar = jsonDoc.GetValue( "components" );
		if ( jsonComponentsVar.IsValid() )
		{
			if ( jsonComponentsVar.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue> jsonComponentsArray = jsonComponentsVar.GetArray();
				for ( uint32 componentIdx = 0, numComponents = (uint32)jsonComponentsArray.size(); componentIdx < numComponents; ++componentIdx )
				{
					const CJsonValue&		 jsonComponentVar = jsonComponentsArray[componentIdx];
					CSENTEntityDescComponent sentEntityComponent;
					if ( !GrabValueAsComponent( jsonComponentVar, sentEntityComponent ) )
					{
						Error( "SENTDoc: Invalid component at id %i", componentIdx );
						bResult = false;
						continue;
					}

					components.emplace_back( sentEntityComponent );
				}
			}
			else
			{
				Error( "SENTDoc: Invalid \"components\", must be array of objects" );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsNumber
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsNumber() )
	{
		return false;
	}

	value = jsonValue.GetNumber();
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsVec2
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject jsonObject = jsonValue.GetObject();
	CJsonValue	jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue	jsonValueY = jsonObject.GetValue( "y" );
	if ( !jsonValueX.IsValid() || !jsonValueY.IsValid() || !jsonValueX.IsNumber() || !jsonValueY.IsNumber() )
	{
		return false;
	}

	value.x = jsonValueX.GetNumber();
	value.y = jsonValueY.GetNumber();
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsVec3
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject jsonObject = jsonValue.GetObject();
	CJsonValue	jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue	jsonValueY = jsonObject.GetValue( "y" );
	CJsonValue	jsonValueZ = jsonObject.GetValue( "z" );
	if ( !jsonValueX.IsValid() || !jsonValueY.IsValid() || !jsonValueZ.IsValid() || !jsonValueX.IsNumber() || !jsonValueY.IsNumber() || !jsonValueZ.IsNumber() )
	{
		return false;
	}

	value.x = jsonValueX.GetNumber();
	value.y = jsonValueY.GetNumber();
	value.z = jsonValueZ.GetNumber();
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsVec4
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject jsonObject = jsonValue.GetObject();
	CJsonValue	jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue	jsonValueY = jsonObject.GetValue( "y" );
	CJsonValue	jsonValueZ = jsonObject.GetValue( "z" );
	CJsonValue	jsonValueW = jsonObject.GetValue( "w" );
	if ( !jsonValueX.IsValid() || !jsonValueY.IsValid() || !jsonValueZ.IsValid() || !jsonValueW.IsValid() || !jsonValueX.IsNumber() || !jsonValueY.IsNumber() || !jsonValueZ.IsNumber() || !jsonValueW.IsNumber() )
	{
		return false;
	}

	value.x = jsonValueX.GetNumber();
	value.y = jsonValueY.GetNumber();
	value.z = jsonValueZ.GetNumber();
	value.w = jsonValueW.GetNumber();
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsMatrix
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject jsonObject = jsonValue.GetObject();
	return GrabValueAsVec4( jsonObject.GetValue( "row0" ), value[0] ) && GrabValueAsVec4( jsonObject.GetValue( "row1" ), value[1] ) && GrabValueAsVec4( jsonObject.GetValue( "row2" ), value[2] ) && GrabValueAsVec4( jsonObject.GetValue( "row3" ), value[3] );
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsString
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsString( const CJsonValue& jsonValue, std::string& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		return false;
	}

	value = jsonValue.GetString();
	return true;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsComponent
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsComponent( const CJsonValue& jsonValue, CSENTEntityDescComponent& component ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	bool		bResult	   = true;
	CJsonObject jsonObject = jsonValue.GetObject();
	std::string type	   = jsonObject.GetValue( "type" ).GetString();
	if ( type.empty() )
	{
		Error( "SENTDoc: Invalid 'type' in a component" );
		bResult = false;
	}
	component.SetType( type.c_str() );

	CJsonValue jsonVars = jsonObject.GetValue( "properties" );
	if ( jsonVars.IsValid() )
	{
		if ( jsonVars.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			std::vector<CJsonValue> jsonArray = jsonVars.GetArray();
			for ( uint32 varIdx = 0, numVars = (uint32)jsonArray.size(); varIdx < numVars; ++varIdx )
			{
				const CJsonValue&  jsonVar = jsonArray[varIdx];
				CSENTEntityDescVar sentEntityVar;
				if ( !GrabValueAsVar( jsonVar, sentEntityVar ) )
				{
					Error( "SENTDoc: Invalid property at id %i", varIdx );
					bResult = false;
					continue;
				}

				component.AddVar( sentEntityVar );
			}
		}
		else
		{
			Error( "SENTDoc: Invalid \"properties\" in a component, must be array of objects" );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CSENTSourceEntityDescDoc::GrabValueAsVar
==================
*/
bool CSENTSourceEntityDescDoc::GrabValueAsVar( const CJsonValue& jsonValue, CSENTEntityDescVar& var ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	bool		bResult	   = true;
	CJsonObject jsonObject = jsonValue.GetObject();
	std::string name	   = jsonObject.GetValue( "name" ).GetString();
	std::string type	   = jsonObject.GetValue( "type" ).GetString();
	if ( name.empty() )
	{
		Error( "SENTDoc: Invalid 'name' in a variable" );
		bResult = false;
	}

	if ( type.empty() )
	{
		Error( "SENTDoc: Invalid 'type' in variable '%s'", name.c_str() );
		bResult = false;
	}

	sentEntityDescVarType_t sentEntityVarType = ConvTextToSENTEntityVarType( type.c_str() );
	var.SetName( name.c_str() );
	switch ( sentEntityVarType )
	{
	case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
	{
		float value = 0.f;
		if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be number type", name.c_str() );
			bResult = false;
			break;
		}

		var.SetBoolValue( value > 0.f );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_INT:
	{
		float value = 0.f;
		if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be number type", name.c_str() );
			bResult = false;
			break;
		}

		var.SetIntValue( (int32)value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
	{
		float value = 0.f;
		if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be number type", name.c_str() );
			bResult = false;
			break;
		}

		var.SetFloatValue( value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
	{
		vec2_t value = { 0.f, 0.f };
		if ( !GrabValueAsVec2( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be object type with required number fields: 'x' and 'y'", name.c_str() );
			bResult = false;
			break;
		}

		var.SetVec2Value( value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
	{
		vec3_t value = { 0.f, 0.f, 0.f };
		if ( !GrabValueAsVec3( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be object type with required number fields: 'x', 'y' and 'z'", name.c_str() );
			bResult = false;
			break;
		}

		var.SetVec3Value( value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
	{
		vec4_t value = { 0.f, 0.f, 0.f, 0.f };
		if ( !GrabValueAsVec4( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be object type with required number fields: 'x', 'y', 'z' and 'w'", name.c_str() );
			bResult = false;
			break;
		}

		var.SetVec4Value( value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
	{
		matrix_t value = g_matrixIdentity;
		if ( !GrabValueAsMatrix( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be object type with required vector 4D fields: 'row0', 'row1', 'row2', 'row3'. Each vector 4D must have number fields: 'x', 'y', 'z' and 'w'", name.c_str() );
			bResult = false;
			break;
		}

		var.SetMatrixValue( value );
		break;
	}

	case SENT_ENTITY_DESC_VAR_TYPE_STRING:
	{
		std::string value;
		if ( !GrabValueAsString( jsonObject.GetValue( "value" ), value ) )
		{
			Error( "SENTDoc: Invalid value in '%s', must be string type", name.c_str() );
			bResult = false;
			break;
		}

		var.SetStringValue( value.c_str() );
		break;
	}

	default:
		Error( "SENTDoc: Unknown type '%s' in variable '%s'", type.c_str(), name.c_str() );
		bResult = false;
		break;
	}

	return bResult;
}