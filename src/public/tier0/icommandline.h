#pragma once
#include "tier0/tier0_api.h"
#include "tier1/types.h"

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

	virtual void Init( const char* pCommandLine ) = 0;
	virtual void Shutdown()						   = 0;

	virtual bool		  HasParam( const char* pParam ) const						 = 0;
	virtual bool		  HasParam( const char* pParam, const char* pValue ) const = 0;
	virtual const char*  GetFirstValue( const char* pParam ) const				 = 0;
	virtual const char** GetValues( const char* pParam, uint32& size ) const		 = 0;
};

TIER0_INTERFACE ICommandLine* CommandLine();
