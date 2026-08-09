#pragma once
#include "tier1/refcount.h"
#include "appframework/iappsystem.h"
#include "modelsystem/ivertexfactory.h"

//-----------------------------------------------------------------------------
// Model system interface
//-----------------------------------------------------------------------------
#define MODELSYSTEM_INTERFACE_VERSION "SModelSystem001"
class IModelSystem : public IAppSystem
{
public:
	template<class TVertexFactoryClass>
	CRefPtr<TVertexFactoryClass>	CreateVertexFactory() const;
	virtual CRefPtr<IVertexFactory> CreateVertexFactory( modelVertexType_t vertexType ) const = 0;
};

#include "modelsystem/imodelsystem.inl"
