#include "pch_model_tool.h"
#include "utils/smdldoc/smdl_compiled_doc.h"
#include "tools/resource_tools/imodel_tool.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <meshoptimizer.h>

//-----------------------------------------------------------------------------
// Assimp mesh container
//-----------------------------------------------------------------------------
struct modelToolAiMesh_t
{
	modelToolAiMesh_t()
		: pMesh( NULL )
	{
	}
	modelToolAiMesh_t( const aiMatrix4x4& transformation, aiMesh* pMesh )
		: transformation( transformation )
		, pMesh( pMesh )
	{
	}

	aiMatrix4x4 transformation;
	aiMesh*		pMesh;
};

// Assimp meshes dictionary type
// Material Index -> Ai meshes
typedef eastl::unordered_map<uint32, eastl::vector<modelToolAiMesh_t>> modelToolAiMeshesMap_t;

struct modelToolMeshData_t
{
	eastl::string				name;
	eastl::vector<smdlVertex_t> vertices;
	eastl::vector<uint32>		indices;
	smdlSurface_t				surface;
	uint32						materialID;
};

/*
==================
ModelTool_ChangeAxisUp
==================
*/
static void ModelTool_ChangeAxisUp( aiVector3D& vector, axisUp_t axisUp )
{
	switch ( axisUp )
	{
	case AXIS_UP_MINUS_X:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_X:
		Swap( vector.y, vector.x );
		break;

	case AXIS_UP_MINUS_Y:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_Y:
		break;

	case AXIS_UP_MINUS_Z:
		vector.y = -vector.y;

	case AXIS_UP_PLUS_Z:
		Swap( vector.y, vector.z );
		break;
	}
}

//-----------------------------------------------------------------------------
// Model tool
//-----------------------------------------------------------------------------
class CModelTool : public CBaseAppSystem<IModelTool>
{
public:
	// IAppSystem interface
	virtual bool Connect( createInterfaceFn_t pFactory ) override;
	virtual void Disconnect() override;

	// IModelTool interface
	virtual bool CompileModel( const resourceToolCompileModelParams_t& compileParams ) const override;

private:
	// Fill array meshes from the Assimp root scene
	void ProcessNode( aiNode* pNode, const aiScene* pScene, modelToolAiMeshesMap_t& meshesDict ) const;
	bool ParseModels( const resourceToolCompileModelParams_t& compileParams, eastl::vector<modelToolMeshData_t>& parsedMeshes, eastl::vector<eastl::string>& parsedMaterials ) const;
	void OptimizeModel( modelToolMeshData_t& meshData ) const;
};

EXPOSE_SINGLE_INTERFACE( CModelTool, IModelTool, MODEL_TOOL_INTERFACE_VERSION );

/*
==================
CModelTool::Connect
==================
*/
bool CModelTool::Connect( createInterfaceFn_t pFactory )
{
	return ConnectTier1( pFactory );
}

/*
==================
CModelTool::Disconnect
==================
*/
void CModelTool::Disconnect()
{
	DisconnectTier1();
}

/*
==================
CModelTool::CompileModel
==================
*/
bool CModelTool::CompileModel( const resourceToolCompileModelParams_t& compileParams ) const
{
	// Parse model data from a file
	eastl::vector<modelToolMeshData_t> parsedMeshes;
	eastl::vector<eastl::string>	   parsedMaterials;

	Msg( "ModelTool: Loading models from '%s'..", compileParams.pSrcPath );
	if ( !ParseModels( compileParams, parsedMeshes, parsedMaterials ) )
	{
		return false;
	}
	Msg( "ModelTool: ..Loaded %i models and %i materials", parsedMeshes.size(), parsedMaterials.size() );

	// Optimize models
	Msg( "ModelTool: Optimize models.." );
	for ( uint32 modelIdx = 0, numModels = (uint32)parsedMeshes.size(); modelIdx < numModels; ++modelIdx )
	{
		modelToolMeshData_t& modelToolMeshData = parsedMeshes[modelIdx];
		OptimizeModel( modelToolMeshData );
		Msg( "ModelTool: Model '%s' optimized", modelToolMeshData.name.c_str() );
	}
	Msg( "ModelTool: Models optimized.." );

	// Combine sub-models to one
	eastl::vector<CSMDLCompiledModelDoc> smdlCompiledFiles;
	if ( compileParams.bCombineModels && parsedMeshes.size() > 1 )
	{
		// Separate meshes by material ID
		Msg( "ModelTool: Combining models to one.." );
		eastl::unordered_map<uint32, eastl::vector<uint32>> parsedMeshesDict;  // Key - material ID, Item - index to a mesh data in the meshes
		for ( uint32 meshIdx = 0, numMeshes = (uint32)parsedMeshes.size(); meshIdx < numMeshes; ++meshIdx )
		{
			const modelToolMeshData_t& modelToolMeshData = parsedMeshes[meshIdx];
			parsedMeshesDict[modelToolMeshData.materialID].emplace_back( meshIdx );
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
				const modelToolMeshData_t& modelToolMeshData = parsedMeshes[itMaterial->second[meshIdx]];
				uint32					   offsetVertices	 = (uint32)smdlVertices.size();
				smdlVertices.resize( smdlVertices.size() + modelToolMeshData.vertices.size() );
				Mem_Memcpy( smdlVertices.data() + offsetVertices, modelToolMeshData.vertices.data(), modelToolMeshData.vertices.size() * sizeof( smdlVertex_t ) );

				// Copy new indices
				uint32 offsetIndices = (uint32)indices.size();
				indices.resize( indices.size() + modelToolMeshData.indices.size() );
				for ( uint32 indexIdx = 0, numIndices = (uint32)modelToolMeshData.indices.size(); indexIdx < numIndices; ++indexIdx )
				{
					indices[offsetIndices + indexIdx] = offsetVertices + modelToolMeshData.indices[indexIdx];
				}
			}

			smdlSurface.numIndices = (uint32)indices.size() - smdlSurface.firstIndex;
			smdlSurfaces.emplace_back( smdlSurface );
		}

		CSMDLCompiledModelDoc& smdlCompiledFile = smdlCompiledFiles.emplace_back();
		smdlCompiledFile.SetData( smdlVertices, indices, smdlSurfaces, parsedMaterials );
		Msg( "ModelTool: ..Combine is done" );
	}
	// Otherwise compile separated meshes
	else
	{
		for ( uint32 modelIdx = 0, numModels = (uint32)parsedMeshes.size(); modelIdx < numModels; ++modelIdx )
		{
			CSMDLCompiledModelDoc&		 smdlCompiledFile  = smdlCompiledFiles.emplace_back();
			modelToolMeshData_t&		 modelToolMeshData = parsedMeshes[modelIdx];
			eastl::vector<smdlSurface_t> smdlSurfaces;
			eastl::vector<eastl::string> materials;

			smdlSurfaces.emplace_back( modelToolMeshData.surface );
			materials.emplace_back( parsedMaterials[modelToolMeshData.materialID] );
			smdlCompiledFile.SetData( modelToolMeshData.vertices, modelToolMeshData.indices, smdlSurfaces, materials );
		}
	}

	// Save models to SMDL format
	Msg( "ModelTool: Saving models.." );
	uint32 numErrors = 0;
	for ( uint32 modelIdx = 0, numModels = (uint32)smdlCompiledFiles.size(); modelIdx < numModels; ++modelIdx )
	{
		CSMDLCompiledModelDoc&	   smdlCompiledFile	 = smdlCompiledFiles[modelIdx];
		const modelToolMeshData_t& modelToolMeshData = parsedMeshes[modelIdx];
		eastl::string			   destFilePath		 = S_Sprintf( "%s%s.smdl_c", compileParams.pDestPath, smdlCompiledFiles.size() > 1 ? S_Sprintf( "_%i", modelIdx ).c_str() : "" );
		if ( !smdlCompiledFile.SaveFile( destFilePath.c_str() ) )
		{
			Error( "ModelTool: Failed to save model '%s' to '%s'", modelToolMeshData.name.c_str(), destFilePath.c_str() );
			++numErrors;
		}
		else
		{
			Msg( "ModelTool: Model '%s' saved to '%s'", modelToolMeshData.name.c_str(), destFilePath.c_str() );
		}
	}

	if ( numErrors == 0 )
	{
		Msg( "ModelTool: ..All models saved" );
	}
	else
	{
		Warning( "ModelTool: ..Some models were not saved, errors number: %i", numErrors );
	}

	// We are done
	return true;
}

/*
==================
CModelTool::ProcessNode
==================
*/
void CModelTool::ProcessNode( aiNode* pNode, const aiScene* pScene, modelToolAiMeshesMap_t& meshesDict ) const
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
CModelTool::ParseModels
==================
*/
bool CModelTool::ParseModels( const resourceToolCompileModelParams_t& compileParams, eastl::vector<modelToolMeshData_t>& parsedMeshes, eastl::vector<eastl::string>& parsedMaterials ) const
{
	// Loading a model
	Assimp::Importer aiImport;
	const aiScene*	 pAIScene = aiImport.ReadFile( compileParams.pSrcPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices );
	if ( !pAIScene )
	{
		Error( "ModelTool: Failed to read file '%s', %s", compileParams.pSrcPath, aiImport.GetErrorString() );
		return false;
	}

	// Fill meshes array from the Assimp scene
	modelToolAiMeshesMap_t aiMeshes;
	ProcessNode( pAIScene->mRootNode, pAIScene, aiMeshes );
	if ( aiMeshes.empty() )
	{
		Error( "ModelTool: In file '%s' not found models", compileParams.pSrcPath );
		aiImport.FreeScene();
		return false;
	}

	// Parse models in the file
	for ( auto itMaterial = aiMeshes.begin(), itMaterialEnd = aiMeshes.end(); itMaterial != itMaterialEnd; ++itMaterial )
	{
		for ( auto itMesh = itMaterial->second.begin(), itMeshEnd = itMaterial->second.end(); itMesh != itMeshEnd; ++itMesh )
		{
			aiMesh*				pAiMesh			  = ( *itMesh ).pMesh;
			modelToolMeshData_t modelToolMeshData = {};
			modelToolMeshData.name				  = pAiMesh->mName.C_Str();
			modelToolMeshData.materialID		  = itMaterial->first;
			modelToolMeshData.surface.materialID  = itMaterial->first;
			modelToolMeshData.surface.firstIndex  = (uint32)modelToolMeshData.indices.size();

			// Prepare the vertex buffer
			modelToolMeshData.vertices.resize( pAiMesh->mNumVertices );

			// Read all vertices
			for ( uint32 vertexIdx = 0; vertexIdx < pAiMesh->mNumVertices; ++vertexIdx )
			{
				smdlVertex_t smdlVertex = {};
				aiVector3D	 aiVertex	= ( *itMesh ).transformation * pAiMesh->mVertices[vertexIdx];
				ModelTool_ChangeAxisUp( aiVertex, compileParams.axisUp );

				smdlVertex.position.x = aiVertex.x;
				smdlVertex.position.y = aiVertex.y;
				smdlVertex.position.z = aiVertex.z;
				smdlVertex.position.w = 1.f;

				aiVertex = ( *itMesh ).transformation * pAiMesh->mNormals[vertexIdx];
				ModelTool_ChangeAxisUp( aiVertex, compileParams.axisUp );

				smdlVertex.normal.x = aiVertex.x;
				smdlVertex.normal.y = aiVertex.y;
				smdlVertex.normal.z = aiVertex.z;
				smdlVertex.normal.w = 0.f;

				if ( pAiMesh->mTangents )
				{
					aiVertex = ( *itMesh ).transformation * pAiMesh->mTangents[vertexIdx];
					ModelTool_ChangeAxisUp( aiVertex, compileParams.axisUp );

					smdlVertex.tangent.x = aiVertex.x;
					smdlVertex.tangent.y = aiVertex.y;
					smdlVertex.tangent.z = aiVertex.z;
					smdlVertex.tangent.w = 0.f;
				}

				if ( pAiMesh->mBitangents )
				{
					aiVertex = ( *itMesh ).transformation * pAiMesh->mBitangents[vertexIdx];
					ModelTool_ChangeAxisUp( aiVertex, compileParams.axisUp );

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

				modelToolMeshData.vertices[vertexIdx] = smdlVertex;
			}

			// Read all indices
			for ( uint32 faceIdx = 0; faceIdx < pAiMesh->mNumFaces; ++faceIdx )
			{
				aiFace* pAiFace = &pAiMesh->mFaces[faceIdx];
				uint32	offset	= (uint32)modelToolMeshData.indices.size();
				modelToolMeshData.indices.resize( offset + pAiFace->mNumIndices );
				Mem_Memcpy( modelToolMeshData.indices.data() + offset, pAiFace->mIndices, pAiFace->mNumIndices * sizeof( uint32 ) );
			}

			modelToolMeshData.surface.numIndices = (uint32)modelToolMeshData.indices.size();
			parsedMeshes.emplace_back( modelToolMeshData );
		}
	}

	// Initialize dictionary to fast find materials to rename
	eastl::unordered_map<eastl::string, eastl::string> renamedMaterialsDict;
	for ( uint32 materialIdx = 0; materialIdx < compileParams.numRenamedMaterials; ++materialIdx )
	{
		const resourceToolRenamedMaterial_t& resourceRenamedMaterial = compileParams.pRenamedMaterials[materialIdx];
		renamedMaterialsDict[resourceRenamedMaterial.pOriginalName]	 = resourceRenamedMaterial.pNewName;
	}

	// Parse materials
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

		parsedMaterials[materialIdx] = S_Strlen( compileParams.pMaterialsDir ) > 0 ? S_Sprintf( "%s/%s", compileParams.pMaterialsDir, materialName.c_str() ) : materialName;
	}

	aiImport.FreeScene();
	return true;
}

/*
==================
CModelTool::OptimizeModel
==================
*/
void CModelTool::OptimizeModel( modelToolMeshData_t& meshData ) const
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
