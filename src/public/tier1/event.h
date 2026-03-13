#pragma once
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/unordered_set.h>

#include "tier0/defines.h"
#include "tier0/iprofiler.h"
#include "tier1/threading.h"

//-----------------------------------------------------------------------------
// Helper macros to declare events
//-----------------------------------------------------------------------------
#define DECLARE_EVENT_INTERFACE( EventInterfaceName, ... ) typedef IEvent<__VA_ARGS__> EventInterfaceName;
#define DECLARE_EVENT( EventName, ... )					   typedef CEvent<__VA_ARGS__> EventName;

//-----------------------------------------------------------------------------
// Event interface
//-----------------------------------------------------------------------------
template<typename... TArgs>
class IEvent
{
public:
	typedef void ( *delegateFn_t )( void* /* pUserData */, TArgs... /* args */ );
	typedef uint32 handle_t;

	virtual ~IEvent() {}

	virtual handle_t Subscribe( delegateFn_t pDelegateFn, void* pUserData = NULL ) = 0;
	virtual void	 Unsubscribe( handle_t handle )								   = 0;
	virtual void	 Clear()													   = 0;
	virtual void	 Invoke( TArgs... args ) const								   = 0;
};

//-----------------------------------------------------------------------------
// Event implementation
//-----------------------------------------------------------------------------
template<typename... TArgs>
class CEvent : public IEvent<TArgs...>
{
public:
	typedef typename IEvent<TArgs...>::delegateFn_t delegateFn_t;
	typedef typename IEvent<TArgs...>::handle_t		handle_t;

	CEvent();

	virtual handle_t Subscribe( delegateFn_t pDelegateFn, void* pUserData = NULL ) override;
	virtual void	 Unsubscribe( handle_t handle ) override;
	virtual void	 Clear() override;
	virtual void	 Invoke( TArgs... args ) const override;

private:
	struct delegate_t
	{
		void*		 pUserData;
		delegateFn_t pFunc;
	};

	mutable eastl::vector<delegate_t>	   delegates;
	mutable eastl::vector<handle_t>		   handles;
	mutable eastl::unordered_set<handle_t> pendingUnsubscribeHandles;
	mutable CThreadMutex				   mutex;
	mutable uint32						   handleCounter;
	mutable bool						   bInvoked;
};

#include "tier1/event.inl"
