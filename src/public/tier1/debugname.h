#pragma once
#include "tier0/defines.h"
#include "tier0/types.h"

//-----------------------------------------------------------------------------
// Debug name helpers
//-----------------------------------------------------------------------------
#if !RETAIL
	#define DEBUGNAME( Name ) Name
	#define DEBUGNAMEF( ... ) va( __VA_ARGS__ )
#else
	#define DEBUGNAME( Name ) ""
	#define DEBUGNAMEF( ... ) ""
#endif	// !RETAIL

//-----------------------------------------------------------------------------
// Debug named interface
//
// NOTES:
//	* Interface for a resource that exposes an optional debug name, used to label
//	  it in debugging tools
//	* Derive your interface from it, implement via CDebugNamed<>
//-----------------------------------------------------------------------------
class IDebugNamed
{
public:
	virtual const char* GetDebugName() const = 0;
};

//-----------------------------------------------------------------------------
// Debug named
//
// NOTES:
//	* Template mixin that implements IDebugNamed over a base interface TBaseClass
//	* Compiled out in Retail: nothing is stored and GetDebugName() returns ""
//
// WARNING:
//	* The name is stored by pointer, NOT copied - the passed string must outlive
//	  this object (a literal or an owned/persistent buffer, never a temporary)
//-----------------------------------------------------------------------------
template<class TBaseClass>
class CDebugNamed : public TBaseClass
{
public:
	CDebugNamed( const char* pDebugName = "" );
	virtual const char* GetDebugName() const override;

private:
#if !RETAIL
	const char* pDebugName;
#endif	// !RETAIL
};

#include "tier1/debugname.inl"
