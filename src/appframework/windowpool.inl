#pragma once

/*
==================
CWindowPool::~CWindowPool
==================
*/
template<typename T>
FORCEINLINE CWindowPool<T>::~CWindowPool()
{
	for ( size index = 0, count = windows.size(); index < count; ++index )
	{
		delete windows[index];
	}
}

/*
==================
CWindowPool::Create
==================
*/
template<typename T>
FORCEINLINE T* CWindowPool<T>::Create()
{
	windowId_t id = INVALID_INDEX;
	if ( !freeIds.empty() )
	{
		id = freeIds.front();
		freeIds.pop_front();
	}
	else
	{
		size numWindows = windows.size();
		AssertMsg( numWindows < S_MaxValue<windowId_t>(), "windowId_t overflow, too many windows" );
		id = (windowId_t)numWindows;
		windows.emplace_back( (T*)NULL );
	}

	T*& pWindow = windows[id];
	pWindow		= new T( id );
	return pWindow;
}

/*
==================
CWindowPool::Free
==================
*/
template<typename T>
FORCEINLINE void CWindowPool<T>::Free( windowId_t id )
{
	Assert( id < windows.size() );
	T*& pWindow = windows[id];
	freeIds.emplace_back( id );
	delete pWindow;
	pWindow = NULL;
}

/*
==================
CWindowPool::Get
==================
*/
template<typename T>
FORCEINLINE T* CWindowPool<T>::Get( windowId_t id ) const
{
	Assert( id < windows.size() && windows[id] );
	return windows[id];
}
