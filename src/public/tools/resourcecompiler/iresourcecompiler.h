#pragma once
#include "tier1/interface.h"

//-----------------------------------------------------------------------------
// Resource compiler interface
//-----------------------------------------------------------------------------
#define RESOURCECOMPILER_INTERFACE_VERSION "SResourceCompiler001"
class IResourceCompiler
{
public:
	// Initialize and shutdown
	virtual bool Init( createInterfaceFn_t pFactory ) = 0;
	virtual void Shutdown()							  = 0;

	// Compile a resource
	// pSrcPath		Path to a source file (with file extension)
	// pDestPath	Path to a destination file (without file extension)
	virtual bool Compile( const char* pSrcPath, const char* pDestPath ) const = 0;

	// Get source formats and resource type name
	virtual uint32		GetNumSrcFormats() const		   = 0;
	virtual const char* GetSrcFormat( uint32 index ) const = 0;
	virtual const char* GetName() const					   = 0;
};
