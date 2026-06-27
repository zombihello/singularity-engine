#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/ishader.h"
#include "materialsystem/texture.h"
#include "materialsystem/materialvar.h"
#include "materialsystem/material.h"

/*
==================
CMaterialResource::CMaterialResource
==================
*/
CMaterialResource::CMaterialResource( IShader* pShader, IShaderContextData* pContextData )
	: pShader( pShader )
	, pContextData( pContextData )
{
}

/*
==================
CMaterialResource::FinalRelease
==================
*/
void CMaterialResource::FinalRelease()
{
	if ( IsNeedDeferredDestroy() )
	{
		Studio_BeginDeleteResource( this );
	}
	else
	{
		delete this;
	}
}

/*
==================
CMaterialResource::ReleaseStudioAPI
==================
*/
void CMaterialResource::ReleaseStudioAPI()
{
	pContextData->ReleaseResource();
	pContextData = NULL;
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
CMaterialResource::GetContextData
==================
*/
IShaderContextData* CMaterialResource::GetContextData() const
{
	return pContextData;
}

/*
==================
CMaterial::CMaterial
==================
*/
CMaterial::CMaterial( IResource* pResource )
	: CResourceData<IMaterial>( pResource )
	, pShader( NULL )
{
}

/*
==================
CMaterial::CMaterial
==================
*/
CMaterial::CMaterial( IResource* pResource, const CSMATCompiledMaterialDoc& smatCompiledDoc )
	: CResourceData<IMaterial>( pResource )
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
	// Delete the studio resource
	DeleteStudioResource();

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
	// Delete the studio resource
	DeleteStudioResource();

	// Free allocated memory for variables
	for ( uint32 varIdx = 0, numVars = (uint32)vars.size(); varIdx < numVars; ++varIdx )
	{
		delete vars[varIdx];
	}

	// Clear some fields
	vars.clear();
	resourceVarIds.clear();
	varsDict.clear();
	pShader = NULL;
}

/*
==================
CMaterial::MarkUsedDependencies
==================
*/
void CMaterial::MarkUsedDependencies()
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
			pResource->MarkUsed();
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

	// Delete the studio resource
	DeleteStudioResource();
}

/*
==================
CMaterial::CreateStudioResource
==================
*/
void CMaterial::CreateStudioResource()
{
	// Do nothing if the studio resource already created
	PROFILER_SCOPE_FUNC();
	if ( pStudioResource )
	{
		return;
	}

	// Create a studio resource
	Assert( pShader );
	pStudioResource = new CMaterialResource( pShader, pShader->CreateContextData( (IMaterialVar**)vars.data() ) );
	Studio_BeginInitResource( pStudioResource );

	// Trigger event that the studio resource has been changed
	onStudioResourceChanged.Invoke( this );
}

/*
==================
CMaterial::DeleteStudioResource
==================
*/
void CMaterial::DeleteStudioResource()
{
	// Do nothing if the studio resource already deleted
	PROFILER_SCOPE_FUNC();
	if ( !pStudioResource )
	{
		return;
	}

	// Release the studio resource
	Studio_BeginReleaseResource( pStudioResource );
	pStudioResource = NULL;

	// Trigger event that the studio resource has been changed
	onStudioResourceChanged.Invoke( this );
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
		resourceVarIds.clear();
		varsDict.clear();
		for ( uint32 paramIdx = 0; paramIdx < numParams; ++paramIdx )
		{
			shaderParam_t shaderParam	= pShader->GetParam( paramIdx );
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

/*
==================
CMaterial::GetShaderContextData
==================
*/
IMaterialResource* CMaterial::GetStudioResource() const
{
	if ( !pStudioResource )
	{
		const_cast<CMaterial*>( this )->CreateStudioResource();
	}
	return pStudioResource;
}

/*
==================
CMaterial::OnStudioResourceChanged
==================
*/
IMaterial::IOnStudioResourceChanged* CMaterial::OnStudioResourceChanged() const
{
	return &onStudioResourceChanged;
}