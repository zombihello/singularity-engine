#include "pch_resourcecompiler.h"
#include "tier0/icommandline.h"
#include "tier1/keyvalues.h"
#include "appframework/application.h"
#include "utils/stexdoc/stex_source_doc.h"
#include "utils/smatdoc/smat_source_doc.h"
#include "utils/smdldoc/smdl_source_doc.h"
#include "utils/sentdoc/sent_source_doc.h"
#include "utils/smapdoc/smap_source_doc.h"
#include "tools/resourcecompiler/appsystemgroup_resource_tools.h"

// Resource source types
enum resourceSourceType_t
{
	RESOURCE_SOURCE_TYPE_MATERIAL,
	RESOURCE_SOURCE_TYPE_TEXTURE,
	RESOURCE_SOURCE_TYPE_MODEL,
	RESOURCE_SOURCE_TYPE_ENTITY_DESC,
	RESOURCE_SOURCE_TYPE_MAP,
	RESOURCE_SOURCE_NUM_TYPES
};

// Table of source resource types by script file extension
static const char* s_pScriptFileExtensions[] = {
	"smat",	 // RESOURCE_SOURCE_TYPE_MATERIAL
	"stex",	 // RESOURCE_SOURCE_TYPE_TEXTURE
	"smdl",	 // RESOURCE_SOURCE_TYPE_MODEL
	"sent",	 // RESOURCE_SOURCE_TYPE_ENTITY_DESC
	"smap"	 // RESOURCE_SOURCE_TYPE_MAP
};
static_assert( RESOURCE_SOURCE_NUM_TYPES == ARRAYSIZE( s_pScriptFileExtensions ), "Array size 's_pScriptFileExtensions' must be equal to RESOURCE_SOURCE_NUM_TYPES" );

// Table for convert resourceSourceType_t to text
static const char* s_pResourceSourceTypeNames[] = {
	"material",			  // RESOURCE_SOURCE_TYPE_MATERIAL
	"texture",			  // RESOURCE_SOURCE_TYPE_TEXTURE
	"model",			  // RESOURCE_SOURCE_TYPE_MODEL
	"entity descriptor",  // RESOURCE_SOURCE_TYPE_ENTITY_DESC
	"map"				  // RESOURCE_SOURCE_TYPE_MAP
};
static_assert( RESOURCE_SOURCE_NUM_TYPES == ARRAYSIZE( s_pResourceSourceTypeNames ), "Array size 's_pResourceSourceTypeNames' must be equal to RESOURCE_SOURCE_NUM_TYPES" );

/*
==================
ConvScriptFileExtensionToResourceSourceType
==================
*/
static bool ConvScriptFileExtensionToResourceSourceType( const char* pFileExtension, resourceSourceType_t& resourceSourceType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pScriptFileExtensions ); ++index )
	{
		if ( s_pScriptFileExtensions[index] && !S_Stricmp( pFileExtension, s_pScriptFileExtensions[index] ) )
		{
			// We found! Return current type
			resourceSourceType = (resourceSourceType_t)index;
			return true;
		}
	}

	// We not found, return invalid type
	resourceSourceType = RESOURCE_SOURCE_NUM_TYPES;
	return false;
}

/*
==================
ConvResourceTypeToString
==================
*/
static const char* ConvResourceSourceTypeToString( resourceSourceType_t resourceSourceType )
{
	if ( resourceSourceType >= RESOURCE_SOURCE_NUM_TYPES )
	{
		return "unknown";
	}
	return s_pResourceSourceTypeNames[resourceSourceType];
}

//-----------------------------------------------------------------------------
// Resource compiler app
//-----------------------------------------------------------------------------
class CResourceCompilerApp : public CApplication
{
	typedef CApplication BaseClass;

public:
	// CApplication interface
	virtual void  Init() override;
	virtual int32 Main() override;
	virtual void  Shutdown() override;

	virtual const appInfo_t&		  GetAppInfo() const override;
	virtual const crashDumpAppInfo_t& GetCrashDumpInfo() const override;

	CResourceCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance = NULL );

private:
	struct resourceFile_t
	{
		~resourceFile_t()
		{
		}

		eastl::string		 srcPath;
		eastl::string		 destPath;
		resourceSourceType_t type;
	};

	void PrintUsageHelp();
	bool LoadFileList( const char* pPath );
	bool AddFileToCompile( const char* pPath, const char* pWorkDir );
	bool CompileTexture( const resourceFile_t& resourceFile ) const;
	bool CompileMaterial( const resourceFile_t& resourceFile ) const;
	bool CompileModel( const resourceFile_t& resourceFile ) const;
	bool CompileEntityDesc( const resourceFile_t& resourceFile ) const;
	bool CompileMap( const resourceFile_t& resourceFile ) const;

	CAppSystemGroupResourceTools resourceToolsSystemGroup;
	ITextureTool*				 pTextureTool;
	IMaterialTool*				 pMaterialTool;
	IModelTool*					 pModelTool;
	IEntityDescTool*			 pEntityDescTool;
	IMapTool*					 pMapTool;
	eastl::string				 outputPath;
	eastl::string				 workDir;
	eastl::list<resourceFile_t>	 files;
};

/*
==================
CResourceCompilerApp::CResourceCompilerApp
==================
*/
CResourceCompilerApp::CResourceCompilerApp( const char* pCommandLine, appInstanceHandle_t hInstance /* = NULL */ )
	: CApplication( pCommandLine, hInstance )
	, pTextureTool( NULL )
	, pMaterialTool( NULL )
	, pModelTool( NULL )
	, pEntityDescTool( NULL )
	, pMapTool( NULL )
{
}

/*
==================
CResourceCompilerApp::Init
==================
*/
void CResourceCompilerApp::Init()
{
	BaseClass::Init();

	// Initialize resource tools
	AddGroup( &resourceToolsSystemGroup );
	resourceToolsSystemGroup.InitSystems();

	// Get a texture, material, model and entity tool
	pTextureTool	= (ITextureTool*)FindSystem( TEXTURE_TOOL_INTERFACE_VERSION );
	pMaterialTool	= (IMaterialTool*)FindSystem( MATERIAL_TOOL_INTERFACE_VERSION );
	pModelTool		= (IModelTool*)FindSystem( MODEL_TOOL_INTERFACE_VERSION );
	pEntityDescTool = (IEntityDescTool*)FindSystem( ENTITYDESC_TOOL_INTERFACE_VERSION );
	pMapTool		= (IMapTool*)FindSystem( MAP_TOOL_INTERFACE_VERSION );

	// Initialize a default pixel format
	for ( uint32 index = 0; index < STUDIOAPI_PIXEL_NUM_FORMATS; ++index )
	{
		if ( !pTextureTool->IsSupportPixelFormat( (studioAPIPixelFormat_t)index ) )
		{
			continue;
		}

		// First pixel format which support by the Singularity texture tool it is our default format
		CSTEXSourceTextureDoc::SetDefaultPixelFormat( (studioAPIPixelFormat_t)index );
		break;
	}
}

/*
==================
CResourceCompilerApp::Main
==================
*/
int32 CResourceCompilerApp::Main()
{
	// Is need to print help of usage
	bool bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get a path to a file or file list
	const char* pFile			 = CommandLine()->GetFirstValue( "file" );
	const char* pFileList		 = CommandLine()->GetFirstValue( "filelist" );
	bool		bInvalidFile	 = !pFile || pFile[0] == '\0';
	bool		bInvalidFileList = !pFileList || pFileList[0] == '\0';

	// Get an output directory
	outputPath				= CommandLine()->GetFirstValue( "output" );
	bool bInvalidOutputPath = outputPath.empty();

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || ( bInvalidFile && bInvalidFileList ) || bInvalidOutputPath )
	{
		PrintUsageHelp();
		return 0;
	}

	// Make sure that set only one flag
	if ( !( bInvalidFile ^ bInvalidFileList ) )
	{
		Error( "ResourceCompiler: Must be set only 'file' or 'filelist'" );
		return 1;
	}

	// Get directory with the file list
	{
		eastl::string tmpBuffer;
		S_GetFilePath( bInvalidFileList ? pFile : pFileList, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, workDir, "" );
	}

	// Append a path separator to the output path and convert it into an absolute path
	S_AppendPathSeparator( outputPath );
	if ( !S_IsAbsolutePath( outputPath ) )
	{
		eastl::string tempBuffer = eastl::move( outputPath );
		S_MakeAbsolutePath( tempBuffer, outputPath );
	}

	// Add the file/filelist to the compile list
	if ( bInvalidFileList && !AddFileToCompile( pFile, workDir.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to add file '%s' to compile", pFile );
		return 1;
	}
	else if ( bInvalidFile && !LoadFileList( pFileList ) )
	{
		Error( "ResourceCompiler: Failed to load file list '%s'", pFileList );
		return 1;
	}

	// Compile each file by tools
	bool bResult = true;
	for ( auto it = files.begin(), itEnd = files.end(); it != itEnd; ++it )
	{
		bool				  bResourceCompiled = false;
		const resourceFile_t& resourceFile		= *it;
		switch ( resourceFile.type )
		{
		case RESOURCE_SOURCE_TYPE_TEXTURE: bResourceCompiled = CompileTexture( resourceFile ); break;
		case RESOURCE_SOURCE_TYPE_MATERIAL: bResourceCompiled = CompileMaterial( resourceFile ); break;
		case RESOURCE_SOURCE_TYPE_MODEL: bResourceCompiled = CompileModel( resourceFile ); break;
		case RESOURCE_SOURCE_TYPE_ENTITY_DESC: bResourceCompiled = CompileEntityDesc( resourceFile ); break;
		case RESOURCE_SOURCE_TYPE_MAP: bResourceCompiled = CompileMap( resourceFile ); break;
		default:
			Error( "ResourceCompiler: Failed to compile '%s', unsupported resource type '%s'", resourceFile.srcPath.c_str(), ConvResourceSourceTypeToString( resourceFile.type ) );
			bResult = false;
			continue;
		}

		if ( !bResourceCompiled )
		{
			Error( "ResourceCompiler: Failed to compile '%s'", resourceFile.srcPath.c_str() );
			bResult = false;
		}
		else
		{
			Msg( "ResourceCompiler: Resource '%s' compiled", resourceFile.srcPath.c_str() );
		}
	}

	return bResult ? 0 : 2;
}

/*
==================
CResourceCompilerApp::Shutdown
==================
*/
void CResourceCompilerApp::Shutdown()
{
	RemoveGroup( &resourceToolsSystemGroup );
	BaseClass::Shutdown();
}

/*
==================
CResourceCompilerApp::PrintUsageHelp
==================
*/
void CResourceCompilerApp::PrintUsageHelp()
{
	Msg( "" );
	Msg( "Resource compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage resourcecompiler -file <path> -output <dir>" );
	Msg( "Usage resourcecompiler -filelist <path> -output <dir>" );
	Msg( "" );
	Msg( "The compiler supports next resource types: texture (*.stex), material (*.smat), model (*.smdl), entity (*.sent) and map (*.smap)" );
	Msg( "For syntax example see 'content/core/meterials/default_tex.stex', 'content/core/meterials/default_mat.smat'" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "file\tSpecify a JSON file containing settings to compile the resource" );
	Msg( "filelist\tSpecify a JSON file containing a list of files to be processed as inputs. For syntax example see 'content/core/resourcelist.txt'" );
	Msg( "output <dir>\tOutput directory path" );
	Msg( "" );
}

/*
==================
CResourceCompilerApp::LoadFileList
==================
*/
bool CResourceCompilerApp::LoadFileList( const char* pPath )
{
	Msg( "ResourceCompiler: Load file list '%s'", pPath );

	// Load key values file
	CKeyValues keyValues( "filelist" );
	if ( !keyValues.LoadFromFile( pPath ) )
	{
		return false;
	}

	// Get resources
	bool bResult = true;
	for ( CKeyValuesSubKeysIterator it( &keyValues, "file" ); it; ++it )
	{
		// Make sure that a file in work directory
		eastl::string absoluteFilePath;
		S_MakeAbsolutePath( it->GetString( NULL ), absoluteFilePath, workDir );
		if ( S_Strnicmp( absoluteFilePath.c_str(), workDir.c_str(), (uint32)workDir.size() ) )
		{
			Warning( "ResourceCompiler: Resource file '%s' must be in '%s'", absoluteFilePath.c_str(), workDir.c_str() );
			bResult = false;
			continue;
		}

		// Add the file to compile list
		if ( !AddFileToCompile( absoluteFilePath.c_str(), workDir.c_str() ) )
		{
			bResult = false;
			continue;
		}
	}

	// We are done
	return bResult;
}

/*
==================
CResourceCompilerApp::AddFileToCompile
==================
*/
bool CResourceCompilerApp::AddFileToCompile( const char* pPath, const char* pWorkDir )
{
	// Convert of the file path to absolute
	eastl::string		 absoluteSourceFilePath;
	resourceSourceType_t resourceSourceType;
	S_MakeAbsolutePath( pPath, absoluteSourceFilePath, pWorkDir );

	// Get resource type by the file extension
	if ( !ConvScriptFileExtensionToResourceSourceType( S_GetFileExtension( absoluteSourceFilePath.c_str() ), resourceSourceType ) )
	{
		Warning( "ResourceCompiler: Unknown resource type for file '%s'", absoluteSourceFilePath.c_str() );
		return false;
	}

	// Get an absolute destination path
	eastl::string tempBuffer;
	eastl::string sourceFileDir;
	eastl::string baseFileName;
	S_MakeRelativePath( absoluteSourceFilePath, pWorkDir, tempBuffer, false );
	S_GetFilePath( tempBuffer, sourceFileDir, false );
	S_GetFileBaseName( absoluteSourceFilePath, baseFileName, false );
	S_AppendPathSeparator( sourceFileDir );

	// Add into a list the file
	resourceFile_t resourceFile = {};
	resourceFile.srcPath		= absoluteSourceFilePath;
	resourceFile.destPath		= S_Sprintf( "%s%s%s", outputPath.c_str(), sourceFileDir.c_str(), baseFileName.c_str() );
	resourceFile.type			= resourceSourceType;

	Msg( "ResourceCompiler: File '%s' added to compile as '%s'", absoluteSourceFilePath.c_str(), ConvResourceSourceTypeToString( resourceSourceType ) );
	files.emplace_back( resourceFile );
	return true;
}

/*
==================
CResourceCompilerApp::CompileTexture
==================
*/
bool CResourceCompilerApp::CompileTexture( const resourceFile_t& resourceFile ) const
{
	CSTEXSourceTextureDoc stexSourceFile;
	if ( !stexSourceFile.LoadFromFile( resourceFile.srcPath.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to load STEX file '%s'", resourceFile.srcPath.c_str() );
		return false;
	}

	// Get path to directory with the source file
	eastl::string sourceFileDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( resourceFile.srcPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
	}

	// Get a absolute source texture paths
	eastl::vector<eastl::string> srcPaths;
	eastl::vector<const char*>	 cSrcPaths;
	{
		const eastl::vector<eastl::string>& originalSrcPaths = stexSourceFile.GetSourcePaths();
		srcPaths.resize( originalSrcPaths.size() );
		cSrcPaths.resize( originalSrcPaths.size() );
		for ( uint32 sourcePathIdx = 0, numSourcePaths = (uint32)originalSrcPaths.size(); sourcePathIdx < numSourcePaths; ++sourcePathIdx )
		{
			eastl::string& srcPath = srcPaths[sourcePathIdx];
			S_MakeAbsolutePath( originalSrcPaths[sourcePathIdx], srcPath, sourceFileDir );
			cSrcPaths[sourcePathIdx] = srcPath.c_str();
		}
	}

	resourceToolCompileTextureParams_t compileParams = {};
	compileParams.bGenerateMipMaps					 = stexSourceFile.IsNeedGenerateMipMaps();
	compileParams.type								 = stexSourceFile.GetType();
	compileParams.addressModeU						 = stexSourceFile.GetAddressModeU();
	compileParams.addressModeV						 = stexSourceFile.GetAddressModeV();
	compileParams.addressModeW						 = stexSourceFile.GetAddressModeW();
	compileParams.filter							 = stexSourceFile.GetFilter();
	compileParams.pixelFormat						 = stexSourceFile.GetPixelFormat();
	compileParams.maxAnisotropy						 = stexSourceFile.GetMaxAnisotropy();
	compileParams.numSrcPaths						 = (uint32)cSrcPaths.size();
	compileParams.ppSrcPaths						 = cSrcPaths.data();
	compileParams.pDestPath							 = resourceFile.destPath.c_str();
	return pTextureTool->CompileTexture( compileParams );
}

/*
==================
CResourceCompilerApp::CompileMaterial
==================
*/
bool CResourceCompilerApp::CompileMaterial( const resourceFile_t& resourceFile ) const
{
	CSMATSourceMaterialDoc smatSourceFile;
	if ( !smatSourceFile.LoadFromFile( resourceFile.srcPath.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to load SMAT file '%s'", resourceFile.srcPath.c_str() );
		return false;
	}

	// Get material variables
	eastl::vector<resourceToolMaterialVar_t> resourceToolMaterialVars;
	resourceToolMaterialVars.resize( smatSourceFile.GetNumVars() );
	{
		bool								   bMaterialVarsAreVaild  = true;
		const eastl::vector<CSMATMaterialVar>& smatSourceMaterialVars = smatSourceFile.GetVars();
		for ( uint32 varIdx = 0, numVars = smatSourceFile.GetNumVars(); varIdx < numVars; ++varIdx )
		{
			const CSMATMaterialVar&	   smatSourceMaterialVar   = smatSourceMaterialVars[varIdx];
			resourceToolMaterialVar_t& resourceToolMaterialVar = resourceToolMaterialVars[varIdx];
			resourceToolMaterialVar.pName					   = smatSourceMaterialVar.GetName();
			switch ( smatSourceMaterialVar.GetType() )
			{
			case SMAT_MATERIAL_VAR_TYPE_UNDEFINED:
				resourceToolMaterialVar.type = RESOURCE_TOOL_MATERIAL_VAR_TYPE_UNDEFINED;
				break;

			case SMAT_MATERIAL_VAR_TYPE_BOOL:
				resourceToolMaterialVar.boolValue = smatSourceMaterialVar.GetBoolValue();
				resourceToolMaterialVar.type	  = RESOURCE_TOOL_MATERIAL_VAR_TYPE_BOOL;
				break;

			case SMAT_MATERIAL_VAR_TYPE_INT:
				resourceToolMaterialVar.intValue = smatSourceMaterialVar.GetIntValue();
				resourceToolMaterialVar.type	 = RESOURCE_TOOL_MATERIAL_VAR_TYPE_INT;
				break;

			case SMAT_MATERIAL_VAR_TYPE_FLOAT:
				resourceToolMaterialVar.floatValue = smatSourceMaterialVar.GetFloatValue();
				resourceToolMaterialVar.type	   = RESOURCE_TOOL_MATERIAL_VAR_TYPE_FLOAT;
				break;

			case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
				smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector2DValue.x, 2 );
				resourceToolMaterialVar.type = RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_2D;
				break;

			case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
				smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector3DValue.x, 3 );
				resourceToolMaterialVar.type = RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_3D;
				break;

			case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
				smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector4DValue.x, 4 );
				resourceToolMaterialVar.type = RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_4D;
				break;

			case SMAT_MATERIAL_VAR_TYPE_MATRIX:
				resourceToolMaterialVar.matrixValue = smatSourceMaterialVar.GetMatrixValue();
				resourceToolMaterialVar.type		= RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATRIX;
				break;

			case SMAT_MATERIAL_VAR_TYPE_STRING:
				resourceToolMaterialVar.pStringValue = smatSourceMaterialVar.GetStringValue();
				resourceToolMaterialVar.type		 = RESOURCE_TOOL_MATERIAL_VAR_TYPE_STRING;
				break;

			case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
				resourceToolMaterialVar.pTextureValue = smatSourceMaterialVar.GetTextureValue();
				resourceToolMaterialVar.type		  = RESOURCE_TOOL_MATERIAL_VAR_TYPE_TEXTURE;
				break;

			case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
				resourceToolMaterialVar.pMaterialValue = smatSourceMaterialVar.GetMaterialValue();
				resourceToolMaterialVar.type		   = RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATERIAL;
				break;

			default:
				Error( "ResourceCompiler: Unknown material type 0x%X in variable '%s' of '%s'", smatSourceMaterialVar.GetType(), resourceToolMaterialVar.pName, resourceFile.srcPath.c_str() );
				bMaterialVarsAreVaild = false;
				break;
			}
		}

		if ( !bMaterialVarsAreVaild )
		{
			return false;
		}
	}

	resourceToolCompileMaterialParams_t compileParams = {};
	compileParams.pDestPath							  = resourceFile.destPath.c_str();
	compileParams.pShaderName						  = smatSourceFile.GetShaderName();
	compileParams.pVars								  = resourceToolMaterialVars.data();
	compileParams.numVars							  = smatSourceFile.GetNumVars();
	return pMaterialTool->CompileMaterial( compileParams );
}

/*
==================
CResourceCompilerApp::CompileModel
==================
*/
bool CResourceCompilerApp::CompileModel( const resourceFile_t& resourceFile ) const
{
	CSMDLSourceModelDoc smdlSourceFile;
	if ( !smdlSourceFile.LoadFromFile( resourceFile.srcPath.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to load SMDL file '%s'", resourceFile.srcPath.c_str() );
		return false;
	}

	// Convert renamed materials for SMDL tool
	eastl::vector<resourceToolRenamedMaterial_t> resourceToolRenamedMaterials;
	{
		const eastl::unordered_map<eastl::string, eastl::string>& renamedMaterialsDict = smdlSourceFile.GetRenamedMaterials();
		for ( auto it = renamedMaterialsDict.begin(), itEnd = renamedMaterialsDict.end(); it != itEnd; ++it )
		{
			resourceToolRenamedMaterials.emplace_back( resourceToolRenamedMaterial_t{ it->first.c_str(), it->second.c_str() } );
		}
	}

	// Get path to directory with the source file
	eastl::string sourceFileDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( resourceFile.srcPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
	}

	// Get an absolute source path
	eastl::string srcPath;
	S_MakeAbsolutePath( smdlSourceFile.GetSourcePath(), srcPath, sourceFileDir );

	resourceToolCompileModelParams_t compileParams = {};
	compileParams.bCombineModels				   = smdlSourceFile.IsCombineModels();
	compileParams.axisUp						   = smdlSourceFile.GetAxisUp();
	compileParams.pMaterialsDir					   = smdlSourceFile.GetMaterialsDir().c_str();
	compileParams.pSrcPath						   = srcPath.c_str();
	compileParams.pDestPath						   = resourceFile.destPath.c_str();
	compileParams.pRenamedMaterials				   = resourceToolRenamedMaterials.data();
	compileParams.numRenamedMaterials			   = (uint32)resourceToolRenamedMaterials.size();
	return pModelTool->CompileModel( compileParams );
}

/*
==================
CResourceCompilerApp::CompileEntityDesc
==================
*/
bool CResourceCompilerApp::CompileEntityDesc( const resourceFile_t& resourceFile ) const
{
	CSENTSourceEntityDescDoc sentSourceFile;
	if ( !sentSourceFile.LoadFromFile( resourceFile.srcPath.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to load SENT file '%s'", resourceFile.srcPath.c_str() );
		return false;
	}

	// Get entity descriptor components
	eastl::vector<resourceToolEntityDescVar_t>		 resourceToolEntityDescVars;
	eastl::vector<resourceToolEntityDescComponent_t> resourceToolEntityDescComponents;
	resourceToolEntityDescComponents.resize( sentSourceFile.GetNumComponents() );
	{
		bool										   bEntityDescComponentsAreVaild	= true;
		uint32										   resourceToolEntityDescVarsOffset = 0;
		const eastl::vector<CSENTEntityDescComponent>& sentEntityDescComponents			= sentSourceFile.GetComponents();
		for ( uint32 componentIdx = 0, numComponents = sentSourceFile.GetNumComponents(); componentIdx < numComponents; ++componentIdx )
		{
			const CSENTEntityDescComponent&			 sentEntityDescComponent		 = sentEntityDescComponents[componentIdx];
			const eastl::vector<CSENTEntityDescVar>& sentEntityDescVars				 = sentEntityDescComponent.GetVars();
			resourceToolEntityDescComponent_t&		 resourceToolEntityDescComponent = resourceToolEntityDescComponents[componentIdx];
			resourceToolEntityDescComponent.pType									 = sentEntityDescComponent.GetType();

			// Get entity descriptor vars
			if ( !sentEntityDescVars.empty() )
			{
				resourceToolEntityDescComponent.numVars = sentEntityDescComponent.GetNumVars();
				resourceToolEntityDescVars.resize( resourceToolEntityDescVarsOffset + resourceToolEntityDescComponent.numVars );
				resourceToolEntityDescComponent.pVars = resourceToolEntityDescVars.data() + resourceToolEntityDescVarsOffset;

				for ( uint32 varIdx = 0, numVars = sentEntityDescComponent.GetNumVars(); varIdx < numVars; ++varIdx )
				{
					const CSENTEntityDescVar&	 sentEntityDescVar		   = sentEntityDescVars[varIdx];
					resourceToolEntityDescVar_t& resourceToolEntityDescVar = resourceToolEntityDescVars[resourceToolEntityDescVarsOffset + varIdx];
					resourceToolEntityDescVar.pName						   = sentEntityDescVar.GetName();
					switch ( sentEntityDescVar.GetType() )
					{
					case SENT_ENTITY_DESC_VAR_TYPE_UNDEFINED:
						resourceToolEntityDescVar.type = RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_UNDEFINED;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
						resourceToolEntityDescVar.boolValue = sentEntityDescVar.GetBoolValue();
						resourceToolEntityDescVar.type		= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_BOOL;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_INT:
						resourceToolEntityDescVar.intValue = sentEntityDescVar.GetIntValue();
						resourceToolEntityDescVar.type	   = RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_INT;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
						resourceToolEntityDescVar.floatValue = sentEntityDescVar.GetFloatValue();
						resourceToolEntityDescVar.type		 = RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_FLOAT;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
						resourceToolEntityDescVar.vector2DValue = sentEntityDescVar.GetVec2Value();
						resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_2D;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
						resourceToolEntityDescVar.vector3DValue = sentEntityDescVar.GetVec3Value();
						resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_3D;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
						resourceToolEntityDescVar.vector4DValue = sentEntityDescVar.GetVec4Value();
						resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_4D;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
						resourceToolEntityDescVar.matrixValue = sentEntityDescVar.GetMatrixValue();
						resourceToolEntityDescVar.type		  = RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_MATRIX;
						break;

					case SENT_ENTITY_DESC_VAR_TYPE_STRING:
						resourceToolEntityDescVar.pStringValue = sentEntityDescVar.GetStringValue();
						resourceToolEntityDescVar.type		   = RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_STRING;
						break;

					default:
						Error( "ResourceCompiler: Unknown entity descriptor type 0x%X in variable '%s' of '%s'", sentEntityDescVar.GetType(), resourceToolEntityDescVar.pName, resourceFile.srcPath.c_str() );
						bEntityDescComponentsAreVaild = false;
						break;
					}
				}
				resourceToolEntityDescVarsOffset += resourceToolEntityDescComponent.numVars;
			}
		}

		if ( !bEntityDescComponentsAreVaild )
		{
			return false;
		}
	}

	resourceToolCompileEntityDescParams_t compileParams = {};
	compileParams.pDestPath								= resourceFile.destPath.c_str();
	compileParams.pComponents							= resourceToolEntityDescComponents.data();
	compileParams.numComponents							= (uint32)resourceToolEntityDescComponents.size();
	return pEntityDescTool->CompileEntityDesc( compileParams );
}

/*
==================
CResourceCompilerApp::CompileMap
==================
*/
bool CResourceCompilerApp::CompileMap( const resourceFile_t& resourceFile ) const
{
	CSMAPSourceMapDoc smapSourceFile;
	if ( !smapSourceFile.LoadFromFile( resourceFile.srcPath.c_str() ) )
	{
		Error( "ResourceCompiler: Failed to load SMAP file '%s'", resourceFile.srcPath.c_str() );
		return false;
	}

	// Get entities
	eastl::vector<resourceToolMapEntity_t> entities;
	entities.resize( smapSourceFile.GetNumEntities() );
	{
		const eastl::vector<CSMAPEntity>& smapEntities = smapSourceFile.GetEntities();
		for ( uint32 entityIdx = 0, numEntities = (uint32)smapEntities.size(); entityIdx < numEntities; ++entityIdx )
		{
			const CSMAPEntity&		 smapEntity			   = smapEntities[entityIdx];
			resourceToolMapEntity_t& resourceToolMapEntity = entities[entityIdx];
			resourceToolMapEntity.pClassName			   = smapEntity.GetClassName();
			resourceToolMapEntity.pName					   = smapEntity.GetName();
		}
	}

	resourceToolCompileMapParams_t compileParams = {};
	compileParams.pDestPath						 = resourceFile.destPath.c_str();
	compileParams.pEntities						 = entities.data();
	compileParams.numEntities					 = (uint32)entities.size();
	return pMapTool->CompileMap( compileParams );
}

/*
==================
CResourceCompilerApp::GetAppInfo
==================
*/
const appInfo_t& CResourceCompilerApp::GetAppInfo() const
{
	static appInfo_t s_appInfo{ "resourcecompiler", APPLICATION_TYPE_CONSOLE, FCVAR_NONE, NULL, NULL };
	return s_appInfo;
}

/*
==================
CResourceCompilerApp::GetCrashDumpInfo
==================
*/
const crashDumpAppInfo_t& CResourceCompilerApp::GetCrashDumpInfo() const
{
	static crashDumpAppInfo_t s_crashDumpAppInfo{ "Resource Compiler", __DATE__ " " __TIME__, NULL, NULL };
	return s_crashDumpAppInfo;
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

	return CResourceCompilerApp( commandLine.c_str() ).Run();
}
