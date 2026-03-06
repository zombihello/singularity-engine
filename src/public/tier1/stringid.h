#pragma once
#include "tier1/stringpool.h"

//-----------------------------------------------------------------------------
// String ID
//-----------------------------------------------------------------------------
class CStringID
{
public:
	// Functions to extract the string ID as a key for eastl::unordered_map and eastl::unordered_set
	struct keyFunc_t
	{
		size operator()( const CStringID& stringID ) const;
		bool   operator()( const CStringID& a, const CStringID& b ) const;
	};

	CStringID();
	CStringID( const char* pString );
	CStringID( const char* pString, uint64 length );
	CStringID( const eastl::string& string );
	CStringID( const eastl::string_view& string );
	CStringID( const CStringID& copy );

	static CStringID Make( const char* pString );
	static CStringID Make( const char* pString, uint64 length );
	static CStringID Make( const eastl::string& string );
	static CStringID Make( const eastl::string_view& string );
	static CStringID Make( const CStringID& stringId );

	void		Clear();
	const char* ToString() const;

	bool   IsEmpty() const;
	hash GetHash() const;

	bool operator==( const char* pString ) const;
	bool operator==( const eastl::string& string ) const;
	bool operator==( const eastl::string_view& string ) const;
	bool operator==( const CStringID& stringId ) const;
	bool operator!=( const char* pString ) const;
	bool operator!=( const eastl::string& string ) const;
	bool operator!=( const eastl::string_view& string ) const;
	bool operator!=( const CStringID& stringId ) const;

	CStringID& operator=( const char* pString );
	CStringID& operator=( const eastl::string& string );
	CStringID& operator=( const eastl::string_view& string );
	CStringID& operator=( const CStringID& stringId );

private:
	typedef uint16			  id_t;
	static TStringPool<id_t>& GetStringPool();
	id_t					  id;
};

#include "tier1/stringid.inl"
