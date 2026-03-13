#pragma once

/*
==================
CGuardValue<TType>::operator*
==================
*/
template<typename TType>
FORCEINLINE const TType& CGuardValue<TType>::operator*() const
{
	return oldValue;
}
