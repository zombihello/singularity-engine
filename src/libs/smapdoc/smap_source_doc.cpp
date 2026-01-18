#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "stdlib/jsondoc.h"
#include "filesystem/ifilesystem.h"
#include "smapdoc/smap_source_doc.h"

/*
==================
CSMAPSourceMapDoc::LoadFromFile
==================
*/
bool CSMAPSourceMapDoc::LoadFromFile( const char* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );

	// Load a JSON file
	CJsonDoc jsonDoc;
	if ( !jsonDoc.LoadFromFile( pPath ) )
	{
		Warning( "SMAPDoc: Failed to load '%s', maybe wrong JSON syntax?", pPath );
		return false;
	}
	Clear();

	// If all ok grab data from JSON
	return GrabData( jsonDoc );
}

/*
==================
CSMAPSourceMapDoc::GrabData
==================
*/
bool CSMAPSourceMapDoc::GrabData( const CJsonDoc& jsonDoc )
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
					Error( "SMAPDoc: Invalid 'output-dir', an output directory can't be empty" );
					bResult = false;
				}

				CSMAPSourceMapDoc::outputDir = outputDir;
			}
			else
			{
				Error( "SMAPDoc: Invalid 'output-dir', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMAPDoc: A source map must have 'output-dir' field" );
			bResult = false;
		}
	}

	// Get entities
	{
		CJsonValue jsonEntitiesVar = jsonDoc.GetValue( "entities" );
		if ( jsonEntitiesVar.IsValid() )
		{
			if ( jsonEntitiesVar.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue> jsonEntitiesArray = jsonEntitiesVar.GetArray();
				for ( uint32 entityIdx = 0, numEntities = (uint32)jsonEntitiesArray.size(); entityIdx < numEntities; ++entityIdx )
				{
					const CJsonValue& jsonEntityVar = jsonEntitiesArray[entityIdx];
					CSMAPEntity		  smapEntity;
					if ( !GrabValueAsEntity( jsonEntityVar, smapEntity ) )
					{
						Error( "SMAPDoc: Invalid entity at id %i", entityIdx );
						bResult = false;
						continue;
					}

					entities.emplace_back( smapEntity );
				}
			}
			else
			{
				Error( "SMAPDoc: Invalid \"entities\", must be array of objects" );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CSMAPSourceMapDoc::GrabValueAsEntity
==================
*/
bool CSMAPSourceMapDoc::GrabValueAsEntity( const CJsonValue& jsonValue, CSMAPEntity& entity ) const
{
	if ( !jsonValue.IsValid() || !jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
	{
		return false;
	}

	bool		bResult	   = true;
	CJsonObject jsonObject = jsonValue.GetObject();

	// Get entity descriptor
	{
		CJsonValue jsonEntityDesc = jsonObject.GetValue( "entity-desc" );
		if ( jsonEntityDesc.IsValid() )
		{
			if ( jsonEntityDesc.IsA( JSONVALUE_TYPE_STRING ) )
			{
				std::string entityDesc = jsonEntityDesc.GetString();
				if ( entityDesc.empty() )
				{
					Error( "SMAPDoc: Invalid 'entity-desc', an entity descriptor can't be empty" );
					bResult = false;
				}

				entity.SetEntityDesc( entityDesc.c_str() );
			}
			else
			{
				Error( "SMAPDoc: Invalid 'entity-desc', must be string type" );
				bResult = false;
			}
		}
		else
		{
			Error( "SMAPDoc: An entity must have 'entity-desc' field" );
			bResult = false;
		}
	}

	// Get name
	{
		CJsonValue jsonName = jsonObject.GetValue( "name" );
		if ( jsonName.IsValid() )
		{
			if ( jsonName.IsA( JSONVALUE_TYPE_STRING ) )
			{
				entity.SetName( jsonName.GetString().c_str() );
			}
			else
			{
				Error( "SMAPDoc: Invalid 'name', must be string type" );
				bResult = false;
			}
		}
	}

	return bResult;
}

/*
==================
CSMAPSourceMapDoc::SaveFile
==================
*/
bool CSMAPSourceMapDoc::SaveFile( const char* pPath )
{
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMAPDoc: Failed to open file '%s' for save a SMAP source map", pPath );
		return false;
	}

	std::string buffer;
	buffer += "{\n";

	// Write an output directory
	buffer += S_Sprintf( "\t\"output-dir\": \"%s\",\n", outputDir.c_str() );

	// Write entities
	if ( !entities.empty() )
	{
		buffer += "\t\"entities\": [\n";
		for ( uint32 entityIdx = 0, numEntities = (uint32)entities.size(); entityIdx < numEntities; ++entityIdx )
		{
			const CSMAPEntity& entity = entities[entityIdx];

			buffer += "\t\t{\n";
			buffer += S_Sprintf( "\t\t\t\"entity-desc\": \"%s\",\n", entity.GetEntityDesc() );
			buffer += S_Sprintf( "\t\t\t\"name\": \"%s\"\n", entity.GetName() );

			if ( entityIdx + 1 < numEntities )
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