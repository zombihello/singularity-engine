#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "materialsystem/ishader.h"
#include "materialsystem/materialvar.h"
#include "materialsystem/material.h"

/*
==================
CMaterialResource::CMaterialResource
==================
*/
CMaterialResource::CMaterialResource()
	: pShader( NULL )
{
}

/*
==================
CMaterialResource::Update
==================
*/
void CMaterialResource::Update( IShader* pShader, CMaterialVar** pVars )
{
	PROFILER_SCOPE_FUNC();
	Assert( pShader );
	Assert( pVars || pShader->GetNumParams( SHADER_PARAM_FREQUENCY_PERMATERIAL ) == 0 );

	// Create a new per-material context data if shader has been changed
	if ( CMaterialResource::pShader != pShader )
	{
		// Remember a new shader and create a new context data
		CMaterialResource::pShader = pShader;
		pPerMaterialContextData	   = pShader->CreatePerMaterialContextData();
	}

	// Update the per-material context data
	pPerMaterialContextData->Update( (IMaterialVar**)pVars );
}

/*
==================
CMaterialResource::Clear
==================
*/
void CMaterialResource::Clear()
{
	// Do nothing if the studio resource isn't valid
	PROFILER_SCOPE_FUNC();
	if ( !IsValid() )
	{
		return;
	}

	// Clear all fields
	pShader					= NULL;
	pPerMaterialContextData = NULL;
}

/*
==================
CMaterialResource::GetShader
==================
*/
IShader* CMaterialResource::GetShader() const
{
	return pShader;
}

/*
==================
CMaterialResource::GetPerMaterialContextData
==================
*/
IPerMaterialContextData* CMaterialResource::GetPerMaterialContextData() const
{
	return pPerMaterialContextData;
}

/*
==================
CMaterial::CMaterial
==================
*/
CMaterial::CMaterial( IResource* pResource )
	: CResourceData<IMaterial>( pResource )
	, bDirtyStudioResource( false )
	, bDirtyDependencies( false )
	, bBatchDependencies( false )
	, pShader( NULL )
	, pStudioResource( new CMaterialResource() )
{
}

/*
==================
CMaterial::~CMaterial
==================
*/
CMaterial::~CMaterial()
{
	// Clear the studio resource
	ClearStudioResource();

	// Free allocated memory for variables
	for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
	{
		delete vars[varIdx];
	}
}

/*
==================
CMaterial::Init
==================
*/
void CMaterial::Init( const materialInitialData_t& initialData )
{
	PROFILER_SCOPE_FUNC();
	{
		// Batch all the dependency changes below into a single rebuild
		CGuardValue<bool> batchDependenciesGuardValue( bBatchDependencies, true );

		// Set a shader from the initial data
		SetShader( initialData.pShaderName );

		// Set material variable from the initial data
		for ( uint32 varIdx = 0; varIdx < initialData.numVars; ++varIdx )
		{
			const materialVarInfo_t& matVarInfo = initialData.pVars[varIdx];
			CMaterialVar*			 pVar		= (CMaterialVar*)FindVar( matVarInfo.pName );
			if ( pVar )
			{
				switch ( matVarInfo.type )
				{
				case MATERIALVAR_TYPE_BOOL: pVar->SetBoolValue( matVarInfo.boolValue ); break;
				case MATERIALVAR_TYPE_INT: pVar->SetIntValue( matVarInfo.intValue ); break;
				case MATERIALVAR_TYPE_FLOAT: pVar->SetFloatValue( matVarInfo.floatValue ); break;
				case MATERIALVAR_TYPE_VECTOR_2D: pVar->SetVecValue( &matVarInfo.vector2DValue.x, 2 ); break;
				case MATERIALVAR_TYPE_VECTOR_3D: pVar->SetVecValue( &matVarInfo.vector3DValue.x, 3 ); break;
				case MATERIALVAR_TYPE_VECTOR_4D: pVar->SetVecValue( &matVarInfo.vector4DValue.x, 4 ); break;
				case MATERIALVAR_TYPE_MATRIX: pVar->SetMatrixValue( matVarInfo.matrixValue ); break;
				case MATERIALVAR_TYPE_STRING: pVar->SetStringValue( matVarInfo.pStringValue ); break;
				case MATERIALVAR_TYPE_TEXTURE: pVar->SetTextureValue( matVarInfo.pResourceValue ); break;
				case MATERIALVAR_TYPE_MATERIAL: pVar->SetMaterialValue( matVarInfo.pResourceValue ); break;
				default:
					Warning( "MaterialSystem: Unknown variable type 0x%X (shader '%s', variable '%s')", matVarInfo.type, pShader->GetName(), matVarInfo.pName );
					break;
				}
			}
			else
			{
				Warning( "MaterialSystem: Invalid variable '%s', it isn't defined in shader '%s'", matVarInfo.pName, pShader->GetName() );
			}
		}
	}

	// Rebuild the dependency list once for all the changes above
	// NOTE: The batch guard must be released before it, otherwise the rebuild will be skipped
	UpdateDependencies();
}

/*
==================
CMaterial::Destroy
==================
*/
void CMaterial::Destroy()
{
	// Clear the studio resource
	PROFILER_SCOPE_FUNC();
	ClearStudioResource();

	// Free allocated memory for variables
	for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
	{
		delete vars[varIdx];
	}

	// Clear some fields
	vars.clear();
	resourceVarIds.clear();
	varsDict.clear();
	pShader				 = NULL;
	bDirtyStudioResource = false;

	// Rebuild the dependency list, since the material doesn't reference any resource anymore
	bDirtyDependencies = true;
	UpdateDependencies();
}

/*
==================
CMaterial::CollectDependencies
==================
*/
void CMaterial::CollectDependencies( IResourceDependencyCollector* pCollector ) const
{
	PROFILER_SCOPE_FUNC();
	for ( uint32 index = 0, count = (uint32)resourceVarIds.size(); index < count; ++index )
	{
		CMaterialVar* pVar		= vars[resourceVarIds[index]];
		IResource*	  pResource = NULL;
		switch ( pVar->GetType() )
		{
		case MATERIALVAR_TYPE_TEXTURE: pResource = pVar->GetTextureValue(); break;
		case MATERIALVAR_TYPE_MATERIAL: pResource = pVar->GetMaterialValue(); break;
		default:
			AssertMsg( false, "Unknown material variable type 0x%X", pVar->GetType() );
			break;
		}

		if ( pResource )
		{
			pCollector->AddDependency( pResource );
		}
	}
}

/*
==================
CMaterial::ReportVarChanged
==================
*/
void CMaterial::ReportVarChanged( CMaterialVar* pVar, materialVarType_t oldType )
{
	// Update resource var indices if the type has been changed
	PROFILER_SCOPE_FUNC();
	bool bOldResourceVarType = CMaterialVar::IsResourceVarType( oldType );
	bool bNewResourceVarType = CMaterialVar::IsResourceVarType( pVar->GetType() );
	if ( bNewResourceVarType != bOldResourceVarType )
	{
		// Remove the var from `resourceVarIds` if the old type was a resource
		uint32 varId = pVar->GetId();
		if ( bOldResourceVarType )
		{
			for ( uint32 index = 0, count = (uint32)resourceVarIds.size(); index < count; ++index )
			{
				if ( resourceVarIds[index] == varId )
				{
					resourceVarIds.erase( resourceVarIds.begin() + index );
					break;
				}
			}
		}

		// Add the var to `resourceVarIds` if the new type is a resource
		if ( bNewResourceVarType )
		{
			resourceVarIds.emplace_back( varId );
		}
	}

	// Rebuild the dependency list if the var references a resource or has just stopped referencing one.
	// NOTE: The resource the var references may change even when its type (and therefore `resourceVarIds`) didn't
	if ( bNewResourceVarType || bOldResourceVarType )
	{
		bDirtyDependencies = true;
		UpdateDependencies();
	}

	// Insert a fence to the render thread and mark the studio resource as dirty
	pStudioResource->GetRenderCmdFence().InsertFence();
	bDirtyStudioResource = true;
}

/*
==================
CMaterial::UpdateStudioResource
==================
*/
void CMaterial::UpdateStudioResource()
{
	// Do nothing if the studio resource isn't dirty
	PROFILER_SCOPE_FUNC();
	if ( !bDirtyStudioResource )
	{
		return;
	}
	bDirtyStudioResource = false;

	// Wait fences to make sure that the render thread not using the studio resource
	pStudioResource->GetRenderCmdFence().Wait();

	// Update the studio resource
	pStudioResource->Update( pShader, vars.data() );
}

/*
==================
CMaterial::UpdateDependencies
==================
*/
void CMaterial::UpdateDependencies()
{
	// Do nothing if the dependency list isn't dirty or the changes are batched
	PROFILER_SCOPE_FUNC();
	if ( !bDirtyDependencies || bBatchDependencies )
	{
		return;
	}
	bDirtyDependencies = false;

	// Re-collect the dependency list from the current resource vars
	GetResource()->RebuildDependencies();
}

/*
==================
CMaterial::ClearStudioResource
==================
*/
void CMaterial::ClearStudioResource()
{
	// Do nothing if the studio resource already isn't valid
	PROFILER_SCOPE_FUNC();
	if ( !pStudioResource->IsValid() )
	{
		return;
	}

	// Insert and wait a fence to make sure that the render thread not using the studio resource
	CStudioRenderCmdFence& renderCmdFence = pStudioResource->GetRenderCmdFence();
	renderCmdFence.InsertFence();
	renderCmdFence.Wait();

	// Clear the studio resource
	pStudioResource->Clear();
}

/*
==================
CMaterial::SetShader
==================
*/
void CMaterial::SetShader( const char* pShaderName )
{
	// Destroy the material
	PROFILER_SCOPE_FUNC();
	Destroy();

	// Keep going until there's no more fallbacks
	const char* pCurrentShaderName = pShaderName;
	while ( true )
	{
		// Find the shader for this material. Note that this may not be
		// the actual shader we use due to fallbacks
		pShader = g_pShaderMgr->FindShader( pCurrentShaderName );
		if ( !pShader )
		{
			// Couldn't find the shader we wanted to use, it's not defined
			Warning( "MaterialSystem: Couldn't find shader '%s'", pCurrentShaderName );
			pShader = g_pShaderMgr->FindShader( "wireframe" );
			Assert( pShader );
		}

		// Free allocated memory for variables
		for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
		{
			delete vars[varIdx];
		}

		// Create undefined vars for all the actual per-material shader params
		uint32 numParams = pShader->GetNumParams( SHADER_PARAM_FREQUENCY_PERMATERIAL );
		vars.resize( numParams );
		resourceVarIds.clear();
		varsDict.clear();
		for ( uint32 paramIdx = 0; paramIdx < numParams; ++paramIdx )
		{
			shaderParam_t shaderParam	= pShader->GetParam( SHADER_PARAM_FREQUENCY_PERMATERIAL, paramIdx );
			vars[paramIdx]				= new CMaterialVar( this, shaderParam.pName, paramIdx );
			varsDict[shaderParam.pName] = paramIdx;
		}

		// Make sure we set default values before the fallback is looked for
		pShader->InitDefaultParams( (IMaterialVar**)vars.data() );

		// Check for a fallback, if isn't we're done
		const char* pFallbackShaderName = pShader->GetFallbackShader();
		if ( !pFallbackShaderName )
		{
			break;
		}
		pCurrentShaderName = pFallbackShaderName;
	}

	// Insert a fence to the render thread and mark the studio resource as dirty
	pStudioResource->GetRenderCmdFence().InsertFence();
	bDirtyStudioResource = true;

	// Rebuild the dependency list, since the resource vars have been recreated
	bDirtyDependencies = true;
	UpdateDependencies();
}

/*
==================
CMaterial::FindVar
==================
*/
IMaterialVar* CMaterial::FindVar( const char* pName ) const
{
	PROFILER_SCOPE_FUNC();
	auto it = varsDict.find( pName );
	if ( it != varsDict.end() )
	{
		return vars[it->second];
	}
	return NULL;
}

/*
==================
CMaterial::GetNumVars
==================
*/
uint32 CMaterial::GetNumVars() const
{
	return (uint32)vars.size();
}

/*
==================
CMaterial::GetVars
==================
*/
IMaterialVar** CMaterial::GetVars() const
{
	return (IMaterialVar**)vars.data();
}

/*
==================
CMaterial::GetShaderName
==================
*/
const char* CMaterial::GetShaderName() const
{
	return pShader ? pShader->GetName() : "";
}

/*
==================
CMaterial::GetShader
==================
*/
IShader* CMaterial::GetShader() const
{
	return pShader;
}

/*
==================
CMaterial::GetStudioResource
==================
*/
IMaterialResource* CMaterial::GetStudioResource() const
{
	// TODO BS yehor.pohuliaka - Remove it when will be done CIT-43
	if ( bDirtyStudioResource )
	{
		( (CMaterial*)this )->UpdateStudioResource();
	}
	return pStudioResource;
}
