#include "pch_shadercompiler.h"
#include "tier0/icommandline.h"
#include "appframework/application.h"
#include "utils/shadercache/shadercache.h"
#include "tools/shadercompiler/shadercompiler_cppgenerator.h"
#include "tools/shadercompiler/shadercompiler.h"
#include "tools/shadercompiler/shadercompiler_makefile.h"
#include "tools/shadercompiler/ishadercompiler_backend.h"
#include "tools/shadercompiler/shadercompiler_environment.h"
#include "tools/shadercompiler/shadercompiler_output.h"

// Table of shader type names
static const char* s_pShaderTypeNames[] = {
	"vertex",	 // STUDIOAPI_SHADER_VERTEX
	"hull",		 // STUDIOAPI_SHADER_HULL
	"domain",	 // STUDIOAPI_SHADER_DOMAIN
	"geometry",	 // STUDIOAPI_SHADER_GEOMETRY
	"pixel",	 // STUDIOAPI_SHADER_PIXEL
	"compute"	 // STUDIOAPI_SHADER_COMPUTE
};
static_assert( STUDIOAPI_SHADER_NUM_TYPES == ARRAYSIZE( s_pShaderTypeNames ), "Array size 's_pShaderTypeNames' must be equal to STUDIOAPI_SHADER_NUM_TYPES" );

// Table of shader compiler mode names
static const char* s_pShaderCompilerModeNames[] = {
	"compile",	// SHADER_COMPILER_MODE_COMPILE
	"gencpp"	// SHADER_COMPILER_MODE_GENCPP
};
static_assert( SHADER_COMPILER_MODE_NUM == ARRAYSIZE( s_pShaderCompilerModeNames ), "Array size 's_pShaderCompilerModeNames' must be equal to SHADER_COMPILER_MODE_NUM" );

// Table of system shader flag names
static const char* s_pShaderSystemFlagNames[] = {
	"VERTEXFACTORY"  // SHADER_SYSTEM_FLAG_VERTEXFACTORY
};
static_assert( SHADER_SYSTEM_FLAG_NUM == ARRAYSIZE( s_pShaderSystemFlagNames ), "Array size 's_pShaderSystemFlagNames' must be equal to SHADER_SYSTEM_FLAG_NUM" );

/*
==================
ConvStringToShaderType
==================
*/
bool ConvStringToShaderType( const char* pShaderTypeName, studioAPIShaderType_t& shaderType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pShaderTypeNames ); ++index )
	{
		if ( !S_Stricmp( pShaderTypeName, s_pShaderTypeNames[index] ) )
		{
			// We found! Return current type
			shaderType = (studioAPIShaderType_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	shaderType = STUDIOAPI_SHADER_NUM_TYPES;
	return false;
}

/*
==================
ConvShaderTypeToString
==================
*/
void ConvShaderTypeToString( studioAPIShaderType_t shaderType, const char*& pShaderTypeName )
{
	Assert( shaderType < STUDIOAPI_SHADER_NUM_TYPES );
	pShaderTypeName = s_pShaderTypeNames[(uint32)shaderType];
}

/*
==================
ConvStringToShaderCompilerMode
==================
*/
bool ConvStringToShaderCompilerMode( const char* pShaderCompilerModeName, shaderCompilerMode_t& shaderCompilerMode )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pShaderCompilerModeNames ); ++index )
	{
		if ( !S_Stricmp( pShaderCompilerModeName, s_pShaderCompilerModeNames[index] ) )
		{
			// We found! Return current type
			shaderCompilerMode = (shaderCompilerMode_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	shaderCompilerMode = SHADER_COMPILER_MODE_NUM;
	return false;
}

/*
==================
ConvShaderCompilerModeToString
==================
*/
void ConvShaderCompilerModeToString( shaderCompilerMode_t shaderCompilerMode, const char*& pShaderCompilerModeName )
{
	Assert( shaderCompilerMode < SHADER_COMPILER_MODE_NUM );
	pShaderCompilerModeName = s_pShaderCompilerModeNames[(uint32)shaderCompilerMode];
}

/*
==================
ConvShaderSystemFlagToString
==================
*/
const char* ConvShaderSystemFlagToString( shaderSystemFlag_t systemFlag )
{
	Assert( systemFlag < SHADER_SYSTEM_FLAG_NUM );
	return s_pShaderSystemFlagNames[(uint32)systemFlag];
}

//-----------------------------------------------------------------------------
// Shader compile app
//-----------------------------------------------------------------------------
class CShaderCompilerApp : public CApplication
{
	typedef CApplication BaseClass;

public:
	// CApplication interface
	virtual int32 Main() override;

	virtual const appInfo_t&		  GetAppInfo() const override;
	virtual const crashDumpAppInfo_t& GetCrashDumpInfo() const override;

	CShaderCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance = NULL );

private:
	void PrintUsageHelp();
	bool GenerateShaderCppClass();
	bool CompileShaders();
	bool CompileShader( const shader_t& shader, CShaderCacheDoc& shaderCacheDoc );
	bool LoadBackend();
	void UnloadBackend();

	bool					bDebugConfiguration;
	dllHandle_t				backendHandle;
	IShaderCompilerBackend* pShaderCompileBackend;
	CShaderCompilerMakeFile makefile;
	eastl::string			outputPath;
};

/*
==================
CShaderCompilerApp::CShaderCompilerApp
==================
*/
CShaderCompilerApp::CShaderCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance /*= NULL*/ )
	: CApplication( pCommandLine, hInstance )
	, bDebugConfiguration( false )
	, backendHandle( NULL )
	, pShaderCompileBackend( NULL )
{
}

/*
==================
CShaderCompilerApp::Main
==================
*/
int32 CShaderCompilerApp::Main()
{
	// Is need to print help of usage
	bool bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get shader compiler mode
	shaderCompilerMode_t mode		  = SHADER_COMPILER_MODE_NUM;
	const char*			 pMode		  = CommandLine()->GetFirstValue( "mode" );
	bool				 bInvalidMode = !pMode || pMode[0] == '\0' || !ConvStringToShaderCompilerMode( pMode, mode );

	// Get shader file
	const char* pFilePath	 = CommandLine()->GetFirstValue( "file" );
	bool		bInvalidFile = !pFilePath || pFilePath[0] == '\0';

	// Get an output directory
	outputPath				= CommandLine()->GetFirstValue( "output" );
	bool bInvalidOutputPath = outputPath.empty();

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || bInvalidMode || bInvalidFile || bInvalidOutputPath )
	{
		PrintUsageHelp();
		return 0;
	}

	// Load the vertex factory list if it's given on the command line
	const char* pVertexFactoryPath = CommandLine()->GetFirstValue( "vertexfactory" );
	if ( pVertexFactoryPath && pVertexFactoryPath[0] != '\0' && !makefile.LoadVertexFactoryList( pVertexFactoryPath ) )
	{
		Error( "ShaderCompiler: Failed to load vertex factory list '%s'", pVertexFactoryPath );
		return 1;
	}

	// Load shader file
	const char* pFileExtension = S_GetFileExtension( pFilePath );
	if ( !S_Stricmp( pFileExtension, "shader" ) )
	{
		if ( !makefile.LoadShader( pFilePath ) )
		{
			Error( "ShaderCompiler: Failed to load shader '%s'", pFilePath );
			return 1;
		}
	}
	else if ( !S_Stricmp( pFileExtension, "makefile" ) )
	{
		if ( !makefile.LoadMakeFile( pFilePath ) )
		{
			Error( "ShaderCompiler: Failed to load makefile '%s'", pFilePath );
			return 1;
		}
	}
	else
	{
		Error( "ShaderCompiler: Unknown '%s' extension", pFileExtension );
		return 1;
	}

	// Do some actions
	switch ( mode )
	{
		// Shader compilation mode
	case SHADER_COMPILER_MODE_COMPILE:
	{
		// Check if need compile shaders in debug configuration
		bDebugConfiguration = CommandLine()->HasParam( "debug" );

		// Add extra include paths from the command line
		uint32		 numIncludes = 0;
		const char** pIncludes	 = CommandLine()->GetValues( "include", numIncludes );
		if ( numIncludes > 0 )
		{
			for ( uint32 index = 0; index < numIncludes; ++index )
			{
				makefile.AddIncludeDir( pIncludes[index] );
			}
		}

		// Load compiler backend and compile shaders
		if ( !LoadBackend() || !CompileShaders() )
		{
			UnloadBackend();
			return 1;
		}

		UnloadBackend();
		break;
	}

		// Generate C++ helper classes mode
	case SHADER_COMPILER_MODE_GENCPP:
		if ( !GenerateShaderCppClass() )
		{
			return 1;
		}
		break;

	default:
		AssertMsg( false, "Unknown shader compiler mode 0x%X", mode );
		Error( "ShaderCompiler: Unknown shader compiler mode 0x%X", mode );
		return 1;
	}

	return 0;
}

/*
==================
CShaderCompilerApp::PrintHelp
==================
*/
void CShaderCompilerApp::PrintUsageHelp()
{
	Msg( "" );
	Msg( "Shader compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage: shadercompiler -mode <name> <input> -output <dir> [options]" );
	Msg( "Ex: shadercompiler -mode compile -file C:/shaderlist.makefile -output C:/shaders/stdshaders/ -backend shadercompiler_vk" );
	Msg( "Ex: shadercompiler -mode compile -file C:/test.shader -vertexfactory C:/vertexfactory.list -output C:/shaders/stdshaders/ -include \"C:/hlsl_include\" \"E:/hlsl_include\" -backend shadercompiler_vk" );
	Msg( "Ex: shadercompiler -mode gencpp -file C:/test.shader -vertexfactory C:/vertexfactory.list -output C:/cpp_shaders/" );
	Msg( "Ex: shadercompiler -mode gencpp -file C:/shaderlist.makefile -output C:/cpp_shaders/" );
	Msg( "" );
	Msg( "-mode <name>\t\tOperation mode." );
	Msg( "\t\t\t\tValid values:" );
	Msg( "\t\t\t\t\t* compile (Shader compilation mode)" );
	Msg( "\t\t\t\t\t* gencpp (Generate C++ helper classes mode)" );
	Msg( "-help\t\t\tPrint this message" );
	Msg( "-file <path>\t\tPath to a shader file (*.makefile or *.shader)" );
	Msg( "-output <dir>\t\tOutput directory path" );
	Msg( "-include <p1> [p2 ...]\tInclude directories (used in compile mode)" );
	Msg( "-debug\t\t\tCompile debug shader version (used in compile mode)" );
	Msg( "-backend <name>\t\tShader compiler backend (used in compile mode)" );
	Msg( "-vertexfactory <path>\tPath to a vertex factory list (*.list)" );
	Msg( "" );
}

/*
==================
CShaderCompilerApp::GenerateShaderCppClass
==================
*/
bool CShaderCompilerApp::GenerateShaderCppClass()
{
	bool						   bResult = true;
	CShaderCompilerCppGenerator	   cppGenerator;
	const eastl::vector<shader_t>& shaders = makefile.GetShaders();
	for ( uint32 index = 0, count = (uint32)shaders.size(); index < count; ++index )
	{
		// Generate C++ class for this shader
		const shader_t& shader = shaders[index];
		cppGenerator.Reset();
		cppGenerator.Generate( shader );
		const eastl::string& buffer = cppGenerator.GetBuffer();

		// Generate file path from base name of shader source file and the one type
		eastl::string shaderName;
		eastl::string filePath;
		{
			// Convert shader type to string
			const char* pShaderTypeName;
			ConvShaderTypeToString( shader.type, pShaderTypeName );

			// Get shader name from source file name
			S_GetFileBaseName( shader.source, shaderName, false );

			// Get file path
			filePath = S_Sprintf( "%s/%s_%s.gen.h", outputPath.c_str(), shaderName.c_str(), pShaderTypeName );
			S_FixPathSeparators( filePath );
		}

		// Save buffer into file
		CRefPtr<IStreamDataWriter> file = g_pFileSystem->CreateFileWriter( filePath.c_str() );
		if ( file )
		{
			file->Write( (void*)buffer.data(), buffer.size() * sizeof( char ) );
			Msg( "ShaderCompiler: C++ class for '%s' saved to '%s'", shaderName.c_str(), filePath.c_str() );
		}
		else
		{
			Error( "ShaderCompiler: Failed to save C++ class for '%s' to '%s'", shaderName.c_str(), filePath.c_str() );
			bResult = false;
		}
	}

	return bResult;
}

/*
==================
CShaderCompilerApp::CompileShaders
==================
*/
bool CShaderCompilerApp::CompileShaders()
{
	// Iterate over shaders and each the one compile for all flag combination
	bool						   bResult = true;
	const eastl::vector<shader_t>& shaders = makefile.GetShaders();
	for ( uint32 index = 0, count = (uint32)shaders.size(); index < count; ++index )
	{
		const shader_t& shader = shaders[index];
		CShaderCacheDoc shaderCacheDoc;
		shaderCacheDoc.SetType( shader.type );

		// Compile the shader
		Msg( "ShaderCompiler: Compile shader '%s' (%i combos)", shader.source.c_str(), shader.numFlagCombos );
		if ( !CompileShader( shader, shaderCacheDoc ) )
		{
			bResult = false;
			continue;
		}

		// Save shader cache
		eastl::string shaderFileName;
		S_GetFileBaseName( shader.source, shaderFileName, false );
		eastl::string outputFile = S_Sprintf( "%s/%s/%s.ssc", outputPath.c_str(), pShaderCompileBackend->GetShaderPlatform(), shaderFileName.c_str() );
		if ( !shaderCacheDoc.SaveFile( outputFile.c_str() ) )
		{
			bResult = false;
			continue;
		}
	}

	return bResult;
}

/*
==================
CShaderCompilerApp::CompileShader
==================
*/
bool CShaderCompilerApp::CompileShader( const shader_t& shader, CShaderCacheDoc& shaderCacheDoc )
{
	eastl::vector<int32> flagVarSlots( shader.flags.size() );

	// Set all the variables to min values
	int32*		  pFlagVarSlotBegin = flagVarSlots.data();
	int32*		  pFlagVarSlotEnd	= pFlagVarSlotBegin + (uint32)flagVarSlots.size();
	int32*		  pSetFlagVar		= NULL;
	shaderFlag_t* pSetFlag			= NULL;

	for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = (shaderFlag_t*)shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
	{
		*pSetFlagVar = pSetFlag->minValue;
	}

	bool								bResult		 = true;
	const eastl::vector<eastl::string>& includeDirs	 = makefile.GetIncludeDirs();
	uint32								currentCombo = 0;
	while ( currentCombo < shader.numFlagCombos )
	{
		++currentCombo;

		// Compile shader
		CShaderCompilerOutput	   shaderCompileOutput;
		CShaderCompilerEnvironment shaderCompileEnvironment( shader.type );

		// Set debug flag if it need
		if ( bDebugConfiguration )
		{
			shaderCompileEnvironment.SetCompileFlags( SHADERCOMPILER_DEBUG );
		}

		// Add to the environment all include directories
		for ( uint32 includeDirIdx = 0, numIncludeDirs = (uint32)includeDirs.size(); includeDirIdx < numIncludeDirs; ++includeDirIdx )
		{
			shaderCompileEnvironment.AddIncludeDir( includeDirs[includeDirIdx].c_str() );
		}

		// Add to the environment all defines
		for ( uint32 defineIdx = 0, numDefines = (uint32)shader.defines.size(); defineIdx < numDefines; ++defineIdx )
		{
			const shaderDefine_t& shaderDefine = shader.defines[defineIdx];
			shaderCompileEnvironment.AddDefine( shaderDefine.name.c_str(), shaderDefine.value.c_str() );
		}

		// Add to the environment current flags
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = (shaderFlag_t*)shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
		{
			shaderCompileEnvironment.AddDefine( pSetFlag->name.c_str(), S_Sprintf( "%i", *pSetFlagVar ).c_str() );
		}

		// Select the vertex factory implementation to substitute for 'vertexfactory.hlsl' in the combo
		if ( !shader.vertexFactories.empty() )
		{
			int32 vertexFactoryIndex = shader.GetSystemFlagValue( SHADER_SYSTEM_FLAG_VERTEXFACTORY, pFlagVarSlotBegin );
			Assert( vertexFactoryIndex >= 0 && vertexFactoryIndex < (int32)shader.vertexFactories.size() );
			shaderCompileEnvironment.SetVertexFactoryFile( shader.vertexFactories[vertexFactoryIndex]->source.c_str() );
		}

		// Compile shader
		bool bCompileResult = pShaderCompileBackend->CompileShader( shader.source.c_str(), shader.mainFunc.c_str(), shader.type, &shaderCompileEnvironment, &shaderCompileOutput );
		if ( !bCompileResult )
		{
			Error( "ShaderCompiler: Failed to compile shader '%s' at %i combo", shader.source.c_str(), currentCombo );
			if ( !shaderCompileOutput.GetErrorMsg().empty() )
			{
				Error( "ShaderCompiler: Message:" );
				Error( shaderCompileOutput.GetErrorMsg().c_str() );
			}
			else
			{
				Error( "ShaderCompiler: Message: <None>" );
			}
			Error( "ShaderCompiler: ------------ ENVIRONMENT INFO --------------" );

			// Print includes
			Error( "ShaderCompiler: Includes:" );
			for ( uint32 includeIdx = 0, numIncludes = shaderCompileEnvironment.GetNumIncludeDirs(); includeIdx < numIncludes; ++includeIdx )
			{
				Error( "ShaderCompiler:\t %s", shaderCompileEnvironment.GetIncludeDir( includeIdx ) );
			}
			Error( "ShaderCompiler:" );

			// Print defines
			Error( "ShaderCompiler: Defines:" );
			for ( uint32 defineIdx = 0, numDefines = shaderCompileEnvironment.GetNumDefines(); defineIdx < numDefines; ++defineIdx )
			{
				const char* pDefineName	 = NULL;
				const char* pDefineValue = NULL;
				shaderCompileEnvironment.GetDefine( defineIdx, pDefineName, pDefineValue );
				Error( "ShaderCompiler:\t %s: %s", pDefineName, pDefineValue );
			}

			// Print combinations
			if ( shader.numFlagCombos > 1 )
			{
				Error( "ShaderCompiler:" );
				Error( "ShaderCompiler: Current combo:" );
				for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = (shaderFlag_t*)shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
				{
					Error( "ShaderCompiler:\t %s: %i", pSetFlag->name.c_str(), *pSetFlagVar );
				}
			}

			Error( "ShaderCompiler: --------------------------------------------" );
			bResult = false;
		}

		// If shader was compiled add output data into shader cache
		if ( bCompileResult )
		{
			shaderCacheDoc.AddCache( shader.mainFunc.c_str(), shaderCompileOutput.GetReflectionData(), shaderCompileOutput.GetBytecode() );
		}

		// Do the next iteration
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = (shaderFlag_t*)shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
		{
			if ( ++*pSetFlagVar <= pSetFlag->maxValue )
			{
				break;
			}

			*pSetFlagVar = pSetFlag->minValue;
		}
	}

	return bResult;
}

/*
==================
CShaderCompilerApp::LoadBackend
==================
*/
bool CShaderCompilerApp::LoadBackend()
{
	// Unload old backend
	UnloadBackend();

	// Make sure what we have valid the backend
	const char* pBackendName = CommandLine()->GetFirstValue( "backend" );
	if ( !pBackendName || pBackendName[0] == '\0' )
	{
		Error( "ShaderCompiler: Shader compiler backend isn't specified" );
		return false;
	}

	// Get path to backend
	eastl::string backendPath = S_Sprintf( "//ENGINEBIN/%s" DLL_EXT_STRING, pBackendName );

	// Load module
	dllHandle_t backendHandle = g_pFileSystem->LoadModule( backendPath.c_str() );
	if ( !backendHandle )
	{
		Error( "ShaderCompiler: Failed to load backend '%s'", backendPath.c_str() );
		return false;
	}

	// Get at the backend interface
	createInterfaceFn_t pFactory = Sys_GetFactory( backendHandle );
	if ( !pFactory )
	{
		Error( "ShaderCompiler: Could not find factory interface in '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	IShaderCompilerBackend* pShaderCompileBackend = (IShaderCompilerBackend*)pFactory( SHADERCOMPILERBACKEND_INTERFACE_VERSION );
	if ( !pShaderCompileBackend )
	{
		Error( "ShaderCompiler: Could not get IShaderCompilerBackend interface from '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// Allow the backend to try to connect to interfaces it needs
	if ( !pShaderCompileBackend->Connect( GetFactory() ) )
	{
		Error( "ShaderCompiler: Failed to initialize backend '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// We are done
	Msg( "ShaderCompiler: Loaded backend '%s'", backendPath.c_str() );
	CShaderCompilerApp::backendHandle		  = backendHandle;
	CShaderCompilerApp::pShaderCompileBackend = pShaderCompileBackend;
	return true;
}

/*
==================
CShaderCompilerApp::UnloadBackend
==================
*/
void CShaderCompilerApp::UnloadBackend()
{
	if ( backendHandle || pShaderCompileBackend )
	{
		// Shutdown the backend
		if ( pShaderCompileBackend )
		{
			pShaderCompileBackend->Disconnect();
		}

		// Unload shader module
		if ( backendHandle )
		{
			g_pFileSystem->UnloadModule( backendHandle );
		}

		Msg( "ShaderCompiler: Backend unloaded" );
		backendHandle		  = NULL;
		pShaderCompileBackend = NULL;
	}
}

/*
==================
CShaderCompilerApp::GetAppInfo
==================
*/
const appInfo_t& CShaderCompilerApp::GetAppInfo() const
{
	static appInfo_t s_appInfo{ "shadercompiler", APPLICATION_TYPE_CONSOLE, CMD_FLAG_NONE, CVAR_FLAG_NONE };
	return s_appInfo;
}

/*
==================
CShaderCompilerApp::GetCrashDumpInfo
==================
*/
const crashDumpAppInfo_t& CShaderCompilerApp::GetCrashDumpInfo() const
{
	static crashDumpAppInfo_t s_crashDumpAppInfo{ "Shader Compiler", __DATE__ " " __TIME__, NULL, NULL };
	return s_crashDumpAppInfo;
}

/*
==================
shader_t::CalcNumFlagCombos
==================
*/
void shader_t::CalcNumFlagCombos()
{
	numFlagCombos = 1;
	for ( uint32 index = 0, count = (uint32)flags.size(); index < count; ++index )
	{
		const shaderFlag_t& flag = flags[index];
		numFlagCombos *= flag.maxValue - flag.minValue + 1;
	}
}

/*
==================
main
==================
*/
int main( int argc, char** argv )
{
	// Prepare a command line
	eastl::string commandLine;
	for ( uint32 index = 0; index < (uint32)argc; ++index )
	{
		if ( *argv[index] == '-' || *argv[index] == '/' )
		{
			commandLine += argv[index];
			commandLine += " ";
		}
		else
		{
			commandLine += S_Sprintf( "\"%s\" ", argv[index] );
		}
	}

	return CShaderCompilerApp( commandLine.c_str() ).Run();
}
