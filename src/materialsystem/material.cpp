#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "resourcesystem/iresourcesystem.h"
#include "studiorender/studioapi/istudioapi_barrier.h"
#include "materialsystem/ishader.h"
#include "materialsystem/texture.h"
#include "materialsystem/materialvar.h"
#include "materialsystem/material.h"

/*
==================
CMaterial::CMaterial
==================
*/
CMaterial::CMaterial()
	: bDirtyBuffers( false )
	, pShader( NULL )
{
}

/*
==================
CMaterial::CMaterial
==================
*/
CMaterial::CMaterial( const CSMATCompiledMaterialDoc& smatCompiledDoc )
	: bDirtyBuffers( false )
	, pShader( NULL )
{
	// Initialize the material by SMAT compiled document
	Init( smatCompiledDoc );
}

/*
==================
CMaterial::~CMaterial
==================
*/
CMaterial::~CMaterial()
{
	// Destroy buffers
	DestroyBuffers();

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
void CMaterial::Init( const CSMATCompiledMaterialDoc& smatCompiledDoc )
{
	// Set a shader from the SMAT compiled file
	SetShader( smatCompiledDoc.GetShaderName() );

	// Set material variable from the SMAT file
	IResourceTypeMgr*					   pTexturesMgr	 = g_pResourceSystem->GetResourceManagerForType<CTexture>();
	IResourceTypeMgr*					   pMaterialsMgr = g_pResourceSystem->GetResourceManagerForType<CMaterial>();
	const eastl::vector<CSMATMaterialVar>& smatMatVars	 = smatCompiledDoc.GetVars();
	for ( uint32 varIdx = 0, count = (uint32)smatMatVars.size(); varIdx < count; ++varIdx )
	{
		const CSMATMaterialVar& smatMatVar = smatMatVars[varIdx];
		CMaterialVar*			pVar	   = (CMaterialVar*)FindVar( smatMatVar.GetName() );
		if ( pVar )
		{
			switch ( smatMatVar.GetType() )
			{
			case SMAT_MATERIAL_VAR_TYPE_BOOL: pVar->SetBoolValue( smatMatVar.GetBoolValue() ); break;
			case SMAT_MATERIAL_VAR_TYPE_INT: pVar->SetIntValue( smatMatVar.GetIntValue() ); break;
			case SMAT_MATERIAL_VAR_TYPE_FLOAT: pVar->SetFloatValue( smatMatVar.GetFloatValue() ); break;
			case SMAT_MATERIAL_VAR_TYPE_MATRIX: pVar->SetMatrixValue( smatMatVar.GetMatrixValue() ); break;
			case SMAT_MATERIAL_VAR_TYPE_STRING: pVar->SetStringValue( smatMatVar.GetStringValue() ); break;
			case SMAT_MATERIAL_VAR_TYPE_TEXTURE: pVar->SetTextureValue( pTexturesMgr->LoadResource( smatMatVar.GetTextureValue() ) ); break;
			case SMAT_MATERIAL_VAR_TYPE_MATERIAL: pVar->SetMaterialValue( pMaterialsMgr->LoadResource( smatMatVar.GetMaterialValue() ) ); break;
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
			{
				vector2_t value = { 0.f, 0.f };
				smatMatVar.GetVecValue( &value.x, 2 );
				pVar->SetVecValue( &value.x, 2 );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
			{
				vector3_t value = { 0.f, 0.f, 0.f };
				smatMatVar.GetVecValue( &value.x, 3 );
				pVar->SetVecValue( &value.x, 3 );
				break;
			}
			case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
			{
				vector4_t value = { 0.f, 0.f, 0.f, 0.f };
				smatMatVar.GetVecValue( &value.x, 4 );
				pVar->SetVecValue( &value.x, 4 );
				break;
			}

			default:
				Warning( "MaterialSystem: Unknown variable type 0x%X (shader '%s', variable '%s')", smatMatVar.GetType(), smatCompiledDoc.GetShaderName(), smatMatVar.GetName() );
				break;
			}
		}
		else
		{
			Warning( "MaterialSystem: Invalid variable '%s', it isn't defined in shader '%s'", smatMatVar.GetName(), pShader->GetName() );
		}
	}
}

/*
==================
CMaterial::Clear
==================
*/
void CMaterial::Clear()
{
	// Destroy buffers
	DestroyBuffers();

	// Free allocated memory for variables
	for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
	{
		delete vars[varIdx];
	}

	// Clear some fields
	vars.clear();
	varsDict.clear();
	bDirtyBuffers = false;
	pShader		  = NULL;
}

/*
==================
CMaterial::R_UpdateBuffers
==================
*/
void CMaterial::R_UpdateBuffers( IStudioAPICmdContext* pCmdContext )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pShader );

	// Do nothing if buffers aren't dirty
	if ( !bDirtyBuffers )
	{
		return;
	}

	// Get a new buffer count
	uint32 numBuffers = pShader->GetNumBuffers();
	bDirtyBuffers	  = false;

	// Destroy buffers if the shader doesn't use they
	if ( numBuffers == 0 )
	{
		studioAPIBuffers.clear();
		return;
	}
	studioAPIBuffers.resize( numBuffers );

	// Update all buffers
	pShader->R_UpdateBuffers( pCmdContext, studioAPIBuffers.data(), (IMaterialVar**)vars.data() );
}

/*
==================
CMaterial::R_Barrier
==================
*/
void CMaterial::R_Barrier( IStudioAPICmdList* pStudioAPICmdList )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pShader );

	// Update buffers
	R_UpdateBuffers( pStudioAPICmdList->GetCmdContext() );

	// Place barriers into the list
	pShader->R_Barrier( pStudioAPICmdList, (IMaterialVar**)vars.data(), (IStudioAPIBuffer**)studioAPIBuffers.data() );
}

/*
==================
CMaterial::R_PrepareForDraw
==================
*/
void CMaterial::R_PrepareForDraw( IStudioAPICmdList* pStudioAPICmdList, studioRenderPassType_t renderPassType )
{
	PROFILER_SCOPE_FUNC_GROUP( PROFILER_SCOPE_GROUP_RENDERING );
	Assert( pShader );

	// Update buffers
	R_UpdateBuffers( pStudioAPICmdList->GetCmdContext() );

	// Prepare the shader for draw
	pShader->R_PrepareForDraw( pStudioAPICmdList, renderPassType, (IMaterialVar**)vars.data(), (IStudioAPIBuffer**)studioAPIBuffers.data() );
}

/*
==================
CMaterial::SetShader
==================
*/
void CMaterial::SetShader( const char* pShaderName )
{
	PROFILER_SCOPE_FUNC();

	// Clear the material
	Clear();

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

		// Create undefined vars for all the actual material vars
		uint32 numParams = pShader->GetNumParams();
		vars.resize( numParams );
		varsDict.clear();
		for ( uint32 paramIdx = 0; paramIdx < numParams; ++paramIdx )
		{
			shaderParam_t shaderParam	= pShader->GetParam( paramIdx );
			vars[paramIdx]				= new CMaterialVar( this, shaderParam.pName );
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

	// Mark buffers as dirty
	bDirtyBuffers = true;
}

/*
==================
CMaterial::FindVar
==================
*/
IMaterialVar* CMaterial::FindVar( const char* pName ) const
{
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
