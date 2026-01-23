#pragma once
#include "tier1/interface.h"

//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized
//-----------------------------------------------------------------------------
class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) = 0;
	virtual void Disconnect()							 = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const char* pInterfaceName ) = 0;

	// Initialize and shutdown
	virtual bool Init()		= 0;
	virtual void Shutdown() = 0;
};

//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem
//-----------------------------------------------------------------------------
template<class TInterface>
class CBaseAppSystem : public TInterface
{
public:
	// Here's where the app systems get to learn about each other
	virtual bool Connect( createInterfaceFn_t pFactory ) { return true; }
	virtual void Disconnect() {}

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface( const char* pInterfaceName ) { return NULL; }

	// Initialize and shutdown
	virtual bool Init() { return true; }
	virtual void Shutdown() {}
};