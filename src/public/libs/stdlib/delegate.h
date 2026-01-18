#pragma once
#include <list>

#include "stdlib/defines.h"
#include "core/threading.h"
#include "core/profile.h"

//-----------------------------------------------------------------------------
// Helper macros to declare delegates
//-----------------------------------------------------------------------------
#define DECLARE_MULTICAST_DELEGATE_INTERFACE( DelegateInterfaceName, ... ) typedef IMulticastDelegate<__VA_ARGS__> DelegateInterfaceName;
#define DECLARE_MULTICAST_DELEGATE( DelegateName, ... )					   typedef TMulticastDelegate<__VA_ARGS__> DelegateName;
#define DECLARE_DELEGATE_INTERFACE( DelegateInterfaceName, ... )		   typedef IDelegate<__VA_ARGS__> DelegateInterfaceName;
#define DECLARE_DELEGATE( DelegateName, ... )							   typedef TDelegate<__VA_ARGS__> DelegateName;

//-----------------------------------------------------------------------------
// Delegate interfaces
//-----------------------------------------------------------------------------
template<typename... TParamTypes>
class IMulticastDelegate
{
public:
	typedef void ( *delegateFn_t )( void* /* pUserData */, TParamTypes... /* params */ );
	struct funcDelegate_t
	{
		void*		 pUserData;
		delegateFn_t pFunc;
	};

	virtual ~IMulticastDelegate() {}

	virtual funcDelegate_t* AddFunc( delegateFn_t pDelegateFn, void* pUserData = NULL ) = 0;
	virtual void			RemoveAll()													= 0;
	virtual void			RemoveFunc( funcDelegate_t* pDelegate )						= 0;
	virtual void			Broadcast( TParamTypes... params ) const					= 0;
};

template<typename... TParamTypes>
class IDelegate
{
public:
	typedef void ( *delegateFn_t )( void* /* pUserData */, TParamTypes... /* params */ );

	virtual ~IDelegate() {}

	virtual void BindFunc( delegateFn_t pDelegateFn, void* pUserData = nullptr ) = 0;
	virtual void Unbind()														 = 0;
	virtual void Execute( TParamTypes... params )								 = 0;
};

//-----------------------------------------------------------------------------
// Delegate implementations
//-----------------------------------------------------------------------------
template<typename... TParamTypes>
class TMulticastDelegate : public IMulticastDelegate<TParamTypes...>
{
public:
	using typename IMulticastDelegate<TParamTypes...>::funcDelegate_t;
	using typename IMulticastDelegate<TParamTypes...>::delegateFn_t;

	virtual funcDelegate_t* AddFunc( delegateFn_t pDelegateFn, void* pUserData = NULL ) override;
	virtual void			RemoveAll() override;
	virtual void			RemoveFunc( funcDelegate_t* pDelegate ) override;
	virtual void			Broadcast( TParamTypes... params ) const override;

private:
	std::list<funcDelegate_t> funcDelegates;
	mutable CThreadMutex	  mutex;
};

template<typename... TParamTypes>
class TDelegate : public IDelegate<TParamTypes...>
{
public:
	using typename IDelegate<TParamTypes...>::delegateFn_t;
	struct funcDelegate_t
	{
		void*		 pUserData;
		delegateFn_t pFunc;
	};

	virtual void BindFunc( delegateFn_t pDelegateFn, void* pUserData = NULL ) override;
	virtual void Unbind() override;
	virtual void Execute( TParamTypes... params ) override;

private:
	funcDelegate_t		 funcDelegate;
	mutable CThreadMutex mutex;
};

#include "stdlib/delegate.inl"