#include "pch_materialsystem.h"
#include "tier1/filetools.h"
#include "resourcesystem/iresourcesystem.h"
#include "utils/smatdoc/smat_compiled_doc.h"
#include "materialsystem/texture.h"
#include "materialsystem/material.h"
#include "materialsystem/material_loader.h"

/*
==================
CMaterialLoader::Load
==================
*/
bool CMaterialLoader::Load( const char* pPath, IResourceData* pData ) const
{
	PROFILER_SCOPE_FUNC();
	Assert( pData );
	CSMATCompiledMaterialDoc smatCompiledMaterialDoc;
	if ( !smatCompiledMaterialDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smat_c", pPath ).c_str() ) )
	{
		return NULL;
	}

	// Convert SMAT material variables to the engine format
	IResourceTypeMgr*					   pTexturesMgr	 = g_pResourceSystem->GetResourceManagerForType<CTexture>();
	IResourceTypeMgr*					   pMaterialsMgr = g_pResourceSystem->GetResourceManagerForType<CMaterial>();
	const eastl::vector<CSMATMaterialVar>& smatMatVars	 = smatCompiledMaterialDoc.GetVars();
	eastl::vector<materialVarInfo_t>	   matVarInfos;
	eastl::vector<CRefPtr<IResource>>	   referencedResources;

	matVarInfos.reserve( smatMatVars.size() );
	referencedResources.reserve( smatMatVars.size() );
	for ( uint32 varIdx = 0, count = (uint32)smatMatVars.size(); varIdx < count; ++varIdx )
	{
		const CSMATMaterialVar& smatMatVar = smatMatVars[varIdx];
		materialVarInfo_t		matVarInfo = {};
		switch ( smatMatVar.GetType() )
		{
		case SMAT_MATERIAL_VAR_TYPE_BOOL: matVarInfo = MaterialVar_MakeBool( smatMatVar.GetName(), smatMatVar.GetBoolValue() ); break;
		case SMAT_MATERIAL_VAR_TYPE_INT: matVarInfo = MaterialVar_MakeInt( smatMatVar.GetName(), smatMatVar.GetIntValue() ); break;
		case SMAT_MATERIAL_VAR_TYPE_FLOAT: matVarInfo = MaterialVar_MakeFloat( smatMatVar.GetName(), smatMatVar.GetFloatValue() ); break;
		case SMAT_MATERIAL_VAR_TYPE_MATRIX: matVarInfo = MaterialVar_MakeMatrix( smatMatVar.GetName(), smatMatVar.GetMatrixValue() ); break;
		case SMAT_MATERIAL_VAR_TYPE_STRING: matVarInfo = MaterialVar_MakeString( smatMatVar.GetName(), smatMatVar.GetStringValue() ); break;
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_2D:
		{
			vector2_t vector2d;
			smatMatVar.GetVecValue( &vector2d.x, 2 );
			matVarInfo = MaterialVar_MakeVec( smatMatVar.GetName(), vector2d );
			break;
		}
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_3D:
		{
			vector3_t vector3d;
			smatMatVar.GetVecValue( &vector3d.x, 3 );
			matVarInfo = MaterialVar_MakeVec( smatMatVar.GetName(), vector3d );
			break;
		}
		case SMAT_MATERIAL_VAR_TYPE_VECTOR_4D:
		{
			vector4_t vector4d;
			smatMatVar.GetVecValue( &vector4d.x, 4 );
			matVarInfo = MaterialVar_MakeVec( smatMatVar.GetName(), vector4d );
			break;
		}
		case SMAT_MATERIAL_VAR_TYPE_TEXTURE:
		{
			CRefPtr<IResource> pTexture = pTexturesMgr->LoadResource( smatMatVar.GetTextureValue() );
			matVarInfo					= MaterialVar_MakeTexture( smatMatVar.GetName(), pTexture );
			referencedResources.emplace_back( pTexture );
			break;
		}
		case SMAT_MATERIAL_VAR_TYPE_MATERIAL:
		{
			CRefPtr<IResource> pMaterial = pMaterialsMgr->LoadResource( smatMatVar.GetMaterialValue() );
			matVarInfo					 = MaterialVar_MakeMaterial( smatMatVar.GetName(), pMaterial );
			referencedResources.emplace_back( pMaterial );
			break;
		}
		default:
			Warning( "MaterialSystem: Unknown SMAT variable type 0x%X (shader '%s', variable '%s')", smatMatVar.GetType(), smatCompiledMaterialDoc.GetShaderName(), smatMatVar.GetName() );
			continue;
		}

		matVarInfos.emplace_back( matVarInfo );
	}

	// Initialize the material
	// NOTE: 'referencedResources' keeps the loaded resources alive until the material takes own references to them
	CMaterial*			  pMaterial			  = (CMaterial*)pData;
	materialInitialData_t materialInitialData = {};
	materialInitialData.pShaderName			  = smatCompiledMaterialDoc.GetShaderName();
	materialInitialData.numVars				  = (uint32)matVarInfos.size();
	materialInitialData.pVars				  = matVarInfos.data();
	pMaterial->Init( materialInitialData );
	return true;
}

/*
==================
CMaterialLoader::GetFormatName
==================
*/
const char* CMaterialLoader::GetFormatName() const
{
	return "Singularity Material";
}
