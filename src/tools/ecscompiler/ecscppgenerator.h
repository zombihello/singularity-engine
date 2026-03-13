#pragma once
#include "tools/ecscompiler/ecsfieldstubs.h"

//-----------------------------------------------------------------------------
// ECS C++ generator
//-----------------------------------------------------------------------------
enum ecsCppFileType_t
{
	ECS_CPP_FILE_TYPE_HEADER,
	ECS_CPP_FILE_TYPE_SOURCE
};

class CEcsCppGenerator
{
public:
	CEcsCppGenerator();
	void			 Generate( CEcsStubModule* pEcsStubModule, ecsCppFileType_t cppFileType );
	FORCEINLINE void Reset()
	{
		buffer.clear();
		bHasError = false;
	}

	FORCEINLINE bool  HasError() const { return bHasError; }
	FORCEINLINE const eastl::string& GetBuffer() const { return buffer; }

private:
	enum ecsStructType_t
	{
		ECS_STRUCT_TYPE_COMPONENT,
		ECS_STRUCT_TYPE_RESOURCE
	};

	void		GenerateHeader( CEcsStubModule* pEcsStubModule );
	void		GenerateSource( CEcsStubModule* pEcsStubModule );
	void		GenerateUsings( const eastl::vector<CRefPtr<CEcsStubUsing>>& ecsStubUsings );
	void		GenerateStructs( const eastl::vector<CRefPtr<CEcsStubDataType>>& ecsStubDataTypes, ecsStructType_t structsType );
	void		GenerateSystems( const eastl::vector<CRefPtr<CEcsStubSystem>>& ecsStubSystems );
	void		GenerateRegistrar( CEcsStubModule* pEcsStubModule );
	eastl::string GenerateRegistrarConstructor( CEcsStubModule* pEcsStubModule );
	eastl::string GenerateRegistrarDestructor( CEcsStubModule* pEcsStubModule );
	void		GenerateImplementationEcsReadDataFuncs( CEcsStubModule* pEcsStubModule );
	void		GenerateImplementationEcsFactories( CEcsStubModule* pEcsStubModule );
	void		GenerateImplementationEcsReflection( CEcsStubModule* pEcsStubModule );
	FORCEINLINE eastl::string GetStringWithUpperFirstChar( const char* pString ) const
	{
		eastl::string result = pString;
		if ( !result.empty() )
		{
			result[0] = S_ToUpper( result[0] );
		}
		return result;
	}

	bool		bHasError;
	eastl::string buffer;
};
