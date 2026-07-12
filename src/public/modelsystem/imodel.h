#pragma once
#include "resourcesystem/iresource.h"
#include "resourcesystem/resourceptr.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration;
class IStudioAPIBuffer;
class IMaterialResource;
class IMaterial;
class IVertexFactory;

//-----------------------------------------------------------------------------
// Initial model data
//-----------------------------------------------------------------------------
struct modelInitialData_t
{
	modelVertexType_t			   vertexType;
	modelIndexType_t			   indexType;
	uint32						   sizeVertices;
	uint32						   sizeIndices;
	uint32						   numMaterials;
	uint32						   numSurfaces;
	const byte*					   pVertices;
	const byte*					   pIndices;
	const CResourcePtr<IMaterial>* pMaterials;
	const modelSurface_t*		   pSurfaces;
};

//-----------------------------------------------------------------------------
// A model resource interface which is owned by the render thread
//-----------------------------------------------------------------------------
class IModelResource : public IRefCounted
{
public:
	virtual ~IModelResource() {}

	virtual uint32							  GetNumMaterials() const  = 0;
	virtual const CRefPtr<IMaterialResource>* GetMaterials() const	   = 0;
	virtual uint32							  GetNumSurfaces() const   = 0;
	virtual const modelSurface_t*			  GetSurfaces() const	   = 0;
	virtual IVertexFactory*					  GetVertexFactory() const = 0;
};

//-----------------------------------------------------------------------------
// A model interface
//-----------------------------------------------------------------------------
class IModel : public IResourceData
{
public:
	virtual ~IModel() {}

	virtual void Init( const modelInitialData_t& initialData ) = 0;
	virtual void Destroy()									   = 0;

	virtual uint32						   GetNumMaterials() const	 = 0;
	virtual const CResourcePtr<IMaterial>* GetMaterials() const		 = 0;
	virtual IModelResource*				   GetStudioResource() const = 0;
};

DECLARE_RESOURCE_TYPE( IModel, RESOURCE_TYPE_MODEL );
