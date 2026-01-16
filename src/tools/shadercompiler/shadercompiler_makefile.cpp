#include "pch_shadercompiler.h"
#include "tools/shadercompiler/shadercompiler_makefile.h"

/*
==================
CShaderCompilerMakeFile::LoadMakeFile
==================
*/
bool CShaderCompilerMakeFile::LoadMakeFile( const achar* pPath, const achar* pBaseDir /* = "" */ )
{
	// Load JSON file
	std::string		absolutePath;
	S_MakeAbsolutePath( pPath, absolutePath, pBaseDir );
	CJsonDoc		jsonMakeFile;
	if ( !jsonMakeFile.LoadFromFile( absolutePath.c_str() ) )
	{
		Error( "ShaderCompiler: Failed to load makefile '%s', maybe wrong JSON syntax or not found", absolutePath.c_str() );
		return false;
	}

	// Get path to directory with the main makefile
	std::string			makeFileDir;
	{
		std::string		tmpBuffer;
		S_GetFilePath( absolutePath.c_str(), tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, makeFileDir, "", false );
	}

	// Get include directories
	{
		CJsonValue		jsonInclude = jsonMakeFile.GetValue( "include" );
		if ( jsonInclude.IsValid() )
		{
			std::vector<CJsonValue>		jsonArray = jsonInclude.GetArray();
			if ( !jsonInclude.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				Error( "ShaderCompiler: Invalid 'include', must be array of strings (file: '%s')", absolutePath.c_str() );
				return false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue& jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Error( "ShaderCompiler: Invalid 'include[%i]', must be string (file: '%s')", index, absolutePath.c_str() );
						return false;
					}

					AddIncludeDir( jsonElement.GetString().c_str(), makeFileDir.c_str() );
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
				Error( "ShaderCompiler: Invalid 'shaders', must be array of strings (file: '%s')", absolutePath.c_str() );
				return false;
			}
			else
			{
				for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
				{
					const CJsonValue& jsonElement = jsonArray[index];
					if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Error( "ShaderCompiler: Invalid 'shaders[%i]', must be string (file: '%s')", index, absolutePath.c_str() );
						return false;
					}

					std::string		shaderPath = S_Sprintf( "%s.shader", jsonElement.GetString().c_str() );
					if ( !LoadShader( shaderPath.c_str(), makeFileDir.c_str() ) )
					{
						Error( "ShaderCompiler: Failed to load shader '%s' (file: '%s')", shaderPath.c_str(), absolutePath.c_str() );
						return false;
					}
				}
			}
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
bool CShaderCompilerMakeFile::LoadShader( const achar* pPath, const achar* pBaseDir /* = "" */ )
{
	// Load JSON file
	shader_t		shader;
	std::string		absolutePath;
	S_MakeAbsolutePath( pPath, absolutePath, pBaseDir );
	CJsonDoc		jsonShader;
	if ( !jsonShader.LoadFromFile( absolutePath.c_str() ) )
	{
		Error( "ShaderCompiler: Failed to load shader '%s', maybe wrong JSON syntax or not found", absolutePath.c_str() );
		return false;
	}

	// Get source file
	CJsonValue	jsonSource = jsonShader.GetValue( "source" );
	if ( jsonSource.IsValid() )
	{
		if ( !jsonSource.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Error( "ShaderCompiler: Invalid 'source', must be string (file: '%s')", absolutePath.c_str() );
			return false;
		}
		else
		{
			S_MakeAbsolutePath( jsonSource.GetString(), shader.source, pBaseDir );
		}
	}
	else
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'source' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get main function
	CJsonValue	jsonMainFunc = jsonShader.GetValue( "main_func" );
	if ( jsonMainFunc.IsValid() )
	{
		if ( !jsonMainFunc.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Error( "ShaderCompiler: Invalid 'main_func', must be string (file: '%s')", absolutePath.c_str() );
			return false;
		}
		else
		{
			shader.mainFunc = jsonMainFunc.GetString();
		}
	}
	else
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'main_func' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get shader type
	CJsonValue	jsonType = jsonShader.GetValue( "type" );
	if ( jsonType.IsValid() )
	{
		if ( !jsonType.IsA( JSONVALUE_TYPE_STRING ) )
		{
			Error( "ShaderCompiler: Invalid 'type', must be string (file: '%s')", absolutePath.c_str() );
			return false;
		}
		else if ( !ConvStringToShaderType( jsonType.GetString().c_str(), shader.type ) )
		{
			Error( "ShaderCompiler: Unknown shader type '%s' (file: '%s')", jsonType.GetString().c_str(), absolutePath.c_str() );
			return false;
		}
	}
	else
	{
		Error( "ShaderCompiler: Invalid shader, not found required field 'type' (file: '%s')", absolutePath.c_str() );
		return false;
	}

	// Get flags
	CJsonValue		jsonFlags = jsonShader.GetValue( "flags" );
	if ( jsonFlags.IsValid() )
	{
		if ( !jsonFlags.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Error( "ShaderCompiler: Invalid 'flags', must be array of objects (file: '%s')", absolutePath.c_str() );
			return false;
		}
		else
		{
			const std::vector<CJsonValue>	jsonArray = jsonFlags.GetArray();
			for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
			{
				const CJsonValue& jsonElement = jsonArray[index];
				if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
				{
					Error( "ShaderCompiler: Invalid 'flags[%i]', must be object with required fields: 'name', 'min' and 'max' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				CJsonObject		jsonObject = jsonElement.GetObject();
				shaderFlag_t	flag;

				// Get flag name
				CJsonValue		jsonName = jsonObject.GetValue( "name" );
				if ( jsonName.IsValid() )
				{
					if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Error( "ShaderCompiler: Invalid 'flags[%i].name', must be string (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						flag.name = jsonName.GetString();
					}
				}
				else
				{
					Error( "ShaderCompiler: Invalid 'flags[%i]', not found required field 'name' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				// Get minimum value
				CJsonValue		jsonMin = jsonObject.GetValue( "min" );
				if ( jsonMin.IsValid() )
				{
					if ( !jsonMin.IsA( JSONVALUE_TYPE_INT ) )
					{
						Error( "ShaderCompiler: Invalid 'flags[%i].min', must be integer (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						flag.minValue = jsonMin.GetInt();
					}
				}
				else
				{
					Error( "ShaderCompiler: Invalid 'flags[%i]', not found required field 'min' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				// Get maximum value
				CJsonValue		jsonMax = jsonObject.GetValue( "max" );
				if ( jsonMax.IsValid() )
				{
					if ( !jsonMax.IsA( JSONVALUE_TYPE_INT ) )
					{
						Error( "ShaderCompiler: Invalid 'flags[%i].max', must be integer (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						flag.maxValue = jsonMax.GetInt();
					}
				}
				else
				{
					Error( "ShaderCompiler: Invalid 'flags[%i]', not found required field 'max' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				// Get default value
				CJsonValue		jsonDefault = jsonObject.GetValue( "default" );
				if ( jsonDefault.IsValid() )
				{
					if ( !jsonDefault.IsA( JSONVALUE_TYPE_INT ) )
					{
						Error( "ShaderCompiler: Invalid 'flags[%i].default', must be integer (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						flag.bSetDefault = true;
						flag.defaultValue = jsonDefault.GetInt();
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
					Error( "ShaderCompiler: Invalid 'flags[%i]', default value is out of range [%i; %i] (file: '%s')", index, flag.minValue, flag.maxValue, absolutePath.c_str() );
					return false;
				}

				// Add a new flag into array
				shader.flags.emplace_back( flag );
			}
		}
	}

	// Get defines
	CJsonValue		jsonDefines = jsonShader.GetValue( "defines" );
	if ( jsonDefines.IsValid() )
	{
		if ( !jsonDefines.IsA( JSONVALUE_TYPE_ARRAY ) )
		{
			Error( "ShaderCompiler: Invalid 'defines', must be array of objects (file: '%s')", absolutePath.c_str() );
			return false;
		}
		else
		{
			const std::vector<CJsonValue>	jsonArray = jsonDefines.GetArray();
			for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
			{
				const CJsonValue&	jsonElement = jsonArray[index];
				if ( !jsonElement.IsA( JSONVALUE_TYPE_OBJECT ) )
				{
					Error( "ShaderCompiler: Invalid 'defines[%i]', must be object with required fields: 'name' and 'value' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				CJsonObject		jsonObject = jsonElement.GetObject();
				shaderDefine_t	define;

				// Get define name
				CJsonValue		jsonName = jsonObject.GetValue( "name" );
				if ( jsonName.IsValid() )
				{
					if ( !jsonName.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Error( "ShaderCompiler: Invalid 'defines[%i].name', must be string (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						define.name = jsonName.GetString();
					}
				}
				else
				{
					Error( "ShaderCompiler: Invalid 'defines[%i]', not found required field 'name' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				// Get define value
				CJsonValue		jsonValue = jsonObject.GetValue( "value" );
				if ( jsonValue.IsValid() )
				{
					if ( !jsonValue.IsA( JSONVALUE_TYPE_STRING ) )
					{
						Error( "ShaderCompiler: Invalid 'defines[%i].value', must be string (file: '%s')", index, absolutePath.c_str() );
						return false;
					}
					else
					{
						define.value = jsonValue.GetString();
					}
				}
				else
				{
					Error( "ShaderCompiler: Invalid 'defines[%i]', not found required field 'value' (file: '%s')", index, absolutePath.c_str() );
					return false;
				}

				// Add a new define into array
				shader.defines.emplace_back( define );
			}
		}
	}

	shader.CalcNumFlagCombos();
	shaders.emplace_back( shader );
	Msg( "ShaderCompiler: Loaded shader '%s'", absolutePath.c_str() );
	return true;
}