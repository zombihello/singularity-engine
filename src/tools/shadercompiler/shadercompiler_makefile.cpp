#include "pch_shadercompiler.h"
#include "tier1/keyvalues.h"
#include "tools/shadercompiler/shadercompiler_makefile.h"

// Reserved vertex factory name. A shader's 'vertexfactory "all"' entry means it uses every vertex factory
// known to the loaded list(s). Forbidden as an actual factory name in a *.list file
#define VERTEXFACTORY_ALL_KEYWORD "all"

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

	// Get vertex factories (optional)
	for ( CKeyValuesSubKeysIterator it( &keyValues, "vertexfactory" ); it; ++it )
	{
		eastl::string vertexFactoryListPath = S_Sprintf( "%s.list", it->GetString( NULL ) );
		if ( !LoadVertexFactoryList( vertexFactoryListPath.c_str(), makeFileDir.c_str() ) )
		{
			Error( "ShaderCompiler: Failed to load vertex factory list '%s' (file: '%s')", vertexFactoryListPath.c_str(), absolutePath.c_str() );
			return false;
		}
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

	// We are done
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
	S_MakeAbsolutePath( keyValues.GetString( "source", "", NULL, &bGotDefaultValue ), shader.source, pBaseDir );
	if ( bGotDefaultValue )
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'source' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get main function
	shader.mainFunc = keyValues.GetString( "main_func", "", NULL, &bGotDefaultValue );
	if ( bGotDefaultValue )
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'main_func' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get shader type
	const char* pType = keyValues.GetString( "type", "", NULL, &bGotDefaultValue );
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
			flag.minValue = it->GetInt( "min", 0, NULL, &bGotDefaultValue );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid flag '%s', not found required field 'min' (file: '%s')", flag.name.c_str(), absolutePath.c_str() );
				return false;
			}

			// Get maximum value
			flag.maxValue = it->GetInt( "max", 0, NULL, &bGotDefaultValue );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid flag '%s', not found required field 'max' (file: '%s')", flag.name.c_str(), absolutePath.c_str() );
				return false;
			}

			// Get default value
			flag.defaultValue = it->GetInt( "default", flag.minValue, NULL, &bGotDefaultValue );
			flag.bSetDefault  = !bGotDefaultValue;

			// Make sure what min and max range is valid. If not we fix it
			if ( flag.minValue > flag.maxValue )
			{
				S_Swap( flag.minValue, flag.maxValue );
			}

			// Make sure what default value in the range
			if ( flag.bSetDefault && ( flag.defaultValue < flag.minValue || flag.defaultValue > flag.maxValue ) )
			{
				Error( "ShaderCompiler: Invalid flag '%s', default value is out of range [%i; %i] (file: '%s')", flag.name.c_str(), flag.minValue, flag.maxValue, absolutePath.c_str() );
				return false;
			}

			// Add a new flag into array
			shader.AddFlag( flag );
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

	// Make sure that the makefile has vertex factories if the shader uses any
	if ( keyValues.FindKey( "vertexfactory" ) && vertexFactories.empty() )
	{
		Error( "ShaderCompiler: Invalid shader, 'vertexfactory' declared but no vertex factory list was loaded (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get vertex factories this shader can be used with (optional)
	for ( CKeyValuesSubKeysIterator it( &keyValues, "vertexfactory" ); it; ++it )
	{
		// Add all known vertex factories if we found a vertex factory named 'all'
		const char* pVertexFactoryName = it->GetString( NULL );
		if ( !S_Stricmp( pVertexFactoryName, VERTEXFACTORY_ALL_KEYWORD ) )
		{
			shader.vertexFactories.resize( vertexFactories.size() );
			for ( uint32 index = 0, count = (uint32)vertexFactories.size(); index < count; ++index )
			{
				shader.vertexFactories[index] = &vertexFactories[index];
			}
			break;
		}

		// Otherwise try to find the vertex factory in the makefile
		const vertexFactory_t* pVertexFactory = NULL;
		for ( uint32 index = 0, count = (uint32)vertexFactories.size(); index < count; ++index )
		{
			const vertexFactory_t& curVertexFactory = vertexFactories[index];
			if ( !S_Stricmp( curVertexFactory.name.c_str(), pVertexFactoryName ) )
			{
				pVertexFactory = &curVertexFactory;
				break;
			}
		}

		// Make sure that we found the vertex factory and add it into the array
		if ( !pVertexFactory )
		{
			Error( "ShaderCompiler: Unknown vertex factory '%s' (file: '%s')", pVertexFactoryName, absolutePath.c_str() );
			return false;
		}
		shader.vertexFactories.emplace_back( pVertexFactory );
	}

	// Add a system flag covering the declared vertex factories
	if ( !shader.vertexFactories.empty() )
	{
		shaderFlag_t systemFlag;
		systemFlag.name			= ConvShaderSystemFlagToString( SHADER_SYSTEM_FLAG_VERTEXFACTORY );
		systemFlag.minValue		= 0;
		systemFlag.maxValue		= (int32)shader.vertexFactories.size() - 1;
		systemFlag.bSetDefault	= false;
		systemFlag.defaultValue = 0;
		shader.AddSystemFlag( systemFlag, SHADER_SYSTEM_FLAG_VERTEXFACTORY );
	}

	// Calculate number of combo flags and add the shader into the array
	shader.CalcNumFlagCombos();
	shaders.emplace_back( shader );
	Msg( "ShaderCompiler: Loaded shader '%s'", absolutePath.c_str() );
	return true;
}

/*
==================
CShaderCompilerMakeFile::LoadVertexFactoryList
==================
*/
bool CShaderCompilerMakeFile::LoadVertexFactoryList( const char* pPath, const char* pBaseDir /* = "" */ )
{
	// Load key values file
	eastl::string absolutePath;
	CKeyValues	  keyValues( "vertexfactorylist" );
	S_MakeAbsolutePath( pPath, absolutePath, pBaseDir );
	if ( !keyValues.LoadFromFile( absolutePath.c_str() ) )
	{
		Error( "ShaderCompiler: Failed to load vertex factory list '%s'", absolutePath.c_str() );
		return false;
	}

	// Get path to directory with the vertex factory list, source file are relative to it
	eastl::string vertexFactoryListDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( absolutePath.c_str(), tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, vertexFactoryListDir, "", false );
	}

	// Get vertex factories
	CKeyValues* pVertexFactories = keyValues.FindKey( "vertexfactories" );
	if ( pVertexFactories )
	{
		for ( CKeyValuesSubKeysIterator it( pVertexFactories, false, true ); it; ++it )
		{
			// Make sure that the vertex factory does not use reserved 'all'
			const char* pVertexFactoryName = it->GetName();
			if ( !S_Stricmp( pVertexFactoryName, VERTEXFACTORY_ALL_KEYWORD ) )
			{
				Error( "ShaderCompiler: Invalid vertex factory name '" VERTEXFACTORY_ALL_KEYWORD "', name is reserved (file: '%s')", absolutePath.c_str() );
				return false;
			}

			// Try to find an existing vertex factory in the array or add a new one
			vertexFactory_t* pVertexFactory = NULL;
			auto			 itFind			= vertexFactoryDict.find( pVertexFactoryName );
			if ( itFind != vertexFactoryDict.end() )
			{
				pVertexFactory = &vertexFactories[itFind->second];
				Warning( "ShaderCompiler: Overriding vertex factory '%s' (file: '%s')", pVertexFactoryName, absolutePath.c_str() );
			}
			else
			{
				uint32 vertexFactoryIndex = (uint32)vertexFactories.size();
				pVertexFactory			  = &vertexFactories.emplace_back();
				pVertexFactory->name	  = pVertexFactoryName;
				vertexFactoryDict.insert( eastl::make_pair( pVertexFactoryName, vertexFactoryIndex ) );
			}

			// Get an index and make sure that it is
			bool bGotDefaultValue = false;
			pVertexFactory->index = it->GetInt( "index", 0, NULL, &bGotDefaultValue );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid vertex factory '%s', not found required field 'index' (file: '%s')", pVertexFactoryName, absolutePath.c_str() );
				return false;
			}

			// Get a source and make sure that it is
			S_MakeAbsolutePath( it->GetString( "source", "", NULL, &bGotDefaultValue ), pVertexFactory->source, vertexFactoryListDir.c_str() );
			if ( bGotDefaultValue )
			{
				Error( "ShaderCompiler: Invalid vertex factory '%s', not found required field 'source' (file: '%s')", pVertexFactoryName, absolutePath.c_str() );
				return false;
			}
		}
	}

	// We are done
	Msg( "ShaderCompiler: Loaded vertex factory list '%s'", absolutePath.c_str() );
	return true;
}
