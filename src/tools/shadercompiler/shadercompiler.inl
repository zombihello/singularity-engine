#pragma once

/*
==================
shader_t::shader_t
==================
*/
FORCEINLINE shader_t::shader_t()
	: numFlagCombos( 0 )
	, type( STUDIOAPI_SHADER_TYPE_UNKNOWN )
{
	Mem_Memset( systemFlagIndices, (uint8)INVALID_INDEX, SHADER_SYSTEM_FLAG_NUM * sizeof( uint32 ) );
}

/*
==================
shader_t::AddFlag
==================
*/
FORCEINLINE void shader_t::AddFlag( const shaderFlag_t& flag )
{
	flags.emplace_back( flag );
}

/*
==================
shader_t::AddSystemFlag
==================
*/
FORCEINLINE void shader_t::AddSystemFlag( const shaderFlag_t& flag, shaderSystemFlag_t type )
{
	Assert( type < SHADER_SYSTEM_FLAG_NUM );
	Assert( systemFlagIndices[type] == INVALID_INDEX );
	systemFlagIndices[type] = (uint32)flags.size();
	flags.emplace_back( flag );
}

/*
==================
shader_t::GetSystemFlagValue
==================
*/
FORCEINLINE int32 shader_t::GetSystemFlagValue( shaderSystemFlag_t systemFlag, const int32* pFlagValues ) const
{
	Assert( systemFlag < SHADER_SYSTEM_FLAG_NUM );
	uint32 systemFlagIndex = systemFlagIndices[systemFlag];
	Assert( systemFlagIndex != INVALID_INDEX );
	return pFlagValues[systemFlagIndex];
}
