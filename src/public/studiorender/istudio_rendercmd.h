#ifndef ISTUDIO_RENDERCMD_H
#define ISTUDIO_RENDERCMD_H

#include "interfaces/interfaces.h"
#include "core/types.h"
#include "core/platform.h"
#include "studiorender/istudio_cmdbuffer.h"
#include "studiorender/istudiorender.h"

//-----------------------------------------------------------------------------
// Studio render command interface stored in the render command queue
//-----------------------------------------------------------------------------
class IStudioRenderCmd
{
public:
	virtual ~IStudioRenderCmd() {}
	virtual uint32 Execute() = 0;
	virtual uint32 GetSize() const = 0;

	void* operator new( size_t size, const studioCmdAlloc_t& studioCmdAlloc );
	void operator delete( void* pPtr, const studioCmdAlloc_t& studioCmdAlloc );
};


//-----------------------------------------------------------------------------
// A rendering command that simply consumes space in the render command queue
//-----------------------------------------------------------------------------
class CStudioRenderCmd_Skip : public IStudioRenderCmd
{
public:
	// IStudioRenderCmd interface
	virtual uint32 Execute() override;
	virtual uint32 GetSize() const override;

	CStudioRenderCmd_Skip( uint32 numSkipBytes )
		: numSkipBytes( numSkipBytes )
	{}

private:
	uint32	numSkipBytes;
};


//-----------------------------------------------------------------------------
// Macros for using render commands
// NOTE: For use g_pStudioRender must be valid
//-----------------------------------------------------------------------------
#define SEND_RENDER_COMMAND( TypeName, Params ) \
	{ \
		/** Send a render command to the render thread if we're not in it */ \
		if ( !g_pStudioRender->IsInRenderThread() ) \
		{ \
			IStudioCmdBuffer*		pStudioCmdBuffer = g_pStudioRender->GetCommandBuffer(); \
			Assert( pStudioCmdBuffer ); \
			\
			/** Allocate chunk in the command buffer for our command */ \
			studioCmdAlloc_t		studioCmdAlloc = pStudioCmdBuffer->GetAllocation( sizeof( TypeName ) ); \
			\
			/** If not enough space we skip this chunk and allocate in another */ \
			if ( studioCmdAlloc.allocatedSize < sizeof( TypeName ) ) \
			{ \
				Assert( studioCmdAlloc.allocatedSize >= sizeof( CStudioRenderCmd_Skip ) ); \
				new( studioCmdAlloc ) CStudioRenderCmd_Skip( studioCmdAlloc.allocatedSize ); \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
				\
				/** Now we have enough space and can allocate our command */ \
				studioCmdAlloc = pStudioCmdBuffer->GetAllocation( sizeof( TypeName ) ); \
				new( studioCmdAlloc ) TypeName Params; \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
			} \
			/** Otherwise if we have enough space allocate our command */ \
			else \
			{ \
				new( studioCmdAlloc ) TypeName Params; \
				pStudioCmdBuffer->CommitAllocation( studioCmdAlloc ); \
			} \
		} \
		/** Otherwise we are in the render thread and simple execute our command */ \
		else \
		{ \
			TypeName	TypeName##_Command Params; \
			TypeName##_Command.Execute(); \
		} \
	}

#define UNIQUE_RENDER_COMMAND( TypeName, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	}; \
	SEND_RENDER_COMMAND( TypeName, );

#define UNIQUE_RENDER_COMMAND_ONEPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1 ) \
			: ParamName1( ParamName1 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1 ) );

#define UNIQUE_RENDER_COMMAND_TWOPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1, ParamType2 ParamName2 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2 ) );

#define UNIQUE_RENDER_COMMAND_THREEPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, ParamType3, ParamName3, ParamValue3, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( ParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
			, ParamName3( ParamName3 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
		ParamType3	ParamName3; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2, ParamValue3 ) );

#define UNIQUE_RENDER_COMMAND_FOURPARAMETER( TypeName, ParamType1, ParamName1, ParamValue1, ParamType2, ParamName2, ParamValue2, ParamType3, ParamName3, ParamValue3, ParamType4, ParamName4, ParamValue4, Code ) \
	class TypeName : public IStudioRenderCmd \
	{ \
	public: \
		TypeName( InParamType1 ParamName1, ParamType2 ParamName2, ParamType3 ParamName3, ParamType4 ParamName4 ) \
			: ParamName1( ParamName1 ) \
			, ParamName2( ParamName2 ) \
			, ParamName3( ParamName3 ) \
			, ParamName4( ParamName4 ) \
		{} \
		virtual uint32 Execute() override \
		{ \
			Code; \
			return sizeof( *this ); \
		} \
		virtual uint32 GetSize() const override \
		{ \
			return sizeof( *this ); \
		} \
	private: \
		ParamType1	ParamName1; \
		ParamType2	ParamName2; \
		ParamType3	ParamName3; \
		ParamType4	ParamName4; \
	}; \
	SEND_RENDER_COMMAND( TypeName, ( ParamValue1, ParamValue2, ParamValue3, ParamValue4 ) );


//-----------------------------------------------------------------------------
// Studio functions
//-----------------------------------------------------------------------------
void Studio_FlushRenderCommands();

#include "studiorender/istudio_rendercmd.inl"

#endif // !ISTUDIO_RENDERCMD_H