#include "core/crashdump.h"
#include "core/icommandline.h"
#include "stdlib/filetools.h"
#include "interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "engine/icvar.h"
#include "appframework/iappsystemgroup.h"
#include "tools/ecscompiler/ecsfileparser.h"

//-----------------------------------------------------------------------------
// ECS compiler application
//-----------------------------------------------------------------------------
class CEcsCompilerApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Allow the application to do some work after AppSystems are connected but
	// they aren't all Initialized
	// Return FALSE if there's any problems and the app will abort
	virtual bool PreInit() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	// Allow the application to do some work after all AppSystems are shut down
	virtual void PostShutdown() override;

private:
	void PrintUsageHelp();
	bool ParseEcsFile( const achar* pPath, CEcsSystemStub& stubs );
	bool GenerateCppHeader( const achar* pPath, const CEcsStubModule* pEcsModule );
};


/*
==================
CEcsCompilerApp::Create
==================
*/
bool CEcsCompilerApp::Create()
{
	// Load application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "engine"			DLL_EXT_STRING,			CVAR_QUERY_INTERFACE_VERSION		},	// This one must be first
		{ "filesystem"		DLL_EXT_STRING,			FILESYSTEM_INTERFACE_VERSION		},
		{ "engine"			DLL_EXT_STRING,			CVAR_INTERFACE_VERSION				},
		{ "", "" }																			// Required to terminate the list
	};

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// We are done
	return true;
}

/*
==================
CEcsCompilerApp::PreInit
==================
*/
bool CEcsCompilerApp::PreInit()
{
	return ConnectStdLib( GetFactory() );
}

/*
==================
CEcsCompilerApp::Main
==================
*/
int32 CEcsCompilerApp::Main()
{
	// Is need to print help of usage
	bool			bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get a path to source ECS file
	const achar*	pEcsFile = CommandLine()->GetFirstValue( "file" );
	bool			bInvalidEcsFilePath = !pEcsFile || pEcsFile[0] == '\0';

	// Get an output directory for C++ file
	const achar*	pCppFileDir = CommandLine()->GetFirstValue( "output" );
	bool			bInvalidCppFileDir = !pCppFileDir || pCppFileDir[0] == '\0';

	// Print help of usage if it need or some parameters aren't set 
	if ( bPrintHelpUsage || bInvalidEcsFilePath || bInvalidCppFileDir )
	{
		PrintUsageHelp();
		return 0;
	}

	// Parse the ECS file
	CEcsSystemStub		ecsSystemStub;
	if ( !ParseEcsFile( pEcsFile, ecsSystemStub ) )
	{
		return 1;
	}

	// Generate C++ headers for each ECS module
	bool											bHasError		= false;
	const std::vector<TRefPtr<CEcsStubModule>>&		ecsStubModules	= ecsSystemStub.GetModules();
	for ( uint32 ecsStubModuleIdx = 0, numEcsStubModules = ( uint32 )ecsStubModules.size(); ecsStubModuleIdx < numEcsStubModules; ++ecsStubModuleIdx )
	{
		const CEcsStubModule*	pEcsStubModule = ecsStubModules[ecsStubModuleIdx];
		std::string				moduleNameLower = pEcsStubModule->GetName();
		S_Strlwr( moduleNameLower.data() );
		bHasError |= !GenerateCppHeader( S_Sprintf( "%s/ecs_%s.h", pCppFileDir, moduleNameLower.c_str() ).c_str(), pEcsStubModule );
	}

	return 0;
}

/*
==================
CEcsCompilerApp::PostShutdown
==================
*/
void CEcsCompilerApp::PostShutdown()
{
	DisconnectStdLib();
}

/*
==================
CEcsCompilerApp::ParseEcsFile
==================
*/
bool CEcsCompilerApp::ParseEcsFile( const achar* pPath, CEcsSystemStub& stubs )
{
	// Read whole the file
	TRefPtr<IStreamDataReader>	pFileReader = g_pFileSystem->CreateFileReader( pPath );
	if ( !pFileReader )
	{
		Error( "EcsCompiler: Failed to open ECS file '%s'", pPath );
		return false;
	}

	std::string		buffer;
	buffer.resize( pFileReader->GetSize() );
	pFileReader->Read( buffer.data(), buffer.size() );
	pFileReader		= NULL;

	// Parse the ECS file
	CEcsFileParser		ecsFileParser( stubs );
	return ecsFileParser.ParseFile( pPath, buffer.c_str() );
}

/*
==================
CEcsCompilerApp::GenerateCppHeader
==================
*/
bool CEcsCompilerApp::GenerateCppHeader( const achar* pPath, const CEcsStubModule* pEcsModule )
{
	// Create a C++ header file
	TRefPtr<IStreamDataWriter>		pFileWriter = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFileWriter )
	{
		Error( "EcsCompiler: Failed to create C++ header '%s'", pPath );
		return false;
	}

	// Get include guard define name
	std::string		includeGuardDefineName = S_GetFileName( pPath );
	S_Strupr( includeGuardDefineName.data() );
	for ( uint32 idx = 0, size = ( uint32 )includeGuardDefineName.size(); idx < size; ++idx )
	{
		if ( includeGuardDefineName[idx] == '.' )
		{
			includeGuardDefineName[idx] = '_';
		}
	}

	// Write header and include guard
	std::string		cppCode;
	cppCode +=  "//\n"
				"// This is automatically generated by the ecscompiler.\n"
				"// DO NOT modify this manually! Edit the corresponding .ecs files instead!\n"
				"// Broken Singularity, All Rights Reserved.\n"
				"//\n\n";
	cppCode += S_Sprintf( "#ifndef %s\n", includeGuardDefineName.c_str() );
	cppCode += S_Sprintf( "#define %s\n", includeGuardDefineName.c_str() );

	// Write usings
	const std::vector<TRefPtr<CEcsStubUsing>>			ecsStubUsings = pEcsModule->GetUsings();
	if ( !ecsStubUsings.empty() )
	{
		cppCode += S_Sprintf( "\n// BEGIN USING\n" );
		for ( uint32 usingIdx = 0, numUsings = ( uint32 )ecsStubUsings.size(); usingIdx < numUsings; ++usingIdx )
		{
			CEcsStubUsing*		pEcsStubUsing = ecsStubUsings[usingIdx];
			cppCode += S_Sprintf( "#include \"%s\"\n", pEcsStubUsing->GetName() );
		}
		cppCode += S_Sprintf( "// END USING\n" );
	}

	// Write components
	const std::vector<TRefPtr<CEcsStubComponent>>&		ecsStubComponents = pEcsModule->GetComponents();
	if ( !ecsStubComponents.empty() )
	{
		cppCode += S_Sprintf( "\n// BEGIN ECS COMPONENTS\n" );
		for ( uint32 componentIdx = 0, numComponents = ( uint32 )ecsStubComponents.size(); componentIdx < numComponents; ++componentIdx )
		{
			CEcsStubComponent*										pEcsStubComponent			= ecsStubComponents[componentIdx];
			const std::vector<TRefPtr<CEcsStubDefaultFieldValue>>&	ecsStubDefaultFieldValues	= pEcsStubComponent->GetDefaultFieldValues();
			const std::vector<TRefPtr<CEcsStubField>>&				ecsStubFields				= pEcsStubComponent->GetFields();		
			bool													bComponentEmpty				= ecsStubDefaultFieldValues.empty() && ecsStubFields.empty();
			cppCode += S_Sprintf( "struct ecs%sComponent_t\n{", pEcsStubComponent->GetName() );
			
			// Write constructor
			if ( !ecsStubDefaultFieldValues.empty() )
			{
				cppCode		+= S_Sprintf( "\n\tecs%sComponent_t()\n", pEcsStubComponent->GetName() );
				for ( uint32 defaultFieldValueIdx = 0, numDefaultFieldValues = ( uint32 )ecsStubDefaultFieldValues.size(); defaultFieldValueIdx < numDefaultFieldValues; ++defaultFieldValueIdx )
				{
					CEcsStubDefaultFieldValue*		pEcsStubDefaultFieldValue = ecsStubDefaultFieldValues[defaultFieldValueIdx];
					cppCode += S_Sprintf( "\t\t%s %s(%s)\n", defaultFieldValueIdx == 0 ? ":" : ",", pEcsStubDefaultFieldValue->GetName(), pEcsStubDefaultFieldValue->GetValue() );
				}
				cppCode		+= "\t{}";
				if ( !ecsStubFields.empty() )
				{
					cppCode += "\n";
				}
			}

			// Write fields
			for ( uint32 fieldIdx = 0, numFields = ( uint32 )ecsStubFields.size(); fieldIdx < numFields; ++fieldIdx )
			{
				CEcsStubField*		pEcsStubField = ecsStubFields[fieldIdx];
				cppCode += S_Sprintf( "\n\t%s %s;", pEcsStubField->GetType(), pEcsStubField->GetName() );
			}

			cppCode += S_Sprintf( "%s};\n", !bComponentEmpty ? "\n" : "" );
			if ( componentIdx + 1 < numComponents )
			{
				cppCode += "\n";
			}
		}
		cppCode += S_Sprintf( "// END ECS COMPONENTS\n" );
	}

	// Write registrar of the module
	cppCode += "\n// Registrar of the module\n";
	cppCode += S_Sprintf( "struct ecs%sModule_t\n"
						  "{\n"
						  "\tecs%sModule_t( flecs::world& flecsWorld )\n"
						  "\t{\n", pEcsModule->GetName(), pEcsModule->GetName() );

	// Register components
	if ( !ecsStubComponents.empty() )
	{
		cppCode += "\t\t// Register components\n";
		for ( uint32 componentIdx = 0, numComponents = ( uint32 )ecsStubComponents.size(); componentIdx < numComponents; ++componentIdx )
		{
			CEcsStubComponent*		pEcsStubComponent = ecsStubComponents[componentIdx];
			cppCode += S_Sprintf( "\t\tflecsWorld.component<ecs%sComponent_t>();\n", pEcsStubComponent->GetName() );
		}
	}

	cppCode += "\t}\n};\n";

	// Write all data into the file
	cppCode += S_Sprintf( "\n#endif // !%s", includeGuardDefineName.c_str() );
	pFileWriter->Write( ( byte* )cppCode.c_str(), cppCode.size() );
	return true;
}

/*
==================
CEcsCompilerApp::PrintUsageHelp
==================
*/
void CEcsCompilerApp::PrintUsageHelp()
{
	Msg( "" );
	Msg( "ECS compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage ecscompiler -file <path> -output <path_dir>" );
	Msg( "Ex: ecscompiler -file ../src/public/libs/gameframework/ecs/ecs_movement.ecs -output ../src/public/libs/gameframework/ecs/" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "file\tSource ECS file to compile. For syntax ecample see 'src/public/libs/gameframework/ecs/*.ecs'" );
	Msg( "output\tSpecify an output directory for a C++ file" );
	Msg( "" );
}


/*
==================
main
==================
*/
int main( int argc, char** argv )
{
	// Initialize the main thread
	Sys_InitMainThread();

	// Setup application information for the crash dump
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ "ECS Compiler", __DATE__ " " __TIME__, NULL, NULL } );

	// Initialize a command line
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

	// Run the application
	CEcsCompilerApp		ecsCompilerApp;
	return ecsCompilerApp.Run();
}