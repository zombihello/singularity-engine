#ifndef ECSCPPGENERATOR_H
#define ECSCPPGENERATOR_H

#include "tools/ecscompiler/ecsfieldstubs.h"

//-----------------------------------------------------------------------------
// ECS C++ generator
//-----------------------------------------------------------------------------
class CEcsCppGenerator
{
public:
	void Generate( CEcsStubModule* pEcsStubModule );

	FORCEINLINE void Reset()							{ buffer.clear(); }
	FORCEINLINE const std::string& GetBuffer() const	{ return buffer; }

private:
	enum ecsStructType_t
	{
		ECS_STRUCT_TYPE_COMPONENT,
		ECS_STRUCT_TYPE_RESOURCE
	};

	void GenerateUsings( const std::vector<TRefPtr<CEcsStubUsing>>& ecsStubUsings );
	void GenerateStructs( const std::vector<TRefPtr<CEcsStubDataType>>& ecsStubDataTypes, ecsStructType_t structsType );
	void GenerateSystems( const std::vector<TRefPtr<CEcsStubSystem>>& ecsStubSystems );
	void GenerateRegistrar( CEcsStubModule* pEcsStubModule );
	FORCEINLINE std::string GetStringWithUpperFirstChar( const achar* pString ) const
	{
		std::string		result = pString;
		if ( !result.empty() )
		{
			result[0] = S_ToUpper( result[0] );
		}
		return result;
	}

	std::string		buffer;
};

#endif // !ECSCPPGENERATOR_H
