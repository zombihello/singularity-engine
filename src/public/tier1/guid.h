#pragma once
#include "tier1/strtools.h"
#include "tier1/fasthash.h"

//-----------------------------------------------------------------------------
// Guid
//-----------------------------------------------------------------------------
class CGuid
{
public:
	// Functions to extract the Guid as a key for eastl::unordered_map and eastl::unordered_set
	struct keyFunc_t
	{
		size operator()( const CGuid& guid ) const;
		bool operator()( const CGuid& a, const CGuid& b ) const;
	};

	CGuid();
	CGuid( uint32 a, uint32 b, uint32 c, uint32 d );
	CGuid( const CGuid& other );

	static CGuid Generate();
	static void	 Generate( CGuid& guid );

	void Set( uint32 a, uint32 b, uint32 c, uint32 d );
	bool Set( const char* pString );
	bool Set( const wchar_t* pString );
	void Clear();

	bool		  IsValid() const;
	eastl::string ToString() const;
	hash		  GetHash() const;

	friend bool operator==( const CGuid& x, const CGuid& y );
	friend bool operator!=( const CGuid& x, const CGuid& y );
	bool		operator<( const CGuid& other ) const;

private:
	uint32 a;
	uint32 b;
	uint32 c;
	uint32 d;
};

#include "tier1/guid.inl"
