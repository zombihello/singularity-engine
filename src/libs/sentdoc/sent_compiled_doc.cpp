#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "sentdoc/sent_compiled_doc.h"

// Singularity entity magic
static const uint32 s_SENTMagicSize				 = 5;
static const char	s_SENTMagic[s_SENTMagicSize] = { 'S', 'E', 'N', 'T', 'C' };	 // Singularity Entity Descriptor Compiled
static_assert( sizeof( s_SENTMagic ) == s_SENTMagicSize, "Size of s_SENTMagic must be equal to s_SENTMagicSize" );

// Singularity entity version
static const uint32 s_SENTVersion = 1;

/*
==================
CSENTCompiledEntityDescDoc::SaveFile
==================
*/
bool CSENTCompiledEntityDescDoc::SaveFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter> pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SENTDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( (void*)s_SENTMagic, s_SENTMagicSize );
	pFile->Write( (void*)&s_SENTVersion, sizeof( uint32 ) );

	// Write components
	uint32 numComponents = (uint32)components.size();
	pFile->Write( &numComponents, sizeof( uint32 ) );
	for ( uint32 componentIdx = 0; componentIdx < numComponents; ++componentIdx )
	{
		const CSENTEntityDescComponent& component = components[componentIdx];

		// Write the component type
		uint32 componentTypeSize = S_Strlen( component.GetType() );
		pFile->Write( &componentTypeSize, sizeof( uint32 ) );
		pFile->Write( (void*)component.GetType(), componentTypeSize );

		// Write component variables
		const std::vector<CSENTEntityDescVar>& vars	   = component.GetVars();
		uint32								   numVars = (uint32)vars.size();
		pFile->Write( &numVars, sizeof( uint32 ) );
		for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
		{
			const CSENTEntityDescVar& var = vars[varIdx];

			// Write the variable name
			uint32 varNameSize = S_Strlen( var.GetName() );
			pFile->Write( &varNameSize, sizeof( uint32 ) );
			pFile->Write( (void*)var.GetName(), varNameSize );

			// Write the variable value
			sentEntityDescVarType_t varType = var.GetType();
			pFile->Write( &varType, sizeof( sentEntityDescVarType_t ) );
			switch ( varType )
			{
			case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
			{
				bool bValue = var.GetBoolValue();
				pFile->Write( &bValue, sizeof( bool ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_INT:
			{
				int32 value = var.GetIntValue();
				pFile->Write( &value, sizeof( int32 ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
			{
				float value = var.GetFloatValue();
				pFile->Write( &value, sizeof( float ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
			{
				vec2_t value = var.GetVec2Value();
				pFile->Write( &value, sizeof( vec2_t ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
			{
				vec3_t value = var.GetVec3Value();
				pFile->Write( &value, sizeof( vec3_t ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
			{
				vec4_t value = var.GetVec4Value();
				pFile->Write( &value, sizeof( vec4_t ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
			{
				matrix_t value = var.GetMatrixValue();
				pFile->Write( &value, sizeof( matrix_t ) );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_STRING:
			{
				uint32 valueStringSize = S_Strlen( var.GetStringValue() );
				pFile->Write( &valueStringSize, sizeof( uint32 ) );
				pFile->Write( (void*)var.GetStringValue(), valueStringSize );
				break;
			}

			default:
				Warning( "SENTDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
				Assert( false );
				break;
			}
		}
	}

	return true;
}

/*
==================
CSENTCompiledEntityDescDoc::LoadFromFile
==================
*/
bool CSENTCompiledEntityDescDoc::LoadFromFile( const char* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataReader> pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "SENTDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	char  magic[s_SENTMagicSize];
	uint32 version = 0;
	pFile->Read( magic, s_SENTMagicSize );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( S_Strncmp( magic, s_SENTMagic, s_SENTMagicSize ) )
	{
		Error( "SENTDoc: Invalid magic value in '%s'. Expected '%.*s', received '%.*s'", pPath, s_SENTMagicSize, s_SENTMagic, s_SENTMagicSize, magic );
		return false;
	}

	if ( version != s_SENTVersion )
	{
		Error( "SENTDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, s_SENTVersion, version );
		return false;
	}
	Clear();

	// Read components
	uint32 numComponents = 0;
	pFile->Read( &numComponents, sizeof( uint32 ) );
	components.resize( numComponents );
	for ( uint32 componentIdx = 0; componentIdx < numComponents; ++componentIdx )
	{
		CSENTEntityDescComponent& component = components[componentIdx];

		// Read a component type
		std::string componentType;
		uint32		componentTypeSize = 0;
		pFile->Read( &componentTypeSize, sizeof( uint32 ) );
		componentType.resize( componentTypeSize );
		pFile->Read( componentType.data(), componentTypeSize );
		component.SetType( componentType.c_str() );

		// Read component variables
		uint32 numVars = 0;
		pFile->Read( &numVars, sizeof( uint32 ) );
		for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
		{
			CSENTEntityDescVar var;

			// Read the variable name
			std::string varName;
			uint32		varNameSize = 0;
			pFile->Read( &varNameSize, sizeof( uint32 ) );
			varName.resize( varNameSize );
			pFile->Read( varName.data(), varNameSize );
			var.SetName( varName.c_str() );

			// Read the variable value
			sentEntityDescVarType_t varType = SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED;
			pFile->Read( &varType, sizeof( sentEntityDescVarType_t ) );
			switch ( varType )
			{
			case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
			{
				bool bValue;
				pFile->Read( &bValue, sizeof( bool ) );
				var.SetBoolValue( bValue );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_INT:
			{
				int32 value;
				pFile->Read( &value, sizeof( int32 ) );
				var.SetIntValue( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
			{
				float value;
				pFile->Read( &value, sizeof( float ) );
				var.SetFloatValue( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
			{
				vec2_t value;
				pFile->Read( &value, sizeof( vec2_t ) );
				var.SetVec2Value( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
			{
				vec3_t value;
				pFile->Read( &value, sizeof( vec3_t ) );
				var.SetVec3Value( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
			{
				vec4_t value;
				pFile->Read( &value, sizeof( vec4_t ) );
				var.SetVec4Value( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
			{
				matrix_t value;
				pFile->Read( &value, sizeof( matrix_t ) );
				var.SetMatrixValue( value );
				break;
			}

			case SENT_ENTITY_DESC_VAR_TYPE_STRING:
			{
				std::string valueString;
				uint32		valueStringSize;
				pFile->Read( &valueStringSize, sizeof( uint32 ) );
				valueString.resize( valueStringSize );
				pFile->Read( valueString.data(), valueStringSize );
				var.SetStringValue( valueString.c_str() );
				break;
			}

			default:
				Warning( "SENTDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
				Assert( false );
				break;
			}

			component.AddVar( var );
		}
	}

	return true;
}