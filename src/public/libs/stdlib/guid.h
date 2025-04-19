#ifndef GUID_H
#define GUID_H

#include "stdlib/strtools.h"
#include "stdlib/hashing/fasthash.h"

//-----------------------------------------------------------------------------
// Guid
//-----------------------------------------------------------------------------
class CGuid
{
public:
	// Functions to extract the Guid as a key for std::unordered_map and std::unordered_set
	struct keyFunc_t
	{
		std::size_t operator()( const CGuid& guid ) const;
		bool operator()( const CGuid& a, const CGuid& b ) const;
	};

	CGuid()
		: a( 0 ), b( 0 ), c( 0 ), d( 0 )
	{}
	CGuid( uint32 a, uint32 b, uint32 c, uint32 d )
		: a( a ), b( b ), c( c ), d( d )
	{}
	CGuid( const CGuid& guid )
		: a( guid.a ), b( guid.b ), c( guid.c ), d( guid.d )
	{}

	void Set( uint32 a, uint32 b, uint32 c, uint32 d );
	bool InitFromString( const std::string& string );
	void Invalidate();

	bool IsValid() const;
	std::string AsString() const;
	hash_t GetHash() const;

	friend bool operator==( const CGuid& x, const CGuid& y );
	friend bool operator!=( const CGuid& x, const CGuid& y );
	bool operator<( const CGuid& other ) const;

private:
	uint32		a;
	uint32		b;
	uint32		c;
	uint32		d;
};

#include "stdlib/guid.inl"

#endif // !GUID_H