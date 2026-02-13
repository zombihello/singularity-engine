#include "pch_shadercompiler.h"
#include "tier1/keyvalues.h"
#include "tools/shadercompiler/shadercompiler_makefile.h"

/*
==================
CShaderCompilerMakeFile::LoadMakeFile
==================
*/
bool CShaderCompilerMakeFile::LoadMakeFile( const char* pPath, const char* pBaseDir /* = "" */ )
{
	// Load key values
	eastl::string absolutePath;
	CKeyValues	  keyValues( "makefile" );
	S_MakeAbsolutePath( pPath, absolutePath, pBaseDir );
	if ( !keyValues.LoadFromFile( absolutePath.c_str() ) )
	{
		Error( "ShaderCompiler: Failed to load makefile '%s'", absolutePath.c_str() );
		return false;
	}

	// Get path to directory with the main makefile
	eastl::string makeFileDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( absolutePath.c_str(), tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, makeFileDir, "", false );
	}

	// Get include directories
	for ( CKeyValuesSubKeysIterator it( &keyValues, "include" ); it; ++it )
	{
		AddIncludeDir( it->GetString( NULL ), makeFileDir.c_str() );
	}

	// Get shaders to compile
	for ( CKeyValuesSubKeysIterator it( &keyValues, "shader" ); it; ++it )
	{
		eastl::string shaderPath = S_Sprintf( "%s.shader", it->GetString( NULL ) );
		if ( !LoadShader( shaderPath.c_str(), makeFileDir.c_str() ) )
		{
			Error( "ShaderCompiler: Failed to load shader '%s' (file: '%s')", shaderPath.c_str(), absolutePath.c_str() );
			return false;
		}
	}

	Msg( "ShaderCompiler: Loaded makefile '%s'", absolutePath.c_str() );
	return true;
}

/*
==================
CShaderCompilerMakeFile::LoadShader
==================
*/
bool CShaderCompilerMakeFile::LoadShader( const char* pPath, const char* pBaseDir /* = "" */ )
{
	// Load key values file
	shader_t	  shader;
	eastl::string absolutePath;
	CKeyValues	  keyValues( "shader" );
	S_MakeAbsolutePath( pPath, absolutePath, pBaseDir );
	if ( !keyValues.LoadFromFile( absolutePath.c_str() ) )
	{
		Error( "ShaderCompiler: Failed to load shader '%s'", absolutePath.c_str() );
		return false;
	}

	// Get source file
	bool bGotDefaultValue = false;
	S_MakeAbsolutePath( keyValues.GetString( "source", "", &bGotDefaultValue ), shader.source, pBaseDir );
	if ( bGotDefaultValue )
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'source' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get main function
	shader.mainFunc = keyValues.GetString( "main_func", "", &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'main_func' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get shader type
	const char* pType = keyValues.GetString( "type", "", &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'type' (file: '%s')", absolutePath.c_str() );
		return false;
	}
	if ( !ConvStringToShaderType( pType, shader.type ) )
	{
		Error( "ShaderCompiler: Unknown shader type '%s' (file: '%s')", pType, absolutePath.c_str() );
		return false;
	}

	// Get flags
	CKeyValues* pFlags = keyValues.FindKey( "flags" );
	if ( pFlags )
	{
		for ( CKeyValuesSubKeysIterator it( pFlags, false, true ); it; ++it )
		{
			// Get flag name
			shaderFlag_t flag;
			flag.name = it->GetName();

			// Get minimum value
			flag.minValue = it->GetInt( "min", 0, &bGotDefaultValue );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid flag '%s', not found required field 'min' (file: '%s')", flag.name.c_str(), absolutePath.c_str() );
				return false;
			}

			// Get maximum value
			flag.maxValue = it->GetInt( "max", 0, &bGotDefaultValue );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid flag '%s', not found required field 'max' (file: '%s')", flag.name.c_str(), absolutePath.c_str() );
				return false;
			}

			// Get default value
			flag.defaultValue = it->GetInt( "default", flag.minValue, &bGotDefaultValue );
			flag.bSetDefault  = !bGotDefaultValue;

			// Make sure what min and max range is valid. If not we fix it
			if ( flag.minValue > flag.maxValue )
			{
				Swap( flag.minValue, flag.maxValue );
			}

			// Make sure what default value in the range
			if ( flag.bSetDefault && ( flag.defaultValue < flag.minValue || flag.defaultValue > flag.maxValue ) )
			{
				Error( "ShaderCompiler: Invalid flag '%s', default value is out of range [%i; %i] (file: '%s')", flag.name.c_str(), flag.minValue, flag.maxValue, absolutePath.c_str() );
				return false;
			}

			// Add a new flag into array
			shader.flags.emplace_back( flag );
		}
	}

	// Get defines
	CKeyValues* pDefines = keyValues.FindKey( "defines" );
	if ( pDefines )
	{
		for ( CKeyValuesSubKeysIterator it( pDefines ); it; ++it )
		{
			shader.defines.emplace_back( shaderDefine_t{ it->GetName(), it->GetString( NULL ) } );
		}
	}

	shader.CalcNumFlagCombos();
	shaders.emplace_back( shader );
	Msg( "ShaderCompiler: Loaded shader '%s'", absolutePath.c_str() );
	return true;
}
