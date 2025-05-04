#ifndef ECSCPPGENERATOR_H
#define ECSCPPGENERATOR_H

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
	void Generate( CEcsStubModule* pEcsStubModule, ecsCppFileType_t cppFileType );
	FORCEINLINE void Reset()							
	{ 
		buffer.clear(); 
		bHasError = false; 
	}

	FORCEINLINE bool HasError() const					{ return bHasError; }
	FORCEINLINE const std::string& GetBuffer() const	{ return buffer; }

private:
	enum ecsStructType_t
	{
		ECS_STRUCT_TYPE_COMPONENT,
		ECS_STRUCT_TYPE_RESOURCE
	};

	void GenerateHeader( CEcsStubModule* pEcsStubModule );
	void GenerateSource( CEcsStubModule* pEcsStubModule );
	void GenerateUsings( const std::vector<TRefPtr<CEcsStubUsing>>& ecsStubUsings );
	void GenerateStructs( const std::vector<TRefPtr<CEcsStubDataType>>& ecsStubDataTypes, ecsStructType_t structsType );
	void GenerateSystems( const std::vector<TRefPtr<CEcsStubSystem>>& ecsStubSystems );
	void GenerateRegistrar( CEcsStubModule* pEcsStubModule );
	void GenerateImplementationEcsReadDataFuncs( CEcsStubModule* pEcsStubModule );
	void GenerateImplementationEcsFactories( CEcsStubModule* pEcsStubModule );
	FORCEINLINE std::string GetStringWithUpperFirstChar( const achar* pString ) const
	{
		std::string		result = pString;
		if ( !result.empty() )
		{
			result[0] = S_ToUpper( result[0] );
		}
		return result;
	}

	bool			bHasError;
	std::string		buffer;
};

#endif // !ECSCPPGENERATOR_H
