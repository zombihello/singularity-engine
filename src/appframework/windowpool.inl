#pragma once

/*
==================
TWindowPool::~TWindowPool
==================
*/
template<typename T>
FORCEINLINE TWindowPool<T>::~TWindowPool()
{
	for ( size index = 0, count = windows.size(); index < count; ++index )
	{
		delete windows[index];
	}
}

/*
==================
TWindowPool::Create
==================
*/
template<typename T>
FORCEINLINE T* TWindowPool<T>::Create()
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
TWindowPool::Free
==================
*/
template<typename T>
FORCEINLINE void TWindowPool<T>::Free( windowId_t id )
{
	Assert( id < windows.size() );
	T*& pWindow = windows[id];
	freeIds.emplace_back( id );
	delete pWindow;
	pWindow = NULL;
}

/*
==================
TWindowPool::Get
==================
*/
template<typename T>
FORCEINLINE T* TWindowPool<T>::Get( windowId_t id ) const
{
	Assert( id < windows.size() && windows[id] );
	return windows[id];
}
