#include <cstdlib>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <meshoptimizer.h>

#include "tier1/filetools.h"
#include "utils/smdldoc/smdl_source_doc.h"
#include "utils/smdldoc/smdl_compiled_doc.h"
#include "tools/resourcecompiler/iresourcecompiler.h"

//-----------------------------------------------------------------------------
// Assimp mesh container
//-----------------------------------------------------------------------------
struct smdlAiMesh_t
{
	smdlAiMesh_t()
		: pMesh( NULL )
	{
	}
	smdlAiMesh_t( const aiMatrix4x4& transformation, aiMesh* pMesh )
		: transformation( transformation )
		, pMesh( pMesh )
	{
	}

	aiMatrix4x4 transformation;
	aiMesh*		pMesh;
};

// Assimp meshes dictionary type
// Material Index -> Ai meshes
typedef eastl::unordered_map<uint32, eastl::vector<smdlAiMesh_t>> smdlAiMeshesMap_t;

struct smdlMeshData_t
{
	eastl::string				name;
	eastl::vector<smdlVertex_t> vertices;
	eastl::vector<uint32>		indices;
	smdlSurface_t				surface;
	uint32						materialID;
};

/*
==================
SMdl_ChangeAxisUp
==================
*/
static void SMdl_ChangeAxisUp( aiVector3D& vector, axisUp_t axisUp )
{
	switch ( axisUp )
	{
	case AXIS_UP_MINUS_X:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_X:
		S_Swap( vector.y, vector.x );
		break;

	case AXIS_UP_MINUS_Y:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_Y:
		break;

	case AXIS_UP_MINUS_Z:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_Z:
		S_Swap( vector.y, vector.z );
		break;
	}
}

//-----------------------------------------------------------------------------
// Resource compiler for 'smdl'
//-----------------------------------------------------------------------------
class CResourceCompilerSMdl : public IResourceCompiler
{
public:
	// IResourceCompiler interface
	// Initialize and shutdown
	virtual bool Init( createInterfaceFn_t pFactory ) override;
	virtual void Shutdown() override;

	// Compile a resource
	// pSrcPath		Path to a source file (with file extension)
	// pDestPath	Path to a destination file (without file extension)
	virtual bool Compile( const char* pSrcPath, const char* pDestPath ) const override;

	// Get source formats and resource type name
	virtual uint32		GetNumSrcFormats() const override;
	virtual const char* GetSrcFormat( uint32 index ) const override;
	virtual const char* GetName() const override;

private:
	// Fill array meshes from the Assimp root scene
	void ProcessNode( aiNode* pNode, const aiScene* pScene, smdlAiMeshesMap_t& meshesDict ) const;
	bool ParseModels( const CSMDLSourceModelDoc& smdlSourceFile, const char* pSrcFileDir, eastl::vector<smdlMeshData_t>& parsedMeshes, eastl::vector<eastl::string>& parsedMaterials ) const;
	void OptimizeModel( smdlMeshData_t& meshData ) const;
};

EXPOSE_SINGLE_INTERFACE( CResourceCompilerSMdl, IResourceCompiler, RESOURCECOMPILER_INTERFACE_VERSION );

/*
==================
CResourceCompilerSMdl::Init
==================
*/
bool CResourceCompilerSMdl::Init( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CResourceCompilerSMdl::Shutdown
==================
*/
void CResourceCompilerSMdl::Shutdown()
{
	DisconnectTier1();
}

/*
==================
CResourceCompilerSMdl::Compile
==================
*/
bool CResourceCompilerSMdl::Compile( const char* pSrcPath, const char* pDestPath ) const
{
	// Load a source file
	CSMDLSourceModelDoc smdlSourceFile;
	if ( !smdlSourceFile.LoadFromFile( pSrcPath ) )
	{
		Error( "ResourceCompilerSMdl: Failed to load SMDL file '%s'", pSrcPath );
		return false;
	}

	// Get path to directory with the SMDL file
	eastl::string sourceFileDir;
	{
		eastl::string tmpBuffer;
		S_GetFilePath( pSrcPath, tmpBuffer, false );
		S_MakeAbsolutePath( tmpBuffer, sourceFileDir, "", false );
	}

	// Parse model data from a file
	eastl::vector<smdlMeshData_t> parsedMeshes;
	eastl::vector<eastl::string>  parsedMaterials;
	if ( !ParseModels( smdlSourceFile, sourceFileDir.c_str(), parsedMeshes, parsedMaterials ) )
	{
		return false;
	}

	// Optimize models
	Msg( "ResourceCompilerSMdl: Optimize models.." );
	for ( uint32 modelIdx = 0, numModels = (uint32)parsedMeshes.size(); modelIdx < numModels; ++modelIdx )
	{
		smdlMeshData_t& smdlMeshData = parsedMeshes[modelIdx];
		OptimizeModel( smdlMeshData );
		Msg( "ResourceCompilerSMdl: Model '%s' optimized", smdlMeshData.name.c_str() );
	}
	Msg( "ResourceCompilerSMdl: Models optimized.." );

	// Combine sub-models to one
	eastl::vector<CSMDLCompiledModelDoc> smdlCompiledFiles;
	if ( smdlSourceFile.IsCombineModels() && parsedMeshes.size() > 1 )
	{
		// Separate meshes by material ID
		Msg( "ResourceCompilerSMdl: Combining models to one.." );
		eastl::unordered_map<uint32, eastl::vector<uint32>> parsedMeshesDict;  // Key - material ID, Item - index to a mesh data in the meshes
		for ( uint32 meshIdx = 0, numMeshes = (uint32)parsedMeshes.size(); meshIdx < numMeshes; ++meshIdx )
		{
			smdlMeshData_t& smdlMeshData = parsedMeshes[meshIdx];
			parsedMeshesDict[smdlMeshData.materialID].emplace_back( meshIdx );
		}

		// Combine meshes
		eastl::vector<smdlVertex_t>	 smdlVertices;
		eastl::vector<uint32>		 indices;
		eastl::vector<smdlSurface_t> smdlSurfaces;
		for ( auto itMaterial = parsedMeshesDict.begin(), itMaterialEnd = parsedMeshesDict.end(); itMaterial != itMaterialEnd; ++itMaterial )
		{
			smdlSurface_t smdlSurface = {};
			smdlSurface.firstIndex	  = (uint32)indices.size();
			smdlSurface.materialID	  = itMaterial->first;

			for ( uint32 meshIdx = 0, numMeshes = (uint32)itMaterial->second.size(); meshIdx < numMeshes; ++meshIdx )
			{
				// Copy new vertices
				const smdlMeshData_t& smdlMeshData	 = parsedMeshes[itMaterial->second[meshIdx]];
				uint32				  offsetVertices = (uint32)smdlVertices.size();
				smdlVertices.resize( smdlVertices.size() + smdlMeshData.vertices.size() );
				Mem_Memcpy( smdlVertices.data() + offsetVertices, smdlMeshData.vertices.data(), smdlMeshData.vertices.size() * sizeof( smdlVertex_t ) );

				// Copy new indices
				uint32 offsetIndices = (uint32)indices.size();
				indices.resize( indices.size() + smdlMeshData.indices.size() );
				for ( uint32 indexIdx = 0, numIndices = (uint32)smdlMeshData.indices.size(); indexIdx < numIndices; ++indexIdx )
				{
					indices[offsetIndices + indexIdx] = offsetVertices + smdlMeshData.indices[indexIdx];
				}
			}

			smdlSurface.numIndices = (uint32)indices.size() - smdlSurface.firstIndex;
			smdlSurfaces.emplace_back( smdlSurface );
		}

		CSMDLCompiledModelDoc& smdlCompiledFile = smdlCompiledFiles.emplace_back();
		smdlCompiledFile.SetData( smdlVertices, indices, smdlSurfaces, parsedMaterials );
		Msg( "ResourceCompilerSMdl: ..Combine is done" );
	}
	// Otherwise compile separated meshes
	else
	{
		for ( uint32 modelIdx = 0, numModels = (uint32)parsedMeshes.size(); modelIdx < numModels; ++modelIdx )
		{
			CSMDLCompiledModelDoc&		 smdlCompiledFile = smdlCompiledFiles.emplace_back();
			smdlMeshData_t&				 smdlMeshData	  = parsedMeshes[modelIdx];
			eastl::vector<smdlSurface_t> smdlSurfaces;
			eastl::vector<eastl::string> materials;

			smdlSurfaces.emplace_back( smdlMeshData.surface );
			materials.emplace_back( parsedMaterials[smdlMeshData.materialID] );
			smdlCompiledFile.SetData( smdlMeshData.vertices, smdlMeshData.indices, smdlSurfaces, materials );
		}
	}

	// Save models to SMDL format
	Msg( "ResourceCompilerSMdl: Saving models.." );
	uint32 numErrors = 0;
	for ( uint32 modelIdx = 0, numModels = (uint32)smdlCompiledFiles.size(); modelIdx < numModels; ++modelIdx )
	{
		CSMDLCompiledModelDoc& smdlCompiledFile = smdlCompiledFiles[modelIdx];
		const smdlMeshData_t&  smdlMeshData		= parsedMeshes[modelIdx];
		eastl::string		   destFilePath		= S_Sprintf( "%s%s.smdl_c", pDestPath, smdlCompiledFiles.size() > 1 ? S_Sprintf( "_%i", modelIdx ).c_str() : "" );
		if ( !smdlCompiledFile.SaveFile( destFilePath.c_str() ) )
		{
			Error( "ResourceCompilerSMdl: Failed to save model '%s' to '%s'", smdlMeshData.name.c_str(), destFilePath.c_str() );
			++numErrors;
		}
		else
		{
			Msg( "ResourceCompilerSMdl: Model '%s' saved to '%s'", smdlMeshData.name.c_str(), destFilePath.c_str() );
		}
	}

	if ( numErrors == 0 )
	{
		Msg( "ResourceCompilerSMdl: ..All models saved" );
	}
	else
	{
		Warning( "ResourceCompilerSMdl: ..Some models were not saved, errors number: %i", numErrors );
	}

	// We are done
	return true;
}

/*
==================
CResourceCompilerSMdl::GetNumSrcFormats
==================
*/
uint32 CResourceCompilerSMdl::GetNumSrcFormats() const
{
	return 1;
}

/*
==================
CResourceCompilerSMdl::GetNumSrcFormats
==================
*/
const char* CResourceCompilerSMdl::GetSrcFormat( uint32 index ) const
{
	Assert( index < GetNumSrcFormats() );
	return "smdl";
}

/*
==================
CResourceCompilerSMdl::GetName
==================
*/
const char* CResourceCompilerSMdl::GetName() const
{
	return "Singularity Model";
}

/*
==================
CResourceCompilerSMdl::ProcessNode
==================
*/
void CResourceCompilerSMdl::ProcessNode( aiNode* pNode, const aiScene* pScene, smdlAiMeshesMap_t& meshesDict ) const
{
	for ( uint32 meshIdx = 0; meshIdx < pNode->mNumMeshes; ++meshIdx )
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[meshIdx]];
		meshesDict[pMesh->mMaterialIndex].emplace_back( pNode->mTransformation, pMesh );
	}

	for ( uint32 childrenIdx = 0; childrenIdx < pNode->mNumChildren; ++childrenIdx )
	{
		ProcessNode( pNode->mChildren[childrenIdx], pScene, meshesDict );
	}
}

/*
==================
CResourceCompilerSMdl::ParseModels
==================
*/
bool CResourceCompilerSMdl::ParseModels( const CSMDLSourceModelDoc& smdlSourceFile, const char* pSrcFileDir, eastl::vector<smdlMeshData_t>& parsedMeshes, eastl::vector<eastl::string>& parsedMaterials ) const
{
	// Get an absolute source path to a model
	eastl::string srcPath;
	S_MakeAbsolutePath( smdlSourceFile.GetSourcePath(), srcPath, pSrcFileDir );

	// Loading a model
	Msg( "ResourceCompilerSMdl: Loading models from '%s'..", srcPath.c_str() );
	Assimp::Importer aiImport;
	const aiScene*	 pAIScene = aiImport.ReadFile( srcPath.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices );
	if ( !pAIScene )
	{
		Error( "ResourceCompilerSMdl: Failed to read file '%s', %s", srcPath.c_str(), aiImport.GetErrorString() );
		return false;
	}

	// Fill meshes array from the Assimp scene
	smdlAiMeshesMap_t aiMeshes;
	ProcessNode( pAIScene->mRootNode, pAIScene, aiMeshes );
	if ( aiMeshes.empty() )
	{
		Error( "ResourceCompilerSMdl: In file '%s' not found models", srcPath.c_str() );
		aiImport.FreeScene();
		return false;
	}

	// Parse models in the file
	for ( auto itMaterial = aiMeshes.begin(), itMaterialEnd = aiMeshes.end(); itMaterial != itMaterialEnd; ++itMaterial )
	{
		for ( auto itMesh = itMaterial->second.begin(), itMeshEnd = itMaterial->second.end(); itMesh != itMeshEnd; ++itMesh )
		{
			aiMesh*		   pAiMesh			= ( *itMesh ).pMesh;
			smdlMeshData_t smdlMeshData		= {};
			smdlMeshData.name				= pAiMesh->mName.C_Str();
			smdlMeshData.materialID			= itMaterial->first;
			smdlMeshData.surface.materialID = itMaterial->first;
			smdlMeshData.surface.firstIndex = (uint32)smdlMeshData.indices.size();

			// Prepare the vertex buffer
			smdlMeshData.vertices.resize( pAiMesh->mNumVertices );

			// Read all vertices
			axisUp_t axisUp = smdlSourceFile.GetAxisUp();
			for ( uint32 vertexIdx = 0; vertexIdx < pAiMesh->mNumVertices; ++vertexIdx )
			{
				smdlVertex_t smdlVertex = {};
				aiVector3D	 aiVertex	= ( *itMesh ).transformation * pAiMesh->mVertices[vertexIdx];
				SMdl_ChangeAxisUp( aiVertex, axisUp );

				smdlVertex.position.x = aiVertex.x;
				smdlVertex.position.y = aiVertex.y;
				smdlVertex.position.z = aiVertex.z;
				smdlVertex.position.w = 1.f;

				aiVertex = ( *itMesh ).transformation * pAiMesh->mNormals[vertexIdx];
				SMdl_ChangeAxisUp( aiVertex, axisUp );

				smdlVertex.normal.x = aiVertex.x;
				smdlVertex.normal.y = aiVertex.y;
				smdlVertex.normal.z = aiVertex.z;
				smdlVertex.normal.w = 0.f;

				if ( pAiMesh->mTangents )
				{
					aiVertex = ( *itMesh ).transformation * pAiMesh->mTangents[vertexIdx];
					SMdl_ChangeAxisUp( aiVertex, axisUp );

					smdlVertex.tangent.x = aiVertex.x;
					smdlVertex.tangent.y = aiVertex.y;
					smdlVertex.tangent.z = aiVertex.z;
					smdlVertex.tangent.w = 0.f;
				}

				if ( pAiMesh->mBitangents )
				{
					aiVertex = ( *itMesh ).transformation * pAiMesh->mBitangents[vertexIdx];
					SMdl_ChangeAxisUp( aiVertex, axisUp );

					smdlVertex.binormal.x = aiVertex.x;
					smdlVertex.binormal.y = aiVertex.y;
					smdlVertex.binormal.z = aiVertex.z;
					smdlVertex.binormal.w = 0.f;
				}

				if ( pAiMesh->mTextureCoords[0] )
				{
					aiVertex			  = ( *itMesh ).pMesh->mTextureCoords[0][vertexIdx];
					smdlVertex.texCoord.x = aiVertex.x;
					smdlVertex.texCoord.y = aiVertex.y;
				}

				smdlMeshData.vertices[vertexIdx] = smdlVertex;
			}

			// Read all indices
			for ( uint32 faceIdx = 0; faceIdx < pAiMesh->mNumFaces; ++faceIdx )
			{
				aiFace* pAiFace = &pAiMesh->mFaces[faceIdx];
				uint32	offset	= (uint32)smdlMeshData.indices.size();
				smdlMeshData.indices.resize( offset + pAiFace->mNumIndices );
				Mem_Memcpy( smdlMeshData.indices.data() + offset, pAiFace->mIndices, pAiFace->mNumIndices * sizeof( uint32 ) );
			}

			smdlMeshData.surface.numIndices = (uint32)smdlMeshData.indices.size();
			parsedMeshes.emplace_back( smdlMeshData );
		}
	}

	// Parse materials
	const eastl::unordered_map<eastl::string, eastl::string>& renamedMaterialsDict = smdlSourceFile.GetRenamedMaterials();
	const eastl::string&									  materialsDir		   = smdlSourceFile.GetMaterialsDir();
	parsedMaterials.resize( pAIScene->mNumMaterials );
	for ( uint32 materialIdx = 0; materialIdx < pAIScene->mNumMaterials; ++materialIdx )
	{
		aiMaterial* pMaterial = pAIScene->mMaterials[materialIdx];
		aiString	aiMaterialName;
		pMaterial->Get( AI_MATKEY_NAME, aiMaterialName );

		// Rename material if it need
		eastl::string materialName		= aiMaterialName.C_Str();
		auto		  itRenamedMaterial = renamedMaterialsDict.find( materialName );
		if ( itRenamedMaterial != renamedMaterialsDict.end() )
		{
			materialName = itRenamedMaterial->second;
		}

		parsedMaterials[materialIdx] = !materialsDir.empty() ? S_Sprintf( "%s/%s", materialsDir.c_str(), materialName.c_str() ) : materialName;
	}

	aiImport.FreeScene();
	Msg( "ResourceCompilerSMdl: ..Loaded %i models and %i materials", parsedMeshes.size(), parsedMaterials.size() );
	return true;
}

/*
==================
CResourceCompilerSMdl::OptimizeModel
==================
*/
void CResourceCompilerSMdl::OptimizeModel( smdlMeshData_t& meshData ) const
{
	uint32 numIndices  = (uint32)meshData.indices.size();
	uint32 numVertices = (uint32)meshData.vertices.size();

	// Create a remap table
	eastl::vector<uint32> meshoptVertexRemap( numIndices );
	uint32				  optimizedNumVertices = (uint32)meshopt_generateVertexRemap( meshoptVertexRemap.data(),
																					  meshData.indices.data(), numIndices,
																					  meshData.vertices.data(), numVertices, sizeof( smdlVertex_t ) );

	// Allocate memory for optimized vertices and indices
	eastl::vector<uint32>		optimizedIndices( numIndices );
	eastl::vector<smdlVertex_t> optimizedVertices( optimizedNumVertices );

	// Remove duplicate vertices
	meshopt_remapIndexBuffer( optimizedIndices.data(), meshData.indices.data(), numIndices, meshoptVertexRemap.data() );
	meshopt_remapVertexBuffer( optimizedVertices.data(), meshData.vertices.data(), numVertices, sizeof( smdlVertex_t ), meshoptVertexRemap.data() );

	// Improve the locality of the vertices
	meshopt_optimizeVertexCache( optimizedIndices.data(), optimizedIndices.data(), numIndices, optimizedNumVertices );

	// Reduce pixel overdraw
	meshopt_optimizeOverdraw( optimizedIndices.data(), optimizedIndices.data(), numIndices,
							  &optimizedVertices[0].position.x, optimizedNumVertices, sizeof( smdlVertex_t ), 1.05f );

	// Optimize access to the vertex buffer
	meshopt_optimizeVertexFetch( optimizedVertices.data(), optimizedIndices.data(), numIndices, optimizedVertices.data(), optimizedNumVertices, sizeof( smdlVertex_t ) );

	// Save the optimized vertices and indices
	meshData.vertices = eastl::move( optimizedVertices );
	meshData.indices  = eastl::move( optimizedIndices );
}
