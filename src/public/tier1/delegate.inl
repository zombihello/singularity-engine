#pragma once

/*
==================
CMulticastDelegate::AddFunc
==================
*/
template<typename... TParamTypes>
typename CMulticastDelegate<TParamTypes...>::funcDelegate_t* CMulticastDelegate<TParamTypes...>::AddFunc( delegateFn_t pDelegateFn, void* pUserData /*= NULL*/ )
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	return &funcDelegates.emplace_back( funcDelegate_t{ pUserData, pDelegateFn } );
}

/*
==================
CMulticastDelegate::RemoveAll
==================
*/
template<typename... TParamTypes>
void CMulticastDelegate<TParamTypes...>::RemoveAll()
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	funcDelegates.clear();
}

/*
==================
CMulticastDelegate::RemoveFunc
==================
*/
template<typename... TParamTypes>
void CMulticastDelegate<TParamTypes...>::RemoveFunc( funcDelegate_t* pDelegate )
{
	PROFILER_SCOPE_FUNC();
	if ( pDelegate )
	{
		CScopeLock scopeLock( mutex );
		for ( auto it = funcDelegates.begin(), itEnd = funcDelegates.end(); it != itEnd; ++it )
		{
			if ( &( *it ) == pDelegate )
			{
				funcDelegates.erase( it );
				return;
			}
		}
	}
}

/*
==================
CMulticastDelegate::Broadcast
==================
*/
template<typename... TParamTypes>
void CMulticastDelegate<TParamTypes...>::Broadcast( TParamTypes... params ) const
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	for ( auto it = funcDelegates.begin(), itEnd = funcDelegates.end(); it != itEnd; ++it )
	{
		const funcDelegate_t& funcDelegate = *it;
		( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
	}
}

/*
==================
CDelegate::BindFunc
==================
*/
template<typename... TParamTypes>
void CDelegate<TParamTypes...>::BindFunc( delegateFn_t pDelegateFn, void* pUserData /*= NULL*/ )
{
	CScopeLock scopeLock( mutex );
	funcDelegate.pFunc	   = pDelegateFn;
	funcDelegate.pUserData = pUserData;
}

/*
==================
CDelegate::Unbind
==================
*/
template<typename... TParamTypes>
void CDelegate<TParamTypes...>::Unbind()
{
	CScopeLock scopeLock( mutex );
	Mem_Memzero( &funcDelegate, sizeof( funcDelegate_t ) );
}

/*
==================
CDelegate::Execute
==================
*/
template<typename... TParamTypes>
void CDelegate<TParamTypes...>::Execute( TParamTypes... params )
{
	PROFILER_SCOPE_FUNC();
	CScopeLock scopeLock( mutex );
	( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
}
