#pragma once
#include "core/coreapi.h"
#include "stdlib/types.h"

//-----------------------------------------------------------------------------
// Command line interface
// Holds parsed data from command line like strings
// 
// Usage:
// 		-<Param>=<Value>
//		/<Param>=<Value>
//		-<Param>="<Value> <Value>"
//		/<Param>="<Value> <Value>"
//		-<Param>
//		/<Param>
//		-<Param> <Value>
//		/<Param> <Value>
//		-<Param> "<Value> <Value>"
//		/<Param> "<Value> <Value>"
//-----------------------------------------------------------------------------
class ICommandLine
{
public:
	virtual ~ICommandLine() {}

	virtual void Init( const achar* pCommandLine ) = 0;
	virtual void Shutdown() = 0;

	virtual bool HasParam( const achar* pParam ) const = 0;
	virtual bool HasParam( const achar* pParam, const achar* pValue ) const = 0;
	virtual const achar* GetFirstValue( const achar* pParam ) const = 0;
	virtual const achar** GetValues( const achar* pParam, uint32& size ) const = 0;
};

CORE_INTERFACE ICommandLine* CommandLine();