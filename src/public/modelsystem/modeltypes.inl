#pragma once

/*
==================
modelSimpleVertex_t::operator==
==================
*/
FORCEINLINE bool modelSimpleVertex_t::operator==( const modelSimpleVertex_t& other ) const
{
	return position == other.position && texCoord == other.texCoord && color == other.color;
}

/*
==================
modelStaticVertex_t::operator==
==================
*/
FORCEINLINE bool modelStaticVertex_t::operator==( const modelStaticVertex_t& other ) const
{
	return position == other.position && texCoord == other.texCoord && normal == other.normal && tangent == other.tangent && binormal == other.binormal;
}