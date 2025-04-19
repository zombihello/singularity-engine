#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "stdlib/jsondoc.h"
#include "filesystem/ifilesystem.h"
#include "smdldoc/smdl_source_doc.h"

// Table for convert text to axisUp_t
static const achar* s_pAxisUpNames[] =
{
	"+x",	// AXIS_UP_PLUS_X
	"-x",	// AXIS_UP_MINUS_X
	"+y",	// AXIS_UP_PLUS_Y
	"-y",	// AXIS_UP_MINUS_Y
	"+z",	// AXIS_UP_PLUS_Z
	"-z"	// AXIS_UP_MINUS_Z
};
static_assert( ARRAYSIZE( s_pAxisUpNames ) == AXIS_UP_NUM, "Array size 's_pAxisUpNames' must be equal to AXIS_UP_NUM" );


/*
==================
ConvTextToAxisUp
==================
*/
static axisUp_t ConvTextToAxisUp( const achar* pText )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pAxisUpNames ); ++index )
	{
		if ( !S_Stricmp( pText, s_pAxisUpNames[index] ) )
		{
			return ( axisUp_t )index;
		}
	}
	return AXIS_UP_DEFAULT;
}


/*
==================
ConvAxisUpToText
==================
*/
static const achar* ConvAxisUpToText( axisUp_t axisUp )
{
	return s_pAxisUpNames[axisUp];
}


/*
==================
CSMDLSourceModelDoc::CSMDLSourceModelDoc
==================
*/
CSMDLSourceModelDoc::CSMDLSourceModelDoc()
	: bCombineModels( false )
	, axisUp( AXIS_UP_DEFAULT )
	, materialsDir( "materials/" )
{}

/*
==================
CSMDLSourceModelDoc::LoadFromFile
==================
*/
bool CSMDLSourceModelDoc::LoadFromFile( const achar* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load a JSON file
	CJsonDoc	jsonDoc;
	if ( !jsonDoc.LoadFromFile( pPath ) )
	{
		Warning( "SMDLDoc: Failed to load '%s', maybe wrong JSON syntax?", pPath );
		return false;
	}
	Clear();
	bool	bResult = true;

	// Get combine models
	{
		CJsonValue		jsonCombineModels = jsonDoc.GetValue( "combine-models" );
		if ( jsonCombineModels.IsValid() )
		{
			if ( jsonCombineModels.IsNumber() )
			{
				bCombineModels = jsonCombineModels.GetNumber();
			}
			else
			{
				Error( "SMDLDoc: Invalid 'combine-models', must be number type" );
				bResult = false;
			}
		}
		else
		{
			bCombineModels = false;
		}
	}

	// Get axis up
	{
		CJsonValue		jsonAxisUp = jsonDoc.GetValue( "axis-up" );
		if ( jsonAxisUp.IsValid() )
		{
			if ( jsonAxisUp.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string		axisUp = jsonAxisUp.GetString();
				if ( axisUp.empty() )
				{
					Error( "SMDLDoc: Invalid 'axis-up', an axis up can't be empty" );
					bResult = false;
				}

				CSMDLSourceModelDoc::axisUp = ConvTextToAxisUp( axisUp.c_str() );
			}
			else
			{
				Error( "SMDLDoc: Invalid 'axis-up', must be string type" );
				bResult = false;
			}
		}
		else
		{
			axisUp = AXIS_UP_DEFAULT;
		}
	}

	// Get source path
	{
		CJsonValue		jsonSourcePath = jsonDoc.GetValue( "source" );
		if ( jsonSourcePath.IsValid() )
		{
			if ( jsonSourcePath.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string		sourcePath = jsonSourcePath.GetString();
				if ( sourcePath.empty() )
				{
					Error( "SMDLDoc: Invalid 'source', an source path can't be empty" );
					bResult = false;
				}

				CSMDLSourceModelDoc::sourcePath = sourcePath;
			}
			else
			{
				Error( "SMDLDoc: Invalid 'source', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMDLDoc: A source model must have 'source'" );
			bResult = false;
		}
	}

	// Get materials directory
	{
		CJsonValue		jsonMaterialsDir = jsonDoc.GetValue( "materials-dir" );
		if ( jsonMaterialsDir.IsValid() )
		{
			if ( jsonMaterialsDir.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string		materialsDir = jsonMaterialsDir.GetString();
				if ( materialsDir.empty() )
				{
					Error( "SMDLDoc: Invalid 'materials-dir', an materials directory can't be empty" );
					bResult = false;
				}

				CSMDLSourceModelDoc::materialsDir = materialsDir;
			}
			else
			{
				Error( "SMDLDoc: Invalid 'materials-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			CSMDLSourceModelDoc::materialsDir = "materials/";
		}
	}

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
					Error( "SMDLDoc: Invalid 'output-dir', an output directory can't be empty" );
					bResult = false;
				}

				CSMDLSourceModelDoc::outputDir = outputDir;
			}
			else
			{
				Error( "SMDLDoc: Invalid 'output-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMDLDoc: A source model '%s' must have 'output-dir' field" );
			bResult = false;
		}
	}

	// Get renamed materials
	{
		CJsonValue		jsonRenamedMaterialsVar = jsonDoc.GetValue( "rename-materials" );
		if ( jsonRenamedMaterialsVar.IsValid() )
		{
			if ( jsonRenamedMaterialsVar.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue>		jsonRenamedMaterialsArray = jsonRenamedMaterialsVar.GetArray();
				for ( uint32 renamedMaterialIdx = 0, count = ( uint32 )jsonRenamedMaterialsArray.size(); renamedMaterialIdx < count; ++renamedMaterialIdx )
				{
					const CJsonValue&		jsonValue = jsonRenamedMaterialsArray[renamedMaterialIdx];
					if ( jsonValue.IsValid() && jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
					{
						CJsonObject		jsonObject		= jsonValue.GetObject();
						std::string		originalName	= jsonObject.GetValue( "original" ).GetString();
						std::string		newName			= jsonObject.GetValue( "new" ).GetString();
						if ( originalName.empty() )
						{
							Error( "SMDLDoc: Invalid 'original' at renamed material id '%i'", renamedMaterialIdx );
							bResult = false;
							continue;
						}

						if ( newName.empty() )
						{
							Error( "SMDLDoc: Invalid 'new' at renamed material id '%i'", renamedMaterialIdx );
							bResult = false;
							continue;
						}

						renamedMaterialsDict[originalName] = newName;
					}
					else
					{
						Error( "SMDLDoc: Invalid renamed material at id %i, must be object with required fields: 'original' and 'new'", renamedMaterialIdx );
						bResult = false;
					}
				}
			}
			else
			{
				Error( "SMDLDoc: Invalid \"rename-materials\", must be array of objects" );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CSMDLSourceModelDoc::SaveFile
==================
*/
bool CSMDLSourceModelDoc::SaveFile( const achar* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMDLDoc: Failed to open file '%s' for save a SMDL source model", pPath );
		return false;
	}

	std::string		buffer;
	buffer += "{\n";

	// Write is need combine models, axis up, source path, materials directory and output directory
	buffer += S_Sprintf( "\t\"combine-models\": %s,\n", bCombineModels ? "true" : "false" );
	buffer += S_Sprintf( "\t\"axis-up\": \"%s\",\n", ConvAxisUpToText( axisUp ) );
	buffer += S_Sprintf( "\t\"source\": \"%s\",\n", sourcePath.c_str() );
	buffer += S_Sprintf( "\t\"materials-dir\": \"%s\",\n", materialsDir.c_str() );
	buffer += S_Sprintf( "\t\"output-dir\": \"%s\",\n", outputDir.c_str() );
	buffer += "\t\"rename-materials\": [\n";
	for ( auto it = renamedMaterialsDict.begin(), itEnd = renamedMaterialsDict.end(); it != itEnd; ++it )
	{
		buffer += "\t\t{\n";
		buffer += S_Sprintf( "\t\t\t\"original\": \"%s\",\n", it->first.c_str() );
		buffer += S_Sprintf( "\t\t\t\"new\": \"%s\"\n", it->second.c_str() );
		buffer += "\t\t}";

		if ( std::next( it, 1 ) != itEnd )
		{
			buffer += ",";
		}
		buffer += "\n";
	}
	buffer += "\t]\n";

	buffer += "}\n";
	pFile->Write( buffer.data(), buffer.size() * sizeof( achar ) );
	return true;
}