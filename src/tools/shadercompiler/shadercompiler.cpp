#include "pch_shadercompiler.h"
#include "stdlib/jsondoc.h"
#include "core/icommandline.h"
#include "core/crashdump.h"
#include "cvar/icvar.h"
#include "appframework/appframework.h"
#include "shadercache/shadercache.h"
#include "tools/shadercompiler/makefile.h"
#include "tools/shadercompiler/shader_cppclass_generator.h"
#include "tools/shadercompiler/shadercompiler.h"
#include "tools/shadercompiler/ishadercompiler_backend.h"
#include "tools/shadercompiler/shadercompiler_environment.h"
#include "tools/shadercompiler/shadercompiler_output.h"

// Table of shader type names
static const achar* s_pShaderTypeNames[] =
{
	"vertex",			// STUDIOAPI_SHADER_VERTEX
	"hull",				// STUDIOAPI_SHADER_HULL
	"domain",			// STUDIOAPI_SHADER_DOMAIN
	"geometry",			// STUDIOAPI_SHADER_GEOMETRY
	"pixel",			// STUDIOAPI_SHADER_PIXEL
	"compute"			// STUDIOAPI_SHADER_COMPUTE
};
static_assert( STUDIOAPI_SHADER_NUM_TYPES == ARRAYSIZE( s_pShaderTypeNames ), "Array size 's_pShaderTypeNames' must be equal to STUDIOAPI_SHADER_NUM_TYPES" );


/*
==================
PrintShaderCompileHelp
==================
*/
static void PrintShaderCompileHelp()
{
	Msg( "" );
	Msg( "Shader compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage: shadercompiler -makefile <path> -backend <name> [-game <path>]" );
	Msg( "Ex: shadercompiler -makefile C:/singularity_engine/src/materialsystem/stdshaders/shaderlist.makefile -backend shadercompile_dx11" );
	Msg( "Ex: shadercompiler -makefile ../src/materialsystem/gameshaders/shaderlist.makefile -game sandbox -backend shadercompile_dx11" );
	Msg( "" );
	Msg( "makefile\t\tPath to a shader makefile. For syntax example see src/materialsystem/stdshaders/shaderlist.makefile" );
	Msg( "backend\t\tShader compile backend. When command line has skipcompilation then this is ignoring (e.g: shadercompiler_vk, shadercompile_dx11, etc)" );
	Msg( "gencpp\t\tIs need generate C++ helper classes for shaders" );
	Msg( "outcpp\t\tOverride output path for generated shader helper C++ classes (relative path from makefile)" );
	Msg( "skipcompilation\t\tIs need skip shader compilation" );
	Msg( "debug\t\tCompile debug shader version" );
	Msg( "" );
}

/*
==================
ConvStringToShaderType
==================
*/
bool ConvStringToShaderType( const achar* pShaderTypeName, studioAPIShaderType_t& shaderType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pShaderTypeNames ); ++index )
	{
		if ( !S_Stricmp( pShaderTypeName, s_pShaderTypeNames[index] ) )
		{
			// We found! Return current type
			shaderType = ( studioAPIShaderType_t )index;
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
void ConvShaderTypeToString( studioAPIShaderType_t shaderType, const achar*& pShaderTypeName )
{
	Assert( shaderType < STUDIOAPI_SHADER_NUM_TYPES );
	pShaderTypeName = s_pShaderTypeNames[( uint32 )shaderType];
}


//-----------------------------------------------------------------------------
// Shader compile app system group
//-----------------------------------------------------------------------------
class CShaderCompileAppSystemGroup : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// IAppSystemGroup interface
	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

	CShaderCompileAppSystemGroup();

private:
	bool GenerateShaderCppClass();
	bool CompileShaders();
	bool CompileShader( const shader_t& shader, CShaderCacheDoc& shaderCacheDoc );
	bool LoadBackend();
	void UnloadBackend();

	bool					bDebugConfiguration;
	CMakeFile				makefile;
	dllHandle_t				backendHandle;
	IShaderCompilerBackend*	pShaderCompileBackend;
};


/*
==================
CShaderCompileAppSystemGroup::CShaderCompileAppSystemGroup
==================
*/
CShaderCompileAppSystemGroup::CShaderCompileAppSystemGroup()
	: bDebugConfiguration( false )
	, backendHandle( NULL )
	, pShaderCompileBackend( NULL )
{}

/*
==================
CShaderCompileAppSystemGroup::Main
==================
*/
int32 CShaderCompileAppSystemGroup::Main()
{
	// Is need to print help of usage
	bool			bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get and parse a shader make file
	const achar*	pMakeFilePath = CommandLine()->GetFirstValue( "makefile" );
	bool			bInvalidMakeFilePath = !pMakeFilePath || pMakeFilePath[0] == '\0';
	if ( !bInvalidMakeFilePath && !makefile.LoadMakeFile( pMakeFilePath ) )
	{
		Error( "ShaderCompiler: Failed to load makefile '%s'", pMakeFilePath );
		return 1;
	}

	// Check if need compile debug shader versions
	bDebugConfiguration = CommandLine()->HasParam( "debug" );

	// Make sure what we have valid the backend (if we haven't 'skipcompilation')
	const achar*	pBackendName = CommandLine()->GetFirstValue( "backend" );
	bool			bInvalidBackend = !CommandLine()->HasParam( "skipcompilation" ) && ( !pBackendName || pBackendName[0] == '\0' );

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || bInvalidMakeFilePath || bInvalidBackend )
	{
		PrintShaderCompileHelp();
		return 0;
	}

	// Override output path of shader C++ helper classes
	if ( CommandLine()->HasParam( "outcpp" ) )
	{
		std::string		outputCppGenShaders;
		S_MakeAbsolutePath( CommandLine()->GetFirstValue( "outcpp" ), outputCppGenShaders, makefile.GetMakeFileDir(), false );
		makefile.SetCppGenOutput( outputCppGenShaders.c_str() );
	}

	// Generate C++ helper classes for shaders
	if ( CommandLine()->HasParam( "gencpp" ) )
	{
		if ( !GenerateShaderCppClass() )
		{
			return 1;
		}
	}

	// We are done if no need do compilation of shaders
	if ( CommandLine()->HasParam( "skipcompilation" ) )
	{
		return 0;
	}

	// Now load backend for compilation
	if ( !LoadBackend() )
	{
		return 1;
	}

	// Compile shaders
	if ( !CompileShaders() )
	{
		return 1;
	}

	// We are done
	UnloadBackend();
	return 0;
}

/*
==================
CShaderCompileAppSystemGroup::GenerateShaderCppClass
==================
*/
bool CShaderCompileAppSystemGroup::GenerateShaderCppClass()
{
	bool							bResult = true;
	CShaderCppClassGenerator		shaderCppClassGenerator;
	const std::vector<shader_t>&	shaders = makefile.GetShaders();	
	Msg( "ShaderCompiler: Generate C++ class for each shader" );

	for ( uint32 index = 0, count = ( uint32 )shaders.size(); index < count; ++index )
	{
		// Generate C++ class for this shader
		const shader_t&			shader = shaders[index];
		shaderCppClassGenerator.Reset();
		shaderCppClassGenerator.Generate( shader );
		const std::string&		buffer = shaderCppClassGenerator.GetBuffer();

		// Generate file path from base name of shader source file and the one type
		std::string		shaderName;
		std::string		filePath;
		{
			// Convert shader type to string
			const achar*	pShaderTypeName;
			ConvShaderTypeToString( shader.type, pShaderTypeName );

			// Get shader name from source file name
			S_GetFileBaseName( shader.source, shaderName, false );

			// Get file path
			filePath = S_Sprintf( "%s/%s_%s.gen.h", makefile.GetCppGenOutput().c_str(), shaderName.c_str(), pShaderTypeName );
			S_FixPathSeparators( filePath );
		}

		// Save buffer into file
		TRefPtr<IStreamDataWriter>	file = g_pFileSystem->CreateFileWriter( filePath.c_str() );
		if ( file )
		{
			file->Write( ( void* )buffer.data(), buffer.size() * sizeof( achar ) );
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
CShaderCompileAppSystemGroup::CompileShaders
==================
*/
bool CShaderCompileAppSystemGroup::CompileShaders()
{
	// Iterate over shaders and each the one compile for all flag combination
	bool							bResult = true;
	const std::vector<shader_t>&	shaders = makefile.GetShaders();
	for ( uint32 index = 0, count = ( uint32 )shaders.size(); index < count; ++index )
	{
		const shader_t&		shader = shaders[index];
		CShaderCacheDoc		shaderCacheDoc;
		shaderCacheDoc.SetType( shader.type );

		// Compile the shader
		Msg( "ShaderCompiler: Compile shader '%s' (%i combos)", shader.source.c_str(), shader.numFlagCombos );
		if ( !CompileShader( shader, shaderCacheDoc ) )
		{
			bResult = false;
			continue;
		}

		// Save shader cache
		std::string		shaderFileName;
		S_GetFileBaseName( shader.source, shaderFileName, false );
		std::string		outputFile = S_Sprintf( "%s/shaders/%s/%s/%s.ssc", makefile.GetCacheOutputDir().c_str(), pShaderCompileBackend->GetShaderPlatform(), makefile.GetShaderListName().c_str(), shaderFileName.c_str() );
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
CShaderCompileAppSystemGroup::CompileShader
==================
*/
bool CShaderCompileAppSystemGroup::CompileShader( const shader_t& shader, CShaderCacheDoc& shaderCacheDoc )
{
	std::vector<int32>				flagVarSlots( shader.flags.size() );

	// Set all the variables to min values
    int32*			pFlagVarSlotBegin   = flagVarSlots.data();
    int32*			pFlagVarSlotEnd     = pFlagVarSlotBegin + ( uint32 )flagVarSlots.size();
    int32*			pSetFlagVar         = NULL;
	shaderFlag_t*	pSetFlag			= NULL;

	for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( shaderFlag_t* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
	{
		*pSetFlagVar = pSetFlag->minValue;
	}

	bool							bResult		= true;
	const std::vector<std::string>&	includeDirs = makefile.GetShaderIncludeDirs();
	uint32							currentCombo = 0;
	while ( currentCombo < shader.numFlagCombos )
	{
		++currentCombo;

		// Compile shader
		CShaderCompilerOutput		shaderCompileOutput;
		CShaderCompilerEnvironment	shaderCompileEnvironment( shader.type );

		// Set debug flag if it need
		if ( bDebugConfiguration )
		{
			shaderCompileEnvironment.SetCompileFlags( SHADERCOMPILER_DEBUG );
		}

		// Add to the environment all include directories
		for ( uint32 includeDirIdx = 0, numIncludeDirs = ( uint32 )includeDirs.size(); includeDirIdx < numIncludeDirs; ++includeDirIdx )
		{
			shaderCompileEnvironment.AddIncludeDir( includeDirs[includeDirIdx].c_str() );
		}

		// Add to the environment all defines
		for ( uint32 defineIdx = 0, numDefines = ( uint32 )shader.defines.size(); defineIdx < numDefines; ++defineIdx )
		{
			const shaderDefine_t&		shaderDefine = shader.defines[defineIdx];
			shaderCompileEnvironment.AddDefine( shaderDefine.name.c_str(), shaderDefine.value.c_str() );
		}

		// Add to the environment current flags
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( shaderFlag_t* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
		{
			shaderCompileEnvironment.AddDefine( pSetFlag->name.c_str(), S_Sprintf( "%i", *pSetFlagVar ).c_str() );
		}

		// Compile shader
		bool	bCompileResult = pShaderCompileBackend->CompileShader( shader.source.c_str(), shader.mainFunc.c_str(), shader.type, &shaderCompileEnvironment, &shaderCompileOutput );
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
				const achar*	pDefineName		= NULL;
				const achar*	pDefineValue	= NULL;
				shaderCompileEnvironment.GetDefine( defineIdx, pDefineName, pDefineValue );
				Error( "ShaderCompiler:\t %s: %s", pDefineName, pDefineValue );
			}

			// Print combinations
			if ( shader.numFlagCombos > 1 )
			{
				Error( "ShaderCompiler:" );
				Error( "ShaderCompiler: Current combo:" );
				for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( shaderFlag_t* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
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
		for ( pSetFlagVar = pFlagVarSlotBegin, pSetFlag = ( shaderFlag_t* )shader.flags.data(); pSetFlagVar < pFlagVarSlotEnd; ++pSetFlagVar, ++pSetFlag )
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
CShaderCompileAppSystemGroup::LoadBackend
==================
*/
bool CShaderCompileAppSystemGroup::LoadBackend()
{
	// Unload old backend
	UnloadBackend();

	// Get path to backend
	std::string		backendPath = S_Sprintf( "//ENGINEBIN/%s" DLL_EXT_STRING, CommandLine()->GetFirstValue( "backend" ) );

	// Load module
	dllHandle_t		backendHandle = g_pFileSystem->LoadModule( backendPath.c_str() );
	if ( !backendHandle )
	{
		Warning( "ShaderCompiler: Failed to load backend '%s'", backendPath.c_str() );
		return false;
	}

	// Get at the backend interface
	createInterfaceFn_t		pFactory = Sys_GetFactory( backendHandle );
	if ( !pFactory )
	{
		Warning( "ShaderCompiler: Could not find factory interface in '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	IShaderCompilerBackend*	pShaderCompileBackend = ( IShaderCompilerBackend* )pFactory( SHADERCOMPILERBACKEND_INTERFACE_VERSION );
	if ( !pShaderCompileBackend )
	{
		Warning( "ShaderCompiler: Could not get IShaderCompilerBackend interface from '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// Allow the backend to try to connect to interfaces it needs
	if ( !pShaderCompileBackend->Connect( GetFactory() ) )
	{
		Warning( "ShaderCompiler: Failed to init backend '%s'", backendPath.c_str() );
		g_pFileSystem->UnloadModule( backendHandle );
		return false;
	}

	// We are done
	Msg( "ShaderCompiler: Backend '%s' loaded", backendPath.c_str() );
	CShaderCompileAppSystemGroup::backendHandle			= backendHandle;
	CShaderCompileAppSystemGroup::pShaderCompileBackend	= pShaderCompileBackend;
	return true;
}

/*
==================
CShaderCompileAppSystemGroup::UnloadBackend
==================
*/
void CShaderCompileAppSystemGroup::UnloadBackend()
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
		backendHandle			= NULL;
		pShaderCompileBackend	= NULL;
	}
}

/*
==================
CShaderCompileAppSystemGroup::PostShutdown
==================
*/
void CShaderCompileAppSystemGroup::PostShutdown()
{
	UnloadBackend();
	DisconnectStdLib();
}


/*
==================
shader_t::CalcNumFlagCombos
==================
*/
void shader_t::CalcNumFlagCombos()
{
	numFlagCombos = 1;
	for ( uint32 index = 0, count = ( uint32 )flags.size(); index < count; ++index )
	{
		const shaderFlag_t&		flag = flags[index];
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
	// Enable developer messages if we in debug configuration
#if DEBUG
	Logger()->SetGroupActivate( LOG_GROUP_DEVELOPER, true );
#endif // DEBUG

	// Initialize the main thread
	Sys_InitMainThread();

	// Initialize OS console
	LogConsoleOS()->Show( true );

	// Setup application information for the crash dump
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ "Shader Compile", __DATE__ " " __TIME__, NULL, NULL } );

	// Initialize command line
	{
		std::string		arguments;
		for ( uint32 index = 0; index < ( uint32 )argc; ++index )
		{
			if ( *argv[index] == '-' || *argv[index] == '/' )
			{
				arguments += argv[index];
				arguments += " ";
			}
			else
			{
				arguments += S_Sprintf( "\"%s\" ", argv[index] );
			}
		}
		CommandLine()->Init( arguments.c_str() );
	}

	// Run application
	CShaderCompileAppSystemGroup	shaderCompileSystems;
	CApplication					application( &shaderCompileSystems, "shadercompiler" );
	return application.Run();
}