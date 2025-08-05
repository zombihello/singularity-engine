#include "pch_resourcecompiler.h"
#include "core/icommandline.h"
#include "core/crashdump.h"
#include "stdlib/jsondoc.h"
#include "stexdoc/stex_source_doc.h"
#include "smatdoc/smat_source_doc.h"
#include "smdldoc/smdl_source_doc.h"
#include "sentdoc/sent_source_doc.h"
#include "smapdoc/smap_source_doc.h"
#include "filesystem/ifilesystem.h"
#include "cvar/icvar.h"
#include "tools/resource_tools/itexture_tool.h"
#include "tools/resource_tools/imaterial_tool.h"
#include "tools/resource_tools/imodel_tool.h"
#include "tools/resource_tools/ientitydesc_tool.h"
#include "tools/resource_tools/imap_tool.h"
#include "appframework/appframework.h"

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
static const achar* s_pScriptFileExtensions[] =
{
	"smat",					// RESOURCE_SOURCE_TYPE_MATERIAL
	"stex",					// RESOURCE_SOURCE_TYPE_TEXTURE
	"smdl",					// RESOURCE_SOURCE_TYPE_MODEL
	"sent",					// RESOURCE_SOURCE_TYPE_ENTITY_DESC
	"smap"					// RESOURCE_SOURCE_TYPE_MAP
};
static_assert( RESOURCE_SOURCE_NUM_TYPES == ARRAYSIZE( s_pScriptFileExtensions ), "Array size 's_pScriptFileExtensions' must be equal to RESOURCE_SOURCE_NUM_TYPES" );

// Table for convert resourceSourceType_t to text
static const achar* s_pResourceSourceTypeNames[] =
{
	"material",				// RESOURCE_SOURCE_TYPE_MATERIAL
	"texture",				// RESOURCE_SOURCE_TYPE_TEXTURE
	"model",				// RESOURCE_SOURCE_TYPE_MODEL
	"entity descriptor",	// RESOURCE_SOURCE_TYPE_ENTITY_DESC
	"map"					// RESOURCE_SOURCE_TYPE_MAP
};
static_assert( RESOURCE_SOURCE_NUM_TYPES == ARRAYSIZE( s_pResourceSourceTypeNames ), "Array size 's_pResourceSourceTypeNames' must be equal to RESOURCE_SOURCE_NUM_TYPES" );


/*
==================
ConvScriptFileExtensionToResourceSourceType
==================
*/
static bool ConvScriptFileExtensionToResourceSourceType( const achar* pFileExtension, resourceSourceType_t& resourceSourceType )
{
	for ( uint32 index = 0; index < ARRAYSIZE( s_pScriptFileExtensions ); ++index )
	{
		if ( s_pScriptFileExtensions[index] && !S_Stricmp( pFileExtension, s_pScriptFileExtensions[index] ) )
		{
			// We found! Return current type
			resourceSourceType = ( resourceSourceType_t )index;
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
static const achar* ConvResourceSourceTypeToString( resourceSourceType_t resourceSourceType )
{
	if ( resourceSourceType >= RESOURCE_SOURCE_NUM_TYPES )
	{
		return "unknown";
	}
	return s_pResourceSourceTypeNames[resourceSourceType];
}


//-----------------------------------------------------------------------------
// Resource compiler app system group
//-----------------------------------------------------------------------------
class CResourceCompilerAppSystemGroup : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	// IAppSystemGroup interface
	// An installed application creation function, you should tell the group
	// the DLLs and the singleton interfaces you want to instantiate
	// Return FALSE if there's any problems and the app will abort
	virtual bool Create() override;

	// Main loop implemented by the application
	// Return exit code. If all ok returns zero
	virtual int32 Main() override;

	CResourceCompilerAppSystemGroup();

private:
	struct resourceFile_t
	{
		~resourceFile_t() 
		{}

		std::string				sourcePath;
		resourceSourceType_t	type;
	};

	void PrintUsageHelp();
	bool LoadFileList( const achar* pPath );
	bool AddFileToCompile( const achar* pPath, const achar* pWorkDir = "" );

	ITextureTool*				pTextureTool;
	IMaterialTool*				pMaterialTool;
	IModelTool*					pModelTool;
	IEntityDescTool*			pEntityDescTool;
	IMapTool*					pMapTool;
	std::list<resourceFile_t>	files;
};


/*
==================
CResourceCompilerAppSystemGroup::CResourceCompilerAppSystemGroup
==================
*/
CResourceCompilerAppSystemGroup::CResourceCompilerAppSystemGroup()
	: pTextureTool( NULL )
	, pMaterialTool( NULL )
	, pModelTool( NULL )
	, pEntityDescTool( NULL )
	, pMapTool( NULL )
{}

/*
==================
CResourceCompilerAppSystemGroup::Create
==================
*/
bool CResourceCompilerAppSystemGroup::Create()
{
	// Load application systems
	appSystemInfo_t		appSystemInfos[] =
	{
		{ "texture_tool"		DLL_EXT_STRING,			TEXTURE_TOOL_INTERFACE_VERSION		},
		{ "material_tool"		DLL_EXT_STRING,			MATERIAL_TOOL_INTERFACE_VERSION		},
		{ "model_tool"			DLL_EXT_STRING,			MODEL_TOOL_INTERFACE_VERSION		},
		{ "entitydesc_tool"		DLL_EXT_STRING,			ENTITYDESC_TOOL_INTERFACE_VERSION	},
		{ "map_tool"			DLL_EXT_STRING,			MAP_TOOL_INTERFACE_VERSION			},
		{ "", "" }																			// Required to terminate the list
	};

	// Add all systems from array
	if ( !AddSystems( appSystemInfos ) )
	{
		return false;
	}

	// Get a texture, material, model and entity tool
	pTextureTool	= ( ITextureTool* )FindSystem( TEXTURE_TOOL_INTERFACE_VERSION );
	pMaterialTool	= ( IMaterialTool* )FindSystem( MATERIAL_TOOL_INTERFACE_VERSION );
	pModelTool		= ( IModelTool* )FindSystem( MODEL_TOOL_INTERFACE_VERSION );
	pEntityDescTool	= ( IEntityDescTool* )FindSystem( ENTITYDESC_TOOL_INTERFACE_VERSION );
	pMapTool		= ( IMapTool* )FindSystem( MAP_TOOL_INTERFACE_VERSION );

	// Initialize a default pixel format
	for ( uint32 index = 0; index < STUDIOAPI_PIXEL_NUM_FORMATS; ++index )
	{
		if ( !pTextureTool->IsSupportPixelFormat( ( studioAPIPixelFormat_t )index ) )
		{
			continue;
		}

		// First pixel format which support by the Singularity texture tool it is our default format
		CSTEXSourceTextureDoc::SetDefaultPixelFormat( ( studioAPIPixelFormat_t )index );
		break;
	}

	// We are done
	return true;
}

/*
==================
CResourceCompilerAppSystemGroup::Main
==================
*/
int32 CResourceCompilerAppSystemGroup::Main()
{
	// Is need to print help of usage
	bool	bPrintHelpUsage = CommandLine()->HasParam( "h" ) || CommandLine()->HasParam( "help" ) || CommandLine()->HasParam( "?" );

	// Get and parse a file list
	const achar*	pFileListPath			= CommandLine()->GetFirstValue( "filelist" );
	const achar*	pFilePath				= CommandLine()->GetFirstValue( "file" );
	bool			bInvalidFileListPath	= !pFileListPath || pFileListPath[0] == '\0';
	bool			bInvalidFilePath		= !pFilePath || pFilePath[0] == '\0';
	bool			bInvalidFilePaths		= bInvalidFileListPath && bInvalidFilePath;
	if ( !bInvalidFilePaths && !bInvalidFileListPath && !LoadFileList( pFileListPath ) )
	{
		Error( "ResourceCompiler: Failed to load file list '%s'", pFileListPath );
		return 1;
	}

	// Add the file to the file list
	if ( !bInvalidFilePaths && !bInvalidFilePath && !AddFileToCompile( pFilePath ) )
	{
		Error( "ResourceCompiler: Failed to add file '%s' to compile", pFilePath );
		return 1;
	}

	// Print help of usage if it need or some parameters aren't set
	if ( bPrintHelpUsage || bInvalidFilePaths )
	{
		PrintUsageHelp();
		return 0;
	}

	// Compile each file by tools
	bool	bResult = true;
	for ( auto it = files.begin(), itEnd = files.end(); it != itEnd; ++it )
	{
		bool					bResourceCompiled	= false;
		const resourceFile_t&	resourceFile		= *it;
		switch ( resourceFile.type )
		{
			// Compile a texture
		case RESOURCE_SOURCE_TYPE_TEXTURE:
		{
			CSTEXSourceTextureDoc		stexSourceFile;
			if ( !stexSourceFile.LoadFromFile( resourceFile.sourcePath.c_str() ) )
			{
				Error( "ResourceCompiler: Failed to load STEX file '%s'", resourceFile.sourcePath.c_str() );
				bResult = false;
				continue;
			}

			// Get path to directory with the source file
			std::string		sourceFileDir;
			{
				std::string		tmpBuffer;
				S_GetFilePath( resourceFile.sourcePath, tmpBuffer, false );
				S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
			}

			// Get a absolute source texture paths
			std::vector<std::string>	srcPaths;
			std::vector<const achar*>	cSrcPaths;
			{
				const std::vector<std::string>	originalSrcPaths = stexSourceFile.GetSourcePaths();
				srcPaths.resize( originalSrcPaths.size() );
				cSrcPaths.resize( originalSrcPaths.size() );
				for ( uint32 sourcePathIdx = 0, numSourcePaths = ( uint32 )originalSrcPaths.size(); sourcePathIdx < numSourcePaths; ++sourcePathIdx )
				{
					std::string&		srcPath = srcPaths[sourcePathIdx];
					S_MakeAbsolutePath( originalSrcPaths[sourcePathIdx], srcPath, sourceFileDir );
					cSrcPaths[sourcePathIdx]	= srcPath.c_str();
				}
			}

			// Get a destination file
			std::string		destPath;
			{
				std::string		outputDir = stexSourceFile.GetOutputDir();
				if ( !S_IsAbsolutePath( outputDir ) )
				{
					S_MakeAbsolutePath( stexSourceFile.GetOutputDir(), outputDir, sourceFileDir );
				}

				std::string		baseFileName;
				S_GetFileBaseName( resourceFile.sourcePath, baseFileName );

				S_AppendPathSeparator( outputDir );
				destPath = S_Sprintf( "%s%s", outputDir.c_str(), baseFileName.c_str() );
			}
			
			resourceToolCompileTextureParams_t	compileParams = {};
			compileParams.bGenerateMipMaps		= stexSourceFile.IsNeedGenerateMipMaps();
			compileParams.type					= stexSourceFile.GetType();
			compileParams.addressModeU			= stexSourceFile.GetAddressModeU();
			compileParams.addressModeV			= stexSourceFile.GetAddressModeV();
			compileParams.addressModeW			= stexSourceFile.GetAddressModeW();
			compileParams.filter				= stexSourceFile.GetFilter();
			compileParams.pixelFormat			= stexSourceFile.GetPixelFormat();
			compileParams.maxAnisotropy			= stexSourceFile.GetMaxAnisotropy();
			compileParams.numSrcPaths			= ( uint32 )cSrcPaths.size();
			compileParams.ppSrcPaths			= cSrcPaths.data();
			compileParams.pDestPath				= destPath.c_str();
			bResourceCompiled					= pTextureTool->CompileTexture( compileParams );
			break;
		}

			// Compile a material
		case RESOURCE_SOURCE_TYPE_MATERIAL:
		{
			CSMATSourceMaterialDoc		smatSourceFile;
			if ( !smatSourceFile.LoadFromFile( resourceFile.sourcePath.c_str() ) )
			{
				Error( "ResourceCompiler: Failed to load SMAT file '%s'", resourceFile.sourcePath.c_str() );
				bResult = false;
				continue;
			}

			// Get material variables
			std::vector<resourceToolMaterialVar_t>			resourceToolMaterialVars;
			resourceToolMaterialVars.resize( smatSourceFile.GetNumVars() );
			{
				bool									bMaterialVarsAreVaild	= true;
				const std::vector<CSMATMaterialVar>&	smatSourceMaterialVars	= smatSourceFile.GetVars();
				for ( uint32 varIdx = 0, numVars = smatSourceFile.GetNumVars(); varIdx < numVars; ++varIdx )
				{
					const CSMATMaterialVar&			smatSourceMaterialVar		= smatSourceMaterialVars[varIdx];
					resourceToolMaterialVar_t&		resourceToolMaterialVar		= resourceToolMaterialVars[varIdx];
					resourceToolMaterialVar.pName	= smatSourceMaterialVar.GetName();
					switch ( smatSourceMaterialVar.GetType() )
					{
					case SMAT_MATERIAL_VAR_TYPE_BOOL:
						resourceToolMaterialVar.boolValue		= smatSourceMaterialVar.GetBoolValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_BOOL;
						break;

					case SMAT_MATERIAL_VAR_TYPE_INT: 
						resourceToolMaterialVar.intValue		= smatSourceMaterialVar.GetIntValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_INT;
						break;

					case SMAT_MATERIAL_VAR_TYPE_FLOAT: 
						resourceToolMaterialVar.floatValue		= smatSourceMaterialVar.GetFloatValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_FLOAT;
						break;

					case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D: 
						smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector2DValue.x, 2 );
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_2D;
						break;

					case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
						smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector3DValue.x, 3 );
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_3D;
						break;

					case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D: 
						smatSourceMaterialVar.GetVecValue( &resourceToolMaterialVar.vector4DValue.x, 4 );
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_VECTOR_4D;
						break;

					case SMAT_MATERIAL_VAR_TYPE_MATRIX:
						resourceToolMaterialVar.matrixValue		= smatSourceMaterialVar.GetMatrixValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATRIX;
						break;

					case SMAT_MATERIAL_VAR_TYPE_STRING:
						resourceToolMaterialVar.pStringValue	= smatSourceMaterialVar.GetStringValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_STRING;
						break;

					case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
						resourceToolMaterialVar.pTextureValue	= smatSourceMaterialVar.GetTextureValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_TEXTURE;
						break;

					case SMAT_MATERIAL_VAR_TYPE_MATERIAL: 
						resourceToolMaterialVar.pMaterialValue	= smatSourceMaterialVar.GetMaterialValue();
						resourceToolMaterialVar.type			= RESOURCE_TOOL_MATERIAL_VAR_TYPE_MATERIAL;
						break;

					default:
						Error( "ResourceCompiler: Unknown material type 0x%X in variable '%s' of '%s'", smatSourceMaterialVar.GetType(), resourceToolMaterialVar.pName, resourceFile.sourcePath.c_str() );
						bMaterialVarsAreVaild = false;
						break;
					}
				}

				if ( !bMaterialVarsAreVaild )
				{
					bResourceCompiled = false;
					break;
				}
			}

			// Get path to directory with the source file
			std::string		sourceFileDir;
			{
				std::string		tmpBuffer;
				S_GetFilePath( resourceFile.sourcePath, tmpBuffer, false );
				S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
			}

			// Get a destination file
			std::string		destPath;
			{
				std::string		outputDir = smatSourceFile.GetOutputDir();
				if ( !S_IsAbsolutePath( outputDir ) )
				{
					S_MakeAbsolutePath( smatSourceFile.GetOutputDir(), outputDir, sourceFileDir );
				}

				std::string		baseFileName;
				S_GetFileBaseName( resourceFile.sourcePath, baseFileName );

				S_AppendPathSeparator( outputDir );
				destPath = S_Sprintf( "%s%s", outputDir.c_str(), baseFileName.c_str() );
			}

			resourceToolCompileMaterialParams_t		compileParams = {};
			compileParams.pDestPath					= destPath.c_str();
			compileParams.pShaderName				= smatSourceFile.GetShaderName();
			compileParams.pVars						= resourceToolMaterialVars.data();
			compileParams.numVars					= smatSourceFile.GetNumVars();
			bResourceCompiled						= pMaterialTool->CompileMaterial( compileParams );
			break;
		}

			// Compile a model
		case RESOURCE_SOURCE_TYPE_MODEL:
		{
			CSMDLSourceModelDoc		smdlSourceFile;
			if ( !smdlSourceFile.LoadFromFile( resourceFile.sourcePath.c_str() ) )
			{
				Error( "ResourceCompiler: Failed to load SMDL file '%s'", resourceFile.sourcePath.c_str() );
				bResult = false;
				continue;
			}

			// Convert renamed materials for SMDL tool
			std::vector<resourceToolRenamedMaterial_t>		resourceToolRenamedMaterials;
			{
				const std::unordered_map<std::string, std::string>&		renamedMaterialsDict = smdlSourceFile.GetRenamedMaterials();
				for ( auto it = renamedMaterialsDict.begin(), itEnd = renamedMaterialsDict.end(); it != itEnd; ++it )
				{
					resourceToolRenamedMaterials.emplace_back( resourceToolRenamedMaterial_t{ it->first.c_str(), it->second.c_str() } );
				}
			}

			// Get path to directory with the source file
			std::string		sourceFileDir;
			{
				std::string		tmpBuffer;
				S_GetFilePath( resourceFile.sourcePath, tmpBuffer, false );
				S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
			}

			// Get an absolute source path
			std::string		srcPath;
			S_MakeAbsolutePath( smdlSourceFile.GetSourcePath(), srcPath, sourceFileDir );

			// Get a destination file
			std::string		destPath;
			{
				std::string		outputDir = smdlSourceFile.GetOutputDir();
				if ( !S_IsAbsolutePath( outputDir ) )
				{
					S_MakeAbsolutePath( smdlSourceFile.GetOutputDir(), outputDir, sourceFileDir );
				}

				std::string		baseFileName;
				S_GetFileBaseName( resourceFile.sourcePath, baseFileName );

				S_AppendPathSeparator( outputDir );
				destPath = S_Sprintf( "%s%s", outputDir.c_str(), baseFileName.c_str() );
			}

			resourceToolCompileModelParams_t	compileParams = {};
			compileParams.bCombineModels		= smdlSourceFile.IsCombineModels();
			compileParams.axisUp				= smdlSourceFile.GetAxisUp();
			compileParams.pMaterialsDir			= smdlSourceFile.GetMaterialsDir().c_str();
			compileParams.pSrcPath				= srcPath.c_str();
			compileParams.pDestPath				= destPath.c_str();
			compileParams.pRenamedMaterials		= resourceToolRenamedMaterials.data();
			compileParams.numRenamedMaterials	= ( uint32 )resourceToolRenamedMaterials.size();
			bResourceCompiled					= pModelTool->CompileModel( compileParams );
			break;
		}

			// Compile an entity descriptor
		case RESOURCE_SOURCE_TYPE_ENTITY_DESC:
		{
			CSENTSourceEntityDescDoc		sentSourceFile;
			if ( !sentSourceFile.LoadFromFile( resourceFile.sourcePath.c_str() ) )
			{
				Error( "ResourceCompiler: Failed to load SENT file '%s'", resourceFile.sourcePath.c_str() );
				bResult = false;
				continue;
			}

			// Get entity descriptor components
			std::vector<resourceToolEntityDescVar_t>			resourceToolEntityDescVars;
			std::vector<resourceToolEntityDescComponent_t>		resourceToolEntityDescComponents;
			resourceToolEntityDescComponents.resize( sentSourceFile.GetNumComponents() );
			{
				bool											bEntityDescComponentsAreVaild		= true;
				uint32											resourceToolEntityDescVarsOffset	= 0;
				const std::vector<CSENTEntityDescComponent>&	sentEntityDescComponents			= sentSourceFile.GetComponents();
				for ( uint32 componentIdx = 0, numComponents = sentSourceFile.GetNumComponents(); componentIdx < numComponents; ++componentIdx )
				{
					const CSENTEntityDescComponent&			sentEntityDescComponent			= sentEntityDescComponents[componentIdx];
					const std::vector<CSENTEntityDescVar>&	sentEntityDescVars				= sentEntityDescComponent.GetVars();
					resourceToolEntityDescComponent_t&		resourceToolEntityDescComponent = resourceToolEntityDescComponents[componentIdx];
					resourceToolEntityDescComponent.pType	= sentEntityDescComponent.GetType();
					
					// Get entity descriptor vars
					if ( !sentEntityDescVars.empty() )
					{
						resourceToolEntityDescComponent.numVars = sentEntityDescComponent.GetNumVars();
						resourceToolEntityDescVars.resize( resourceToolEntityDescVarsOffset + resourceToolEntityDescComponent.numVars );
						resourceToolEntityDescComponent.pVars	= resourceToolEntityDescVars.data() + resourceToolEntityDescVarsOffset;
						
						for ( uint32 varIdx = 0, numVars = sentEntityDescComponent.GetNumVars(); varIdx < numVars; ++varIdx )
						{
							const CSENTEntityDescVar&			sentEntityDescVar			= sentEntityDescVars[varIdx];
							resourceToolEntityDescVar_t&		resourceToolEntityDescVar	= resourceToolEntityDescVars[resourceToolEntityDescVarsOffset + varIdx];
							resourceToolEntityDescVar.pName		= sentEntityDescVar.GetName();
							switch ( sentEntityDescVar.GetType() )
							{
							case SENT_ENTITY_DESC_VAR_TYPE_BOOL:
								resourceToolEntityDescVar.boolValue		= sentEntityDescVar.GetBoolValue();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_BOOL;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_INT:
								resourceToolEntityDescVar.intValue		= sentEntityDescVar.GetIntValue();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_INT;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_FLOAT:
								resourceToolEntityDescVar.floatValue	= sentEntityDescVar.GetFloatValue();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_FLOAT;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_2D:
								resourceToolEntityDescVar.vector2DValue = sentEntityDescVar.GetVec2Value();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_2D;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_3D:
								resourceToolEntityDescVar.vector3DValue = sentEntityDescVar.GetVec3Value( );
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_3D;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_VECTOR_4D:
								resourceToolEntityDescVar.vector4DValue = sentEntityDescVar.GetVec4Value();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_VECTOR_4D;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_MATRIX:
								resourceToolEntityDescVar.matrixValue = sentEntityDescVar.GetMatrixValue();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_MATRIX;
								break;

							case SENT_ENTITY_DESC_VAR_TYPE_STRING:
								resourceToolEntityDescVar.pStringValue = sentEntityDescVar.GetStringValue();
								resourceToolEntityDescVar.type			= RESOURCE_TOOL_ENTITY_DESC_VAR_TYPE_STRING;
								break;

							default:
								Error( "ResourceCompiler: Unknown entity descriptor type 0x%X in variable '%s' of '%s'", sentEntityDescVar.GetType(), resourceToolEntityDescVar.pName, resourceFile.sourcePath.c_str() );
								bEntityDescComponentsAreVaild = false;
								break;
							}
						}
						resourceToolEntityDescVarsOffset += resourceToolEntityDescComponent.numVars;
					}
				}

				if ( !bEntityDescComponentsAreVaild )
				{
					bResourceCompiled = false;
					break;
				}
			}

			// Get path to directory with the source file
			std::string		sourceFileDir;
			{
				std::string		tmpBuffer;
				S_GetFilePath( resourceFile.sourcePath, tmpBuffer, false );
				S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
			}

			// Get a destination file
			std::string		destPath;
			{
				std::string		outputDir = sentSourceFile.GetOutputDir();
				if ( !S_IsAbsolutePath( outputDir ) )
				{
					S_MakeAbsolutePath( sentSourceFile.GetOutputDir(), outputDir, sourceFileDir );
				}

				std::string		baseFileName;
				S_GetFileBaseName( resourceFile.sourcePath, baseFileName );

				S_AppendPathSeparator( outputDir );
				destPath = S_Sprintf( "%s%s", outputDir.c_str(), baseFileName.c_str() );
			}

			resourceToolCompileEntityDescParams_t		compileParams = {};
			compileParams.pDestPath						= destPath.c_str();
			compileParams.pComponents					= resourceToolEntityDescComponents.data();
			compileParams.numComponents					= ( uint32 )resourceToolEntityDescComponents.size();
			bResourceCompiled							= pEntityDescTool->CompileEntityDesc( compileParams );
			break;
		}

			// Compile a map
		case RESOURCE_SOURCE_TYPE_MAP:
		{
			CSMAPSourceMapDoc		smapSourceFile;
			if ( !smapSourceFile.LoadFromFile( resourceFile.sourcePath.c_str() ) )
			{
				Error( "ResourceCompiler: Failed to load SMAP file '%s'", resourceFile.sourcePath.c_str() );
				bResult = false;
				continue;
			}

			// Get entities
			std::vector<resourceToolMapEntity_t>		entities;
			entities.resize( smapSourceFile.GetNumEntities() );
			{
				const std::vector<CSMAPEntity>&		smapEntities = smapSourceFile.GetEntities();
				for ( uint32 entityIdx = 0, numEntities = (uint32)smapEntities.size(); entityIdx < numEntities; ++entityIdx )
				{
					const CSMAPEntity&						smapEntity = smapEntities[entityIdx];
					resourceToolMapEntity_t&				resourceToolMapEntity = entities[entityIdx];
					resourceToolMapEntity.pEntityDesc		= smapEntity.GetEntityDesc();
					resourceToolMapEntity.pName				= smapEntity.GetName();
				}
			}

			// Get path to directory with the source file
			std::string		sourceFileDir;
			{
				std::string		tmpBuffer;
				S_GetFilePath( resourceFile.sourcePath, tmpBuffer, false );
				S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
			}

			// Get a destination file
			std::string		destPath;
			{
				std::string		outputDir = smapSourceFile.GetOutputDir();
				if ( !S_IsAbsolutePath( outputDir ) )
				{
					S_MakeAbsolutePath( smapSourceFile.GetOutputDir(), outputDir, sourceFileDir );
				}

				std::string		baseFileName;
				S_GetFileBaseName( resourceFile.sourcePath, baseFileName );

				S_AppendPathSeparator( outputDir );
				destPath = S_Sprintf( "%s%s", outputDir.c_str(), baseFileName.c_str() );
			}

			resourceToolCompileMapParams_t		compileParams = {};
			compileParams.pDestPath				= destPath.c_str();
			compileParams.pEntities				= entities.data();
			compileParams.numEntities			= ( uint32 )entities.size();
			bResourceCompiled					= pMapTool->CompileMap( compileParams );
			break;
		}

		default:
			Error( "ResourceCompiler: Failed to compile '%s', unsupported resource type '%s'", resourceFile.sourcePath.c_str(), ConvResourceSourceTypeToString( resourceFile.type ) );
			bResult = false;
			continue;
		}

		if ( !bResourceCompiled )
		{
			Error( "ResourceCompiler: Failed to compile '%s'", resourceFile.sourcePath.c_str() );
			bResult = false;
		}
		else
		{
			Msg( "ResourceCompiler: Resource '%s' compiled", resourceFile.sourcePath.c_str() );
		}
	}

	return bResult ? 0 : 2;
}

/*
==================
CResourceCompilerAppSystemGroup::PrintUsageHelp
==================
*/
void CResourceCompilerAppSystemGroup::PrintUsageHelp()
{
	Msg( "" );
	Msg( "Resource compiler for Singularity Engine (" __DATE__ " " __TIME__ ")" );
	Msg( "Usage resourcecompiler -file <path>" );
	Msg( "Usage resourcecompiler -filelist <path>" );
	Msg( "" );
	Msg( "The compiler supports next resource types: texture (*.stex), material (*.smat), model (*.smdl), entity (*.sent) and map (*.smap)" );
	Msg( "For syntax example see 'content/core/meterials/default_tex.stex', 'content/core/meterials/default_mat.smat'" );
	Msg( "" );
	Msg( "Launch arguments:" );
	Msg( "file\tSpecify a JSON file containing settings to compile the resource" );
	Msg( "filelist\tSpecify a JSON file containing a list of files to be processed as inputs. For syntax example see 'content/core/resourcelist.txt'" );
	Msg( "" );
}

/*
==================
CResourceCompilerAppSystemGroup::LoadFileList
==================
*/
bool CResourceCompilerAppSystemGroup::LoadFileList( const achar* pPath )
{
	Msg( "ResourceCompiler: Load file list '%s'", pPath );

	// Load a JSON file
	CJsonDoc	jsonFileList;
	if ( !jsonFileList.LoadFromFile( pPath ) )
	{
		Warning( "ResourceCompiler: Failed to load, maybe wrong JSON syntax?" );
		return false;
	}
	bool	bResult = true;

	// Get path to directory with the file list
	std::string		fileListDir;
	{
		std::string		tmpBuffer;
		S_GetFilePath( pPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, fileListDir, "", false );
	}

	// Get resource list
	CJsonValue		jsonResourceList = jsonFileList.GetValue( "resources" );
	if ( jsonResourceList.IsValid() && jsonResourceList.IsA( JSONVALUE_TYPE_ARRAY ) )
	{
		std::vector<CJsonValue>		jsonArray = jsonResourceList.GetArray();
		for ( uint32 index = 0, count = ( uint32 )jsonArray.size(); index < count; ++index )
		{
			const CJsonValue&	jsonElement = jsonArray[index];
			if ( !jsonElement.IsA( JSONVALUE_TYPE_STRING ) )
			{
				Warning( "ResourceCompiler: Invalid 'resources[%i]', must be string", index );
				bResult = false;
				continue;
			}

			// Add the file to compile
			if ( !AddFileToCompile( jsonElement.GetString().c_str(), fileListDir.c_str() ) )
			{
				bResult = false;
				continue;
			}
		}
	}
	else
	{
		Warning( "ResourceCompiler: File lists must have 'resources'" );
		bResult = false;
	}

	return bResult;
}

/*
==================
CResourceCompilerAppSystemGroup::AddFileToCompile
==================
*/
bool CResourceCompilerAppSystemGroup::AddFileToCompile( const achar* pPath, const achar* pWorkDir /* = "" */ )
{
	// Convert of the file path to absolute
	std::string				absoluteFilePath;
	resourceSourceType_t	resourceSourceType;
	S_MakeAbsolutePath( pPath, absoluteFilePath, pWorkDir );

	// Get resource type by the file extension
	if ( !ConvScriptFileExtensionToResourceSourceType( S_GetFileExtension( absoluteFilePath.c_str() ), resourceSourceType ) )
	{
		Warning( "ResourceCompiler: Unknown resource type for file '%s'", absoluteFilePath.c_str() );
		return false;
	}

	// Add into a list the file
	resourceFile_t			resourceFile	= {};
	resourceFile.sourcePath = absoluteFilePath;
	resourceFile.type		= resourceSourceType;

	Msg( "ResourceCompiler: File '%s' added to compile as '%s'", absoluteFilePath.c_str(), ConvResourceSourceTypeToString( resourceSourceType ) );
	files.emplace_back( resourceFile );
	return true;
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
	CrashDump_SetAppInfo( crashDumpAppInfo_t{ "Resource Compiler", __DATE__ " " __TIME__, NULL, NULL } );

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
	CResourceCompilerAppSystemGroup		resourceCompilerSystems;
	CApplication						application( &resourceCompilerSystems, "resourcecompiler" );
	return application.Run();
}