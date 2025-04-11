/**
 * @file
 * @addtogroup stdlib stdlib
 */

#ifndef DELEGATE_H
#define DELEGATE_H

#include <list>

#include "core/platform.h"
#include "core/threading.h"
#include "core/profile.h"

/**
 * @ingroup stdlib
 * @brief Multicast delegate interface
 */
template<typename... TParamTypes>
class IMulticastDelegate
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IMulticastDelegate() {}

	/**
	 * @brief Delegate type for functions
	 */
	typedef void ( *delegateFn_t )( void* /* pUserData */, TParamTypes... /* params */ );

	/**
	 * @brief Function delegate info
	 */
	struct funcDelegate_t
	{
		void*				pUserData;	/**< Pointer to user data */
		delegateFn_t	pFunc;		/**< Pointer to delegate function */
	};

	/**
	 * @brief Add function delegate
	 * 
	 * @param pUserData		Pointer to user data
	 * @param pDelegate		Function delegate
	 * @return Return pointer to delegate handle
	 */
	virtual funcDelegate_t* AddFunc( delegateFn_t pDelegateFn, void* pUserData = nullptr ) = 0;

	/**
	 * @brief Remove all function delegates
	 */
	virtual void RemoveAll() = 0;

	/**
	 * @brief Remove function delegate
	 * @param pDelegate		Delegate handle
	 */
	virtual void RemoveFunc( funcDelegate_t* pDelegate ) = 0;

	/**
	 * @brief Broadcast all delegates
	 * @param params	Params for call delegate
	 */
	virtual void Broadcast( TParamTypes... params ) const = 0;
};

/**
 * @ingroup stdlib
 * @brief Single cast delegate interface
 */
template<typename... TParamTypes>
class IDelegate
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IDelegate() {}

	/**
	 * @brief Delegate type for functions
	 */
	typedef void ( *delegateFn_t )( void* /* pUserData */, TParamTypes... /* params */ );

	/**
	 * @brief Bind function delegate
	 *
	 * @param pDelegate		Delegate
	 * @param pUserData		Pointer to user data
	 */
	virtual void BindFunc( delegateFn_t pDelegateFn, void* pUserData = nullptr ) = 0;

	/**
	 * @brief Unbind delegate
	 */
	virtual void Unbind() = 0;

	/**
	 * @brief Execute delegate
	 * @param params	Params for call delegate
	 */
	virtual void Execute( TParamTypes... params ) = 0;
};

/**
 * @ingroup stdlib
 * @brief Multicast delegate
 */
template<typename... TParamTypes>
class TMulticastDelegate : public IMulticastDelegate<TParamTypes...>
{
public:
	using typename IMulticastDelegate<TParamTypes...>::funcDelegate_t;
	using typename IMulticastDelegate<TParamTypes...>::delegateFn_t;

	/**
	 * @brief Add function delegate
	 *
	 * @param pUserData		Pointer to user data
	 * @param pDelegate		Function delegate
	 * @return Return pointer to delegate handle
	 */
	virtual funcDelegate_t* AddFunc( delegateFn_t pDelegateFn, void* pUserData = nullptr ) override
	{
		PROFILE_SCOPE();
		CScopeLock		scopeLock( mutex );
		return &funcDelegates.emplace_back( funcDelegate_t{ pUserData, pDelegateFn } );
	}

	/**
	 * @brief Remove all function delegates
	 */
	virtual void RemoveAll() override
	{
		PROFILE_SCOPE();
		CScopeLock		scopeLock( mutex );
		funcDelegates.clear();
	}

	/**
	 * @brief Remove function delegate
	 * @param pDelegate		Delegate handle
	 */
	virtual void RemoveFunc( funcDelegate_t* pDelegate ) override
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

	/**
	 * @brief Broadcast all delegates
	 * @param params	Params for call delegate
	 */
	virtual void Broadcast( TParamTypes... params ) const override
	{
		PROFILE_SCOPE();
		CScopeLock		scopeLock( mutex );
		for ( auto it = funcDelegates.begin(), itEnd = funcDelegates.end(); it != itEnd; ++it )
		{
			const funcDelegate_t&	funcDelegate = *it;
			( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
		}
	}

private:
	std::list<funcDelegate_t>		funcDelegates;	/**< Function delegates */
	mutable CThreadMutex			mutex;			/**< Mutex for thread safe broadcast */
};

/**
 * @ingroup stdlib
 * @brief Single cast delegate
 */
template<typename... TParamTypes>
class TDelegate : public IDelegate<TParamTypes...>
{
public:
	using typename IDelegate<TParamTypes...>::delegateFn_t;

	/**
	 * @brief Function delegate info
	 */
	struct funcDelegate_t
	{
		void*				pUserData;	/**< Pointer to user data */
		delegateFn_t	pFunc;		/**< Pointer to delegate function */
	};

	/**
	 * @brief Bind function delegate
	 *
	 * @param pDelegate		Delegate
	 * @param pUserData		Pointer to user data
	 */
	virtual void BindFunc( delegateFn_t pDelegateFn, void* pUserData = nullptr ) override
	{
		CScopeLock				scopeLock( mutex );
		funcDelegate.pFunc		= pDelegateFn;
		funcDelegate.pUserData	= pUserData;
	}

	/**
	 * @brief Unbind delegate
	 */
	virtual void Unbind() override
	{
		CScopeLock		scopeLock( mutex );
		Mem_Memzero( &funcDelegate, sizeof( funcDelegate_t ) );
	}

	/**
	 * @brief Execute delegate
	 * @param params	Params for call delegate
	 */
	virtual void Execute( TParamTypes... params ) override
	{
		PROFILE_SCOPE();
		CScopeLock		scopeLock( mutex );
		( *funcDelegate.pFunc )( funcDelegate.pUserData, params... );
	}

private:
	funcDelegate_t			funcDelegate;	/**< Function delegate */
	mutable CThreadMutex	mutex;			/**< Mutex for thread safe broadcast */
};

/**
 * @ingroup stdlib
 * @brief Macro for declare multicast delegate interface
 *
 * @param DelegateInterfaceName		Delegate interface name
 * @param ...						Other parameters of delegate
 */
#define DECLARE_MULTICAST_DELEGATE_INTERFACE( DelegateInterfaceName, ... )	\
	typedef IMulticastDelegate<__VA_ARGS__>			DelegateInterfaceName;

/**
 * @ingroup stdlib
 * @brief Macro for declare multicast delegate
 *
 * @param DelegateName	Delegate name
 * @param ...			Other parameters of delegate
 */
#define DECLARE_MULTICAST_DELEGATE( DelegateName, ... )	\
	typedef TMulticastDelegate<__VA_ARGS__>			DelegateName;

/**
 * @ingroup stdlib
 * @brief Macro for declare single cast delegate interface
 *
 * @param DelegateInterfaceName		Delegate interface name
 * @param ...						Other parameters of delegate
 */
#define DECLARE_DELEGATE_INTERFACE( DelegateInterfaceName, ... )	\
	typedef IDelegate<__VA_ARGS__>					DelegateInterfaceName;

/**
 * @ingroup stdlib
 * @brief Macro for declare single cast delegate
 *
 * @param DelegateName	Delegate name
 * @param ...			Other parameters of delegate
 */
#define DECLARE_DELEGATE( DelegateName, ... )	\
	typedef TDelegate<__VA_ARGS__>					DelegateName;

#endif // !DELEGATE_H