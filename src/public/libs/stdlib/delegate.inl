#ifndef DELEGATE_INL
#define DELEGATE_INL

/*
==================
TMulticastDelegate::AddFunc
==================
*/
template<typename... TParamTypes>
typename TMulticastDelegate<TParamTypes...>::funcDelegate_t* TMulticastDelegate<TParamTypes...>::AddFunc( delegateFn_t pDelegateFn, void* pUserData /*= NULL*/ )
{
	PROFILE_SCOPE();
	CScopeLock		scopeLock( mutex );
	return &funcDelegates.emplace_back( funcDelegate_t{ pUserData, pDelegateFn } );
}

/*
==================
TMulticastDelegate::RemoveAll
==================
*/
template<typename... TParamTypes>
void TMulticastDelegate<TParamTypes...>::RemoveAll()
{
	PROFILE_SCOPE();
	CScopeLock		scopeLock( mutex );
	funcDelegates.clear();
}

/*
==================
TMulticastDelegate::RemoveFunc
==================
*/
template<typename... TParamTypes>
void TMulticastDelegate<TParamTypes...>::RemoveFunc( funcDelegate_t* pDelegate )
{
	PROFILE_SCOPE();
	if ( pDelegate )
	{
		CScopeLock		scopeLock( mutex );
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
TMulticastDelegate::Broadcast
==================
*/
template<typename... TParamTypes>
void TMulticastDelegate<TParamTypes...>::Broadcast( TParamTypes... params ) const
{
	PROFILE_SCOPE();
	CScopeLock		scopeLock( mutex );
	for ( auto it = funcDelegates.begin(), itEnd = funcDelegates.end(); it != itEnd; ++it )
	{
		const funcDelegate_t&	funcDelegate = *it;
		( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
	}
}


/*
==================
TDelegate::BindFunc
==================
*/
template<typename... TParamTypes>
void TDelegate<TParamTypes...>::BindFunc( delegateFn_t pDelegateFn, void* pUserData /*= NULL*/ )
{
	CScopeLock				scopeLock( mutex );
	funcDelegate.pFunc		= pDelegateFn;
	funcDelegate.pUserData	= pUserData;
}

/*
==================
TDelegate::Unbind
==================
*/
template<typename... TParamTypes>
void TDelegate<TParamTypes...>::Unbind()
{
	CScopeLock		scopeLock( mutex );
	Mem_Memzero( &funcDelegate, sizeof( funcDelegate_t ) );
}

/*
==================
TDelegate::Execute
==================
*/
template<typename... TParamTypes>
void TDelegate<TParamTypes...>::Execute( TParamTypes... params )
{
	PROFILE_SCOPE();
	CScopeLock		scopeLock( mutex );
	( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
}

#endif // !DELEGATE_INL
