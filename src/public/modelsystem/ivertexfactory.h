#pragma once
#include "tier1/refcount.h"
#include "tier1/debugname.h"
#include "modelsystem/modeltypes.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IStudioAPIVertexDeclaration;
class IStudioAPICmdList;
class IStudioAPIBuffer;

//-----------------------------------------------------------------------------
// Information about a vertex/index buffer in a vertex factory
//-----------------------------------------------------------------------------
struct vertexFactoryStream_t
{
	IStudioAPIBuffer* pStudioAPIBuffer;
	uint64			  offset;
};

//-----------------------------------------------------------------------------
// Information about a vertex/index buffer that lives in user memory (UP = User Pointer)
//-----------------------------------------------------------------------------
struct vertexFactoryStreamUP_t
{
	const byte* pData;
	uint32		numElements;
	uint32		stride;
};

//-----------------------------------------------------------------------------
// Vertex factory interface
//-----------------------------------------------------------------------------
class IVertexFactory : public IRefCounted, public IDebugNamed
{
public:
	virtual ~IVertexFactory() {}

	// Initialize and shutdown the factory
	virtual void Init()		= 0;
	virtual void Shutdown() = 0;

	// Configure which vertex/index buffers the factory binds
	virtual void ClearStreams()												  = 0;
	virtual void AddVertexStream( const vertexFactoryStream_t& vertexStream ) = 0;
	virtual void SetIndexStream( const vertexFactoryStream_t& indexStream )	  = 0;

	// Place barriers into a command list
	virtual void R_Barrier( IStudioAPICmdList* pStudioAPICmdList ) const = 0;

	// Create an instance buffer for drawing
	virtual CRefPtr<IStudioAPIBuffer> R_CreateInstanceBuffer( const byte* pInstances, uint32 numInstances ) const = 0;

	// Bind the vertex factory's own resources (vertex/index buffers, etc) into the command list
	virtual void R_Bind( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStream_t& instanceStream )																																 = 0;
	virtual void R_BindUP( IStudioAPICmdList* pStudioAPICmdList, const vertexFactoryStreamUP_t& instanceStream, const vertexFactoryStreamUP_t* pVertexStreams, uint32 numVertexStreams, const vertexFactoryStreamUP_t* pIndexStream = NULL ) = 0;

	// Functions to cast the interface to specific factory class
	template<class TVertexFactoryClass>
	bool IsA() const;
	template<class TVertexFactoryClass>
	TVertexFactoryClass* As() const;

	virtual bool						 IsIndexed() const					   = 0;
	virtual bool						 IsInstanced() const				   = 0;
	virtual modelVertexType_t			 GetVertexType() const				   = 0;
	virtual IStudioAPIVertexDeclaration* GetStudioAPIVertexDeclaration() const = 0;
	virtual uint32						 GetInstanceStride() const			   = 0;
};

//-----------------------------------------------------------------------------
// Helpers used to define vertex factory types
//-----------------------------------------------------------------------------
template<class TVertexFactoryClass>
struct vertexFactoryInfo_t
{
	static_assert( false, "Used unknown vertex factory class" );
	using class_t	 = TVertexFactoryClass;
	using instance_t = void;
	enum
	{
		VERTEX_TYPE = MODEL_VERTEX_NUM_TYPES
	};
};

template<class TModelInstanceClass>
struct modelInstanceInfo_t
{
	static_assert( false, "Used unknown model instance class" );
	using class_t = TModelInstanceClass;
	enum
	{
		VERTEX_TYPE = MODEL_VERTEX_NUM_TYPES
	};
};

#define DECLARE_VERTEX_FACTORY_INTERFACE( InterfaceName, InstanceType, VertexType ) \
	template<>                                                                      \
	struct vertexFactoryInfo_t<InterfaceName>                                       \
	{                                                                               \
		using class_t	 = InterfaceName;                                           \
		using instance_t = InstanceType;                                            \
		enum                                                                        \
		{                                                                           \
			VERTEX_TYPE = VertexType                                                \
		};                                                                          \
	};                                                                              \
	template<>                                                                      \
	struct modelInstanceInfo_t<InstanceType>                                        \
	{                                                                               \
		using class_t = InstanceType;                                               \
		enum                                                                        \
		{                                                                           \
			VERTEX_TYPE = VertexType                                                \
		};                                                                          \
	};

#define DECLARE_VERTEX_FACTORY( ClassName, InterfaceName )                            \
	template<>                                                                        \
	struct vertexFactoryInfo_t<ClassName> : public vertexFactoryInfo_t<InterfaceName> \
	{                                                                                 \
		using class_t = ClassName;                                                    \
	};

#include "modelsystem/ivertexfactory.inl"
