#include "pch_materialsystem.h"
#include "stdlib/filetools.h"
#include "smatdoc/smat_compiled_doc.h"
#include "resourcesystem/iresourcesystem.h"
#include "materialsystem/material.h"
#include "materialsystem/material_factory.h"

/*
==================
CMaterialFactory::Init
==================
*/
void CMaterialFactory::Init()
{
	// Register the factory in the resource system
	Assert( g_pResourceSystem );
	flags = GetDefaultFlags();
	g_pResourceSystem->RegisterResourceFactory( RESOURCE_TYPE_MATERIAL, this );

	// Initialize a default material
	pDefaultMaterial = g_pResourceSystem->FindOrLoadResource( "//CORE/materials/default", RESOURCE_TYPE_MATERIAL );
	if ( !pDefaultMaterial )
	{
		pDefaultMaterial	 = g_pResourceSystem->CreateProceduralResource( "default", RESOURCE_TYPE_MATERIAL );
		CMaterial* pMaterial = (CMaterial*)pDefaultMaterial->GetData();
		pMaterial->SetShader( "wireframe" );
	}
}

/*
==================
CMaterialFactory::Shutdown
==================
*/
void CMaterialFactory::Shutdown()
{
	// Unregister the factory from the resource system
	flags |= RESOURCE_FACTORY_FLAG_NOT_USED;
	g_pResourceSystem->UnRegisterResourceFactory( RESOURCE_TYPE_MATERIAL );

	// Remove the default material
	pDefaultMaterial = NULL;
}

/*
==================
CMaterialFactory::CreateProceduralResource
==================
*/
TRefPtr<IRefCounted> CMaterialFactory::CreateProceduralResource() const
{
	return new CMaterial();
}

/*
==================
CMaterialFactory::LoadResource
==================
*/
TRefPtr<IRefCounted> CMaterialFactory::LoadResource( const char* pPath, uint32 loadFlags /* = RESOURCE_LOAD_FLAG_NONE */ ) const
{
	CSMATCompiledMaterialDoc smatCompiledMaterialDoc;
	if ( !smatCompiledMaterialDoc.LoadFromFile( S_GetFileExtension( pPath ) ? pPath : S_Sprintf( "%s.smat_c", pPath ).c_str() ) )
	{
		return NULL;
	}

	return new CMaterial( smatCompiledMaterialDoc );
}

/*
==================
CMaterialFactory::UnloadResource
==================
*/
void CMaterialFactory::UnloadResource( IRefCounted* pResoruce ) const
{
	CMaterial* pMaterial = (CMaterial*)pResoruce;
	pMaterial->Clear();
}

/*
==================
CMaterialFactory::GetDefaultResource
==================
*/
TRefPtr<IResource> CMaterialFactory::GetDefaultResource() const
{
	return pDefaultMaterial;
}

/*
==================
CMaterialFactory::GetFormatType
==================
*/
const char* CMaterialFactory::GetFormatType() const
{
	return "SMAT";
}