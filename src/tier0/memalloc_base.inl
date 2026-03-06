#pragma once

/*
==================
CMemAllocBase::GetAlignment
==================
*/
FORCEINLINE uint32 CMemAllocBase::GetAlignment( size numBytes, uint32 alignment ) const
{
	return Max<uint32>( numBytes >= DEFAULT_ALIGNMENT ? DEFAULT_ALIGNMENT : MIN_ALIGNMENT, alignment );
}

/*
==================
CMemAllocBase::OutOfMemory
==================
*/
FORCEINLINE void CMemAllocBase::OutOfMemory( size numBytes, uint32 alignment ) const
{
	alignment = GetAlignment( numBytes, alignment );
	Sys_Error( "Ran out of memory allocating %llu bytes (numBytes: %llu, alignment: %u)", Align<size>( numBytes, alignment ), numBytes, alignment );
}
