#pragma once

/*
==================
CEvent::CEvent
==================
*/
template<typename... TArgs>
FORCEINLINE CEvent<TArgs...>::CEvent()
	: handleCounter( 0 )
	, bInvoked( false )
{
}

/*
==================
CEvent::Subscribe
==================
*/
template<typename... TArgs>
FORCEINLINE typename CEvent<TArgs...>::handle_t CEvent<TArgs...>::Subscribe( delegateFn_t pDelegateFn, void* pUserData /*= NULL*/ )
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	handle_t   handle = handleCounter;
	++handleCounter;

	delegates.emplace_back( delegate_t{ pUserData, pDelegateFn } );
	handles.emplace_back( handle );
	return handle;
}

/*
==================
CEvent::Unsubscribe
==================
*/
template<typename... TArgs>
FORCEINLINE void CEvent<TArgs...>::Unsubscribe( handle_t handle )
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	if ( handle == INVALID_INDEX )
	{
		return;
	}

	if ( !bInvoked )
	{
		uint32 indexFound = INVALID_INDEX;
		for ( uint32 index = 0, count = (uint32)handles.size(); index < count; ++index )
		{
			if ( handles[index] == handle )
			{
				indexFound = index;
				break;
			}
		}

		if ( indexFound != INVALID_INDEX )
		{
			delegates.erase( delegates.begin() + indexFound );
			handles.erase( handles.begin() + indexFound );
		}
	}
	else
	{
		pendingUnsubscribeHandles.insert( handle );
	}
}

/*
==================
CEvent::Clear
==================
*/
template<typename... TArgs>
FORCEINLINE void CEvent<TArgs...>::Clear()
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	if ( !bInvoked )
	{
		delegates.clear();
		handles.clear();
	}
	else
	{
		for ( uint32 index = 0, count = (uint32)handles.size(); index < count; ++index )
		{
			pendingUnsubscribeHandles.insert( handles[index] );
		}
	}
}

/*
==================
CEvent::Invoke
==================
*/
template<typename... TArgs>
FORCEINLINE void CEvent<TArgs...>::Invoke( TArgs... args ) const
{
	// Invoke all delegates
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	bInvoked = true;
	for ( uint32 index = 0, count = (uint32)delegates.size(); index < count; ++index )
	{
		const delegate_t& delegate = delegates[index];
		( *delegate.pFunc )( delegate.pUserData, args... );
	}
	bInvoked = false;

	// Unsubscribe pending handles
	if ( !pendingUnsubscribeHandles.empty() )
	{
		for ( auto it = pendingUnsubscribeHandles.begin(), itEnd = pendingUnsubscribeHandles.end(); it != itEnd; ++it )
		{
			const_cast<CEvent*>( this )->Unsubscribe( *it );
		}
		pendingUnsubscribeHandles.clear();
	}
}
