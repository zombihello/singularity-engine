#include "interfaces/interfaces.h"
#include "core/profile.h"
#include "filesystem/ifilesystem.h"
#include "smatdoc/smat_compiled_doc.h"

// Singularity material magic
static const uint32		s_SMATMagicSize					= 5;
static const achar		s_SMATMagic[s_SMATMagicSize]	= { 'S', 'M', 'A', 'T', 'C' };		// Singularity Material Compiled
static_assert( sizeof( s_SMATMagic ) == s_SMATMagicSize, "Size of s_SMATMagic must be equal to s_SMATMagicSize" );

// Singularity material version
static const uint32		s_SMATVersion					= 1;


/*
==================
CSMATCompiledMaterialDoc::SaveFile
==================
*/
bool CSMATCompiledMaterialDoc::SaveFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataWriter>	pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Error( "SMATDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Write format magic and version
	pFile->Write( ( void* )s_SMATMagic, s_SMATMagicSize );
	pFile->Write( ( void* )&s_SMATVersion, sizeof( uint32 ) );

	// Write shader name
	uint32		shanderNameSize = ( uint32 )shaderName.size();
	pFile->Write( &shanderNameSize, sizeof( uint32 ) );
	pFile->Write( shaderName.data(), shanderNameSize );

	// Write variables
	uint32		numVars = ( uint32 )vars.size();
	pFile->Write( &numVars, sizeof( uint32 ) );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		const CSMATMaterialVar&		var	= vars[varIdx];

		// Write the variable name
		uint32		varNameSize = S_Strlen( var.GetName() );
		pFile->Write( &varNameSize, sizeof( uint32 ) );
		pFile->Write( ( void* )var.GetName(), varNameSize );
		
		// Write the variable value
		smatMaterialVarType_t	varType = var.GetType();
		pFile->Write( &varType, sizeof( smatMaterialVarType_t ) );
		switch ( varType )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL:
		{
			bool	bValue = var.GetBoolValue();
			pFile->Write( &bValue, sizeof( bool ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_INT:
		{
			int32	value = var.GetIntValue();
			pFile->Write( &value, sizeof( int32 ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_FLOAT:
		{
			float	value = var.GetFloatValue();
			pFile->Write( &value, sizeof( float ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
		{
			vec2_t	value = { 0.f, 0.f };
			var.GetVecValue( &value.x, 2 );
			pFile->Write( &value, sizeof( vec2_t ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
		{
			vec3_t	value = { 0.f, 0.f, 0.f };
			var.GetVecValue( &value.x, 3 );
			pFile->Write( &value, sizeof( vec3_t ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
		{
			vec4_t	value = { 0.f, 0.f, 0.f, 0.f };
			var.GetVecValue( &value.x, 4 );
			pFile->Write( &value, sizeof( vec4_t ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_MATRIX:
		{
			matrix_t	value = var.GetMatrixValue();
			pFile->Write( &value, sizeof( matrix_t ) );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_STRING:
		{
			uint32		valueStringSize = S_Strlen( var.GetStringValue() );
			pFile->Write( &valueStringSize, sizeof( uint32 ) );
			pFile->Write( ( void* )var.GetStringValue(), valueStringSize );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
		{
			uint32		valueTextureSize = S_Strlen( var.GetTextureValue() );
			pFile->Write( &valueTextureSize, sizeof( uint32 ) );
			pFile->Write( ( void* )var.GetTextureValue(), valueTextureSize );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
		{
			uint32		valueMaterialSize = S_Strlen( var.GetMaterialValue() );
			pFile->Write( &valueMaterialSize, sizeof( uint32 ) );
			pFile->Write( ( void* )var.GetMaterialValue(), valueMaterialSize );
			break;
		}

		default:
			Warning( "SMATDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
			Assert( false );
			break;
		}
	}

	return true;
}

/*
==================
CSMATCompiledMaterialDoc::LoadFromFile
==================
*/
bool CSMATCompiledMaterialDoc::LoadFromFile( const achar* pPath )
{
	// Do nothing if the file system isn't valid
	PROFILE_SCOPE( PROFILE_SCOPE_GROUP_IO );
	Assert( g_pFileSystem );

	// Try to open a file
	TRefPtr<IStreamDataReader>	pFile = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFile )
	{
		Error( "SMATDoc: Failed to open file '%s'", pPath );
		return false;
	}

	// Read format magic and version
	achar	magic[s_SMATMagicSize];
	uint32	version = 0;
	pFile->Read( magic, s_SMATMagicSize );
	pFile->Read( &version, sizeof( uint32 ) );
	if ( S_Strncmp( magic, s_SMATMagic, s_SMATMagicSize ) )
	{
		Error( "SMATDoc: Invalid magic value in '%s'. Expected '%.*s', received '%.*s'", pPath, s_SMATMagicSize, s_SMATMagic, s_SMATMagicSize, magic );
		return false;
	}

	if ( version != s_SMATVersion )
	{
		Error( "SMATDoc: Invalid version format in '%s'. Expected %i, received %i", pPath, s_SMATVersion, version );
		return false;
	}
	Clear();

	// Read shader name
	uint32		shanderNameSize = 0;
	pFile->Read( &shanderNameSize, sizeof( uint32 ) );
	shaderName.resize( shanderNameSize );
	pFile->Read( shaderName.data(), shanderNameSize );

	// Read variables
	uint32		numVars = 0;
	pFile->Read( &numVars, sizeof( uint32 ) );
	vars.resize( numVars );
	for ( uint32 varIdx = 0; varIdx < numVars; ++varIdx )
	{
		CSMATMaterialVar&		var = vars[varIdx];

		// Read the variable name
		std::string	varName;
		uint32		varNameSize = 0;
		pFile->Read( &varNameSize, sizeof( uint32 ) );
		varName.resize( varNameSize );
		pFile->Read( varName.data(), varNameSize );
		var.SetName( varName.c_str() );

		// Read the variable value
		smatMaterialVarType_t	varType = SMAT_MATERIAL_VAR_TYPE_UNDEFINED;
		pFile->Read( &varType, sizeof( smatMaterialVarType_t ) );
		switch ( varType )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL:
		{
			bool	bValue;
			pFile->Read( &bValue, sizeof( bool ) );
			var.SetBoolValue( bValue );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_INT:
		{
			int32	value;
			pFile->Read( &value, sizeof( int32 ) );
			var.SetIntValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_FLOAT:
		{
			float	value;
			pFile->Read( &value, sizeof( float ) );
			var.SetFloatValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
		{
			vec2_t	value;
			pFile->Read( &value, sizeof( vec2_t ) );
			var.SetVecValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
		{
			vec3_t	value;
			pFile->Read( &value, sizeof( vec3_t ) );
			var.SetVecValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
		{
			vec4_t	value;
			pFile->Read( &value, sizeof( vec4_t ) );
			var.SetVecValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_MATRIX:
		{
			matrix_t	value;
			pFile->Read( &value, sizeof( matrix_t ) );
			var.SetMatrixValue( value );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_STRING:
		{
			std::string		valueString;
			uint32			valueStringSize;
			pFile->Read( &valueStringSize, sizeof( uint32 ) );
			valueString.resize( valueStringSize );
			pFile->Read( valueString.data(), valueStringSize );
			var.SetStringValue( valueString.c_str() );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
		{
			std::string		valueTexture;
			uint32			valueTextureSize;
			pFile->Read( &valueTextureSize, sizeof( uint32 ) );
			valueTexture.resize( valueTextureSize );
			pFile->Read( valueTexture.data(), valueTextureSize );
			var.SetTextureValue( valueTexture.c_str() );
			break;
		}

		case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
		{
			std::string		valueMaterial;
			uint32			valueMaterialSize = S_Strlen( var.GetMaterialValue() );
			pFile->Read( &valueMaterialSize, sizeof( uint32 ) );
			valueMaterial.resize( valueMaterialSize );
			pFile->Read( valueMaterial.data(), valueMaterialSize );
			var.SetMaterialValue( valueMaterial.c_str() );
			break;
		}

		default:
			Warning( "SMATDoc: Unknown type 0x%X in variable '%s'", var.GetType(), var.GetName() );
			Assert( false );
			break;
		}
	}

	return true;
}