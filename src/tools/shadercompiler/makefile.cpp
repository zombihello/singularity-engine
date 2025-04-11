#include "pch_shadercompiler.h"
#include "stdlib/jsondoc.h"
#include "tools/shadercompiler/makefile.h"

/**
 * @ingroup shadercompiler
 * @brief Default output path for generated C++ classes
 */
#define DEFAULT_CPPGEN_OUTPUT	"generated/"


/*
==================
CMakeFile::CMakeFile
==================
*/
CMakeFile::CMakeFile()
{
	Clear();
}

/*
==================
CMakeFile::LoadMakeFile
==================
*/
bool CMakeFile::LoadMakeFile( const achar* pPath )
{
	Msg( "ShaderCompiler: Load makefile '%s'", pPath );

	// Load JSON file
	CJsonDoc	jsonMakeFile;
	if ( !jsonMakeFile.LoadFromFile( pPath ) )
	{
		Warning( "ShaderCompiler: Failed to load, maybe wrong JSON syntax?" );
		return false;
	}
	bool			bResult = true;

	// Get path to directory with the main makefile
	{
		std::string		tmpBuffer;
		S_GetFilePath( pPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, makeFileDir, "", false );
	}

	// Get shader list name
	CJsonValue		jsonShaderListName = jsonMakeFile.GetValue( "name" );
	if ( jsonShaderListName.IsValid() )
	{
		if ( !LoadShaderListName( jsonShaderListName, shaderListName ) )
		{
			Warning( "ShaderCompiler: Failed to shader list name, invalid 'name'" );
			bResult = false;
		}
	}
	else
	{
		Warning( "ShaderCompiler: Shaders makefile must have 'name'" );
		bResult = false;
	}

	// Get output path for generated C++ classes
	CJsonValue		jsonCppGenOutput = jsonMakeFile.GetValue( "cppgen_output" );
	if ( jsonCppGenOutput.IsValid() )
	{
		if ( !LoadCppGenOutput( jsonCppGenOutput, cppGenOutput ) )
		{
			Warning( "ShaderCompiler: Failed to load C++ generate output, invalid 'cppgen_output'" );
			bResult = false;
		}
	}
	else
	{
		cppGenOutput = DEFAULT_CPPGEN_OUTPUT;
	}

	// Get output path for compiled shader cache
	CJsonValue		jsonOutput = jsonMakeFile.GetValue( "output" );
	if ( jsonOutput.IsValid() )
	{
		if ( !LoadCacheOutputDir( jsonOutput, cacheOutputDir ) )
		{
			Warning( "ShaderCompiler: Failed to load shader cache output, invalid 'output'" );
			bResult = false;
		}
	}
	else
	{
		Warning( "ShaderCompiler: Shaders makefile must have 'output'" );
		bResult = false;
	}

	// Convert C++ generated output to absolute 
	{
		std::string		tmpBuffer = cppGenOutput;
		S_MakeAbsolutePath( tmpBuffer, cppGenOutput, makeFileDir );
	}

	// Get shader's include directories
	{
		CJsonValue		jsonInclude = jsonMakeFile.GetValue( "include" );
		if ( jsonInclude.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonInclude.GetArray();
			if ( !jsonInclude.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Warning( "ShaderCompiler: Invalid 'include', must be array of strings" );
				bResult = false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue&	jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Warning( "ShaderCompiler: Invalid 'include[%i]', must be string", index );
						bResult = false;
						continue;
					}

					// Convert of the include path to absolute and emplace at back
					std::string		absoluteInclude;
					S_MakeAbsolutePath( jsonElement.GetString(), absoluteInclude, makeFileDir );
					shaderIncludeDirs.emplace_back( absoluteInclude );
				}
			}
		}
	}

	// Get shaders to compile
	{
		CJsonValue		jsonShaders = jsonMakeFile.GetValue( "shaders" );
		if ( jsonShaders.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonShaders.GetArray();
			if ( !jsonShaders.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Warning( "ShaderCompiler: Invalid 'shaders', must be array of objects" );
				bResult = false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue&	jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
					{
						Warning( "ShaderCompiler: Invalid 'shaders[%i]', must be object with required fields: 'source', 'main_func' and 'type'", index );
						bResult = false;
						continue;
					}

					// Load shader from the JSON object
					shader_t			shader;
					if ( !LoadShader( jsonElement.GetObject(), shader ) )
					{
						Warning( "ShaderCompiler: Failed to load shader from 'shaders[%i]'", index );
						bResult = false;
					}

					// Convert source path to absolute
					std::string		tmpBuffer = shader.source;
					S_MakeAbsolutePath( tmpBuffer, shader.source, makeFileDir );

					// Calculate maximum number of flag combinations
					shader.CalcNumFlagCombos();

					// Add a new shader into array
					shaders.emplace_back( shader );
				}
			}
		}
	}

	return bResult;
}

/*
==================
CMakeFile::LoadCppGenOutput
==================
*/
bool CMakeFile::LoadCppGenOutput( const CJsonValue& jsonValue, std::string& cppGenOutput )
{
	bool	bResult = true;
	cppGenOutput = jsonValue.GetString( DEFAULT_CPPGEN_OUTPUT );
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "ShaderCompiler: Invalid C++ generate output, must be string" );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadCacheOutputDir
==================
*/
bool CMakeFile::LoadCacheOutputDir( const CJsonValue& jsonValue, std::string& cacheOutputDir )
{
	bool			bResult = true;
	cacheOutputDir = jsonValue.GetString();
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "ShaderCompiler: Invalid output directory for compiled shader cache, must be string" );
		bResult = false;
	}

	if ( cacheOutputDir.empty() )
	{
		Warning( "ShaderCompiler: Invalid output directory for compiled shader cache, path can't be empty" );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadShaderListName
==================
*/
bool CMakeFile::LoadShaderListName( const CJsonValue& jsonValue, std::string& shaderListName )
{
	bool			bResult = true;
	shaderListName	= jsonValue.GetString();
	if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
	{
		Warning( "ShaderCompiler: Invalid shader list name, must be string" );
		bResult = false;
	}

	if ( shaderListName.empty() )
	{
		Warning( "ShaderCompiler: Invalid shader list name, the name can't be empty" );
		bResult = false;
	}

	return bResult;
}

/*
==================
CMakeFile::LoadDefines
==================
*/
bool CMakeFile::LoadDefines( const std::vector<CJsonValue>& jsonArray, std::vector<shaderDefine_t>& defines )
{
	bool	bResult = true;
	for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
	{
		const CJsonValue&	jsonElement = jsonArray[index];
		if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
		{
			Warning( "ShaderCompiler: Invalid define at %i, must be object with required fields: 'name' and 'value'", index );
			bResult = false;
			continue;
		}

		CJsonObject		jsonObject = jsonElement.GetObject();
		shaderDefine_t	define;

		// Get define name
		CJsonValue		jsonName = jsonObject.GetValue( "name" );
		if ( jsonName.IsValid() )
		{
			if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "ShaderCompiler: Invalid 'name' of define at %i', must be string", index );
				bResult = false;
			}
			else
			{
				define.name = jsonName.GetString();
			}
		}
		else
		{
			Warning( "ShaderCompiler: Invalid define at %i, not found required field 'name'", index );
			bResult = false;
		}

		// Get define value
		CJsonValue		jsonValue = jsonObject.GetValue( "value" );
		if ( jsonValue.IsValid() )
		{
			if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "ShaderCompiler: Invalid 'value' of define at %i', must be string", index );
				bResult = false;
			}
			else
			{
				define.value = jsonValue.GetString();
			}
		}
		else
		{
			Warning( "ShaderCompiler: Invalid define at %i, not found required field 'value'", index );
			bResult = false;
		}

		// Add a new define into array
		defines.emplace_back( define );
	}

	return bResult;
}

/*
==================
CMakeFile::LoadShader
==================
*/
bool CMakeFile::LoadShader( const CJsonObject& jsonObject, shader_t& shader )
{
	bool	bResult = true;

	// Get source file
	CJsonValue	jsonSource = jsonObject.GetValue( "source" );
	if ( jsonSource.IsValid() )
	{
		if ( !jsonSource.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "ShaderCompiler: Invalid 'source', must be string" );
			bResult = false;
		}
		else
		{
			shader.source = jsonSource.GetString();
		}
	}
	else
	{
		Warning( "ShaderCompiler: Invalid shader, not found required field 'source'" );
		bResult = false;
	}

	// Get main function
	CJsonValue	jsonMainFunc = jsonObject.GetValue( "main_func" );
	if ( jsonMainFunc.IsValid() )
	{
		if ( !jsonMainFunc.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "ShaderCompiler: Invalid 'main_func', must be string" );
			bResult = false;
		}
		else
		{
			shader.mainFunc = jsonMainFunc.GetString();
		}
	}
	else
	{
		Warning( "ShaderCompiler: Invalid shader, not found required field 'main_func'" );
		bResult = false;
	}

	// Get shader type
	CJsonValue	jsonType = jsonObject.GetValue( "type" );
	if ( jsonType.IsValid() )
	{
		if ( !jsonType.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Warning( "ShaderCompiler: Invalid 'type', must be string" );
			bResult = false;
		}
		else if ( !ConvStringToShaderType( jsonType.GetString().c_str(), shader.type ) )
		{
			Warning( "ShaderCompiler: Unknown shader type '%s'", jsonType.GetString().c_str() );
			bResult = false;
		}
	}
	else
	{
		Warning( "ShaderCompiler: Invalid shader, not found required field 'type'" );
		bResult = false;
	}

	// Get flags
	CJsonValue		jsonFlags = jsonObject.GetValue( "flags" );
	if ( jsonFlags.IsValid() )
	{
		if ( !jsonFlags.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "ShaderCompiler: Invalid 'flags', must be array of objects" );
			bResult = false;
		}
		else if ( !LoadFlags( jsonFlags.GetArray(), shader.flags ) )
		{
			Warning( "ShaderCompiler: Failed to load flags from 'flags'" );
			bResult = false;
		}
	}

	// Get defines
	CJsonValue		jsonDefines = jsonObject.GetValue( "defines" );
	if ( jsonDefines.IsValid() )
	{
		if ( !jsonDefines.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Warning( "ShaderCompiler: Invalid 'defines', must be array of objects" );
			bResult = false;
		}
		else if ( !LoadDefines( jsonDefines.GetArray(), shader.defines ) )
		{
			Warning( "ShaderCompiler: Failed to load defines from 'defines'" );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CMakeFile::LoadFlags
==================
*/
bool CMakeFile::LoadFlags( const std::vector<CJsonValue>& jsonArray, std::vector<shaderFlag_t>& flags )
{
	bool	bResult = true;
	for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
	{
		const CJsonValue&	jsonElement = jsonArray[index];
		if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
		{
			Warning( "ShaderCompiler: Invalid flag at %i, must be object with required fields: 'name', 'min' and 'max'", index );
			bResult = false;
			continue;
		}

		CJsonObject		jsonObject = jsonElement.GetObject();
		shaderFlag_t		flag;

		// Get flag name
		CJsonValue		jsonName = jsonObject.GetValue( "name" );
		if ( jsonName.IsValid() )
		{
			if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "ShaderCompiler: Invalid 'name' of flag at %i', must be string", index );
				bResult = false;
			}
			else
			{
				flag.name = jsonName.GetString();
			}
		}
		else
		{
			Warning( "ShaderCompiler: Invalid flag at %i, not found required field 'name'", index );
			bResult = false;
		}

		// Get minimum value
		CJsonValue		jsonMin = jsonObject.GetValue( "min" );
		if ( jsonMin.IsValid() )
		{
			if ( !jsonMin.IsA( JSONVALUE_TYPE_INT) )
			{
				Warning( "ShaderCompiler: Invalid 'min' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.minValue = jsonMin.GetInt();
			}
		}
		else
		{
			Warning( "ShaderCompiler: Invalid flag at %i, not found required field 'min'", index );
			bResult = false;
		}

		// Get maximum value
		CJsonValue		jsonMax = jsonObject.GetValue( "max" );
		if ( jsonMax.IsValid() )
		{
			if ( !jsonMax.IsA( JSONVALUE_TYPE_INT ) )
			{
				Warning( "ShaderCompiler: Invalid 'max' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.maxValue = jsonMax.GetInt();
			}
		}
		else
		{
			Warning( "ShaderCompiler: Invalid flag at %i, not found required field 'max'", index );
			bResult = false;
		}

		// Get default value
		CJsonValue		jsonDefault = jsonObject.GetValue( "default" );
		if ( jsonDefault.IsValid() )
		{
			if ( !jsonDefault.IsA( JSONVALUE_TYPE_INT ) )
			{
				Warning( "ShaderCompiler: Invalid 'default' of flag at %i', must be int", index );
				bResult = false;
			}
			else
			{
				flag.bSetDefault	= true;
				flag.defaultValue	= jsonDefault.GetInt();
			}
		}

		// Make sure what min and max range is valid. If not we fix it
		if ( flag.minValue > flag.maxValue )
		{
			Swap( flag.minValue, flag.maxValue );
		}

		// Make sure what default value in the range
		if ( flag.bSetDefault && ( flag.defaultValue < flag.minValue || flag.defaultValue > flag.maxValue ) )
		{
			Warning( "ShaderCompiler: Invalid default value of flag at %i, out of range [%i; %i]", index, flag.minValue, flag.maxValue );
			bResult = false;
		}

		// Add a new flag into array
		flags.emplace_back( flag );
	}

	return bResult;
}

/*
==================
CMakeFile::Clear
==================
*/
void CMakeFile::Clear()
{
	cppGenOutput = DEFAULT_CPPGEN_OUTPUT;
	cacheOutputDir.clear();
	makeFileDir.clear();
	shaderIncludeDirs.clear();
	shaders.clear();
}