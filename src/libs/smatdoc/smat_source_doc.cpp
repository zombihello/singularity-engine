#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "smatdoc/smat_source_doc.h"

//-----------------------------------------------------------------------------
// Table of variable type names
//-----------------------------------------------------------------------------
static const achar* s_pVarTypeNames[] =
{
	"undefined",		// SMAT_MATERIAL_VAR_TYPE_UNDEFINED
	"bool",				// SMAT_MATERIAL_VAR_TYPE_BOOL
	"int",				// SMAT_MATERIAL_VAR_TYPE_INT
	"float",			// SMAT_MATERIAL_VAR_TYPE_FLOAT
	"vector2d",			// SMAT_MATERIAL_VAR_TYPE_VECTOR_2D
	"vector3d",			// SMAT_MATERIAL_VAR_TYPE_VECTOR_3D
	"vector4d",			// SMAT_MATERIAL_VAR_TYPE_VECTOR_4D
	"matrix",			// SMAT_MATERIAL_VAR_TYPE_MATRIX
	"string",			// SMAT_MATERIAL_VAR_TYPE_STRING
	"texture",			// SMAT_MATERIAL_VAR_TYPE_TEXTURE
	"material"			// SMAT_MATERIAL_VAR_TYPE_MATERIAL
};


//-----------------------------------------------------------------------------
// Functions to convert material type <-> text
//-----------------------------------------------------------------------------
/*
==================
ConvTextToSMTMaterialVarType
==================
*/
static smatMaterialVarType_t ConvTextToSMTMaterialVarType( const achar* pText )
{
	std::string		normalizedText = pText;
	S_Strlwr( ( achar* )normalizedText.c_str() );
	
	for ( uint32 index = 0; index < ARRAYSIZE( s_pVarTypeNames ); ++index )
	{
		if ( normalizedText == s_pVarTypeNames[index] )
		{
			return ( smatMaterialVarType_t )index;
		}
	}
	return SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
}

/*
==================
ConvSMTMaterialVarTypeToText
==================
*/
static const achar* ConvSMTMaterialVarTypeToText( smatMaterialVarType_t varType )
{
	return s_pVarTypeNames[varType];
}


/*
==================
CSMATSourceMaterialDoc::LoadFromFile
==================
*/
bool CSMATSourceMaterialDoc::LoadFromFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Clear the material
	Clear();

	// Try to open a file
	TRefPtr<IStreamDataReader>	pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64	fileSize	= pFile->GetSize() + 1;
	byte*	pBuffer		= ( byte* )Mem_MallocZero( fileSize );

	// Serialize data to the buffer
	pFile->Read( pBuffer, fileSize );

	// Load the JSON file and free allocated memory for the buffer
	CJsonDoc		jsonMaterial;
	bool			bResult = jsonMaterial.LoadFromBuffer( ( const achar* )pBuffer );
	Mem_Free( pBuffer );
	if ( !bResult )
	{
		return false;
	}
	Clear();

	// If all ok grab data from JSON
	return GrabData( jsonMaterial );
}

/*
==================
CSMATSourceMaterialDoc::LoadFromBuffer
==================
*/
bool CSMATSourceMaterialDoc::LoadFromBuffer( const achar* pBuffer )
{
	// Clear the material
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Clear();

	// Load JSON from buffer
	CJsonDoc		jsonMaterial;
	if ( !jsonMaterial.LoadFromBuffer( pBuffer ) )
	{
		return false;
	}
	Clear();

	// If all ok grab data from JSON
	return GrabData( jsonMaterial );
}

/*
==================
CSMATSourceMaterialDoc::GrabData
==================
*/
bool CSMATSourceMaterialDoc::GrabData( const CJsonDoc& jsonDoc )
{
	PROFILE_SCOPE();
	bool	bResult = true;

	// Get output directory
	{
		CJsonValue		jsonOutputDir = jsonDoc.GetValue( "output-dir" );
		if ( jsonOutputDir.IsValid() )
		{
			if ( jsonOutputDir.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string		outputDir = jsonOutputDir.GetString();
				if ( outputDir.empty() )
				{
					Error( "SMATDoc: Invalid 'output-dir', an output directory can't be empty" );
					bResult = false;
				}

				CSMATSourceMaterialDoc::outputDir = outputDir;
			}
			else
			{
				Error( "SMATDoc: Invalid 'output-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMATDoc: A source material '%s' must have 'output-dir' field" );
			bResult = false;
		}
	}

	// Get shader name
	{
		CJsonValue		jsonShaderVar = jsonDoc.GetValue( "shader" );
		if ( jsonShaderVar.IsValid() )
		{
			if ( jsonShaderVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				shaderName = jsonShaderVar.GetString();
			}
			else
			{
				Error( "SMATDoc: Invalid \"shader\", must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMATDoc: A source material must have 'shader'" );
			bResult = false;
		}
	}

	// Get parameters
	{
		CJsonValue		jsonParametersVar = jsonDoc.GetValue( "parameters" );
		if ( jsonParametersVar.IsValid() )
		{
			if ( jsonParametersVar.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue>		jsonParametersArray = jsonParametersVar.GetArray();
				for ( uint32 varIdx = 0, count = ( uint32 )jsonParametersArray.size(); varIdx < count; ++varIdx )
				{
					const CJsonValue&	jsonValue = jsonParametersArray[varIdx];
					if ( jsonValue.IsValid() && jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
					{
						CJsonObject		jsonObject	= jsonValue.GetObject();
						std::string		name		= jsonObject.GetValue( "name" ).GetString();
						std::string		type		= jsonObject.GetValue( "type" ).GetString();
						if ( name.empty() )
						{
							Error( "SMATDoc: Invalid 'name' at parameter id '%i'", varIdx );
							bResult = false;
							continue;
						}

						if ( type.empty() )
						{
							Error( "SMATDoc: Invalid 'type' in '%s' (parameter id: %i)", name.c_str(), varIdx );
							bResult = false;
							continue;
						}

						CSMATMaterialVar		smatMaterialVar;
						smatMaterialVarType_t	smatMaterialVarType	= ConvTextToSMTMaterialVarType( type.c_str() );
						smatMaterialVar.SetName( name.c_str() );
						switch ( smatMaterialVarType )
						{
						case SMAT_MATERIAL_VAR_TYPE_BOOL:
						{
							float		value = 0.f;
							if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be number type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetBoolValue( value > 0.f );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_INT:
						{
							float		value = 0.f;
							if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be number type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetIntValue( ( int32 )value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_FLOAT:
						{
							float		value = 0.f;
							if ( !GrabValueAsNumber( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be number type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetFloatValue( value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
						{
							vec2_t		value = { 0.f, 0.f };
							if ( !GrabValueAsVec2( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be object type with required number fields: 'x' and 'y' (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetVecValue( value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
						{
							vec3_t		value = { 0.f, 0.f, 0.f };
							if ( !GrabValueAsVec3( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be object type with required number fields: 'x', 'y' and 'z' (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetVecValue( value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
						{
							vec4_t		value = { 0.f, 0.f, 0.f, 0.f };
							if ( !GrabValueAsVec4( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be object type with required number fields: 'x', 'y', 'z' and 'w' (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetVecValue( value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_MATRIX:
						{
							matrix_t	value = g_matrixIdentity;
							if ( !GrabValueAsMatrix( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be object type with required vector 4D fields: 'row0', 'row1', 'row2', 'row3'. Each vector 4D must have number fields: 'x', 'y', 'z' and 'w' (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetMatrixValue( value );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_STRING:
						{
							std::string		value;
							if ( !GrabValueAsString( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be string type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetStringValue( value.c_str() );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
						{
							std::string		value;
							if ( !GrabValueAsString( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be string type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetTextureValue( value.c_str() );
							break;
						}

						case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
						{
							std::string		value;
							if ( !GrabValueAsString( jsonObject.GetValue( "value" ), value ) )
							{
								Error( "SMATDoc: Invalid value in '%s', must be string type (parameter id: %i)", name.c_str(), varIdx );
								bResult = false;
								continue;
							}

							smatMaterialVar.SetMaterialValue( value.c_str() );
							break;
						}

						default:
							Error( "SMATDoc: Unknown type '%s' in '%s' (parameter id: %i)", type.c_str(), name.c_str(), varIdx );
							bResult = false;
							continue;
						}

						vars.emplace_back( smatMaterialVar );
					}
					else
					{
						Error( "SMATDoc: Invalid parameter at id %i, must be object with required fields: 'name', 'type' and 'value'", varIdx );
						bResult = false;
					}
				}
			}
			else
			{
				Error( "SMATDoc: Invalid \"parameters\", must be array of objects" );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CSMATSourceMaterialDoc::GrabValueAsNumber
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsNumber( const CJsonValue& jsonValue, float& value ) const
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
CSMATSourceMaterialDoc::GrabValueAsVec2
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsVec2( const CJsonValue& jsonValue, vec2_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject		jsonObject = jsonValue.GetObject();
	CJsonValue		jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue		jsonValueY = jsonObject.GetValue( "y" );
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
CSMATSourceMaterialDoc::GrabValueAsVec3
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsVec3( const CJsonValue& jsonValue, vec3_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject		jsonObject = jsonValue.GetObject();
	CJsonValue		jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue		jsonValueY = jsonObject.GetValue( "y" );
	CJsonValue		jsonValueZ = jsonObject.GetValue( "z" );
	if ( !jsonValueX.IsValid() || !jsonValueY.IsValid() || !jsonValueZ.IsValid() ||
		 !jsonValueX.IsNumber() || !jsonValueY.IsNumber() || !jsonValueZ.IsNumber() )
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
CSMATSourceMaterialDoc::GrabValueAsVec4
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsVec4( const CJsonValue& jsonValue, vec4_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject		jsonObject = jsonValue.GetObject();
	CJsonValue		jsonValueX = jsonObject.GetValue( "x" );
	CJsonValue		jsonValueY = jsonObject.GetValue( "y" );
	CJsonValue		jsonValueZ = jsonObject.GetValue( "z" );
	CJsonValue		jsonValueW = jsonObject.GetValue( "w" );
	if ( !jsonValueX.IsValid() || !jsonValueY.IsValid() || !jsonValueZ.IsValid() || !jsonValueW.IsValid() ||
		 !jsonValueX.IsNumber() || !jsonValueY.IsNumber() || !jsonValueZ.IsNumber() || !jsonValueW.IsNumber() )
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
CSMATSourceMaterialDoc::GrabValueAsMatrix
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsMatrix( const CJsonValue& jsonValue, matrix_t& value ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	CJsonObject		jsonObject = jsonValue.GetObject();
	return	GrabValueAsVec4( jsonObject.GetValue( "row0" ), value[0] ) && GrabValueAsVec4( jsonObject.GetValue( "row1" ), value[1] ) &&
			GrabValueAsVec4( jsonObject.GetValue( "row2" ), value[2] ) && GrabValueAsVec4( jsonObject.GetValue( "row3" ), value[3] );
}

/*
==================
CSMATSourceMaterialDoc::GrabValueAsString
==================
*/
bool CSMATSourceMaterialDoc::GrabValueAsString( const CJsonValue& jsonValue, std::string& value ) const
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
CSMATSourceMaterialDoc::SaveFile
==================
*/
bool CSMATSourceMaterialDoc::SaveFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMATDoc: Failed to open file '%s' for save a SMT material", pPath );
		return false;
	}

	std::string		buffer;
	buffer += "{\n";

	// Write an output directory and the shader name
	buffer += S_Sprintf( "\t\"output-dir\": \"%s\",\n", outputDir.c_str() );
	buffer += S_Sprintf( "\t\"shader\": \"%s\",\n", shaderName.c_str() );

	// Write material parameters
	if ( !vars.empty() )
	{
		buffer += "\t\"parameters\": [\n";
		for ( uint32 varIdx = 0, count = ( uint32 )vars.size(); varIdx < count; ++varIdx )
		{
			const CSMATMaterialVar&		var = vars[varIdx];
			buffer += "\t\t{\n";
			buffer += S_Sprintf( "\t\t\t\"name\": \"%s\",\n", var.GetName() );
			buffer += S_Sprintf( "\t\t\t\"type\": \"%s\",\n", ConvSMTMaterialVarTypeToText( var.GetType() ) );
			buffer += "\t\t\t\"value\": ";
			switch ( var.GetType() )
			{
			case SMAT_MATERIAL_VAR_TYPE_BOOL:	
				buffer += var.GetBoolValue() ? "true\n" : "false\n";		
				break;

			case SMAT_MATERIAL_VAR_TYPE_INT:
				buffer += S_Sprintf( "%i\n", var.GetIntValue() );
				break;

			case SMAT_MATERIAL_VAR_TYPE_FLOAT:
				buffer += S_Sprintf( "%f\n", var.GetFloatValue() );
				break;
			
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
			{
				vec2_t		value = { 0.f, 0.f };
				var.GetVecValue( &value.x, 2 );
				buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f }\n", value.x, value.y );
				break;
			}
			
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
			{
				vec3_t		value = { 0.f, 0.f, 0.f };
				var.GetVecValue( &value.x, 3 );
				buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f, \"z\": %f }\n", value.x, value.y, value.z );
				break;
			}

			case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
			{
				vec4_t		value = { 0.f, 0.f, 0.f, 0.f };
				var.GetVecValue( &value.x, 4 );
				buffer += S_Sprintf( "{ \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f }\n", value.x, value.y, value.z, value.w );
				break;
			}

			case SMAT_MATERIAL_VAR_TYPE_MATRIX:
			{
				matrix_t	value = var.GetMatrixValue();
				buffer += "{\n";
				buffer += S_Sprintf( "\t\t\t\t\"row0\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[0].x, value[0].y, value[0].z, value[0].w );
				buffer += S_Sprintf( "\t\t\t\t\"row1\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[1].x, value[1].y, value[1].z, value[1].w );
				buffer += S_Sprintf( "\t\t\t\t\"row2\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f },\n", value[2].x, value[2].y, value[2].z, value[2].w );
				buffer += S_Sprintf( "\t\t\t\t\"row3\": { \"x\": %f, \"y\": %f, \"z\": %f, \"w\": %f }\n",	value[3].x, value[3].y, value[3].z, value[3].w );
				buffer += "\t\t\t}\n";
				break;
			}

			case SMAT_MATERIAL_VAR_TYPE_STRING:
				buffer += S_Sprintf( "\"%s\"\n", var.GetStringValue() );
				break;

			case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
				buffer += S_Sprintf( "\"%s\"\n", var.GetTextureValue() );
				break;

			case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
				buffer += S_Sprintf( "\"%s\"\n", var.GetMaterialValue() );
				break;

			default:
				Warning( "SMATDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
				Assert( false );
				break;
			}

			if ( varIdx + 1 < count )
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
	pFile->Write( buffer.data(), buffer.size() * sizeof( achar ) );
	return true;
}