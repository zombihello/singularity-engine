#pragma once

/*
==================
CDebugNamed::CDebugNamed
==================
*/
template<class TBaseClass>
FORCEINLINE CDebugNamed<TBaseClass>::CDebugNamed( const char* pDebugName /* = "" */ )
#if !RETAIL
	: pDebugName( pDebugName )
#endif	// !RETAIL
{
}

/*
==================
CDebugNamed::GetDebugName
==================
*/
template<class TBaseClass>
FORCEINLINE const char* CDebugNamed<TBaseClass>::GetDebugName() const
{
#if !RETAIL
	return pDebugName;
#else
	return "";
#endif	// !RETAIL
}
