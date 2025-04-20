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
	void GenerateUsings( const std::vector<TRefPtr<CEcsStubUsing>>& ecsStubUsings );
	void GenerateComponents( const std::vector<TRefPtr<CEcsStubComponent>>& ecsStubComponents );
	void GenerateSystems( const std::vector<TRefPtr<CEcsStubSystem>>& ecsStubSystems );
	void GenerateRegistrar( CEcsStubModule* pEcsStubModule );

	std::string		buffer;
};

#endif // !ECSCPPGENERATOR_H
