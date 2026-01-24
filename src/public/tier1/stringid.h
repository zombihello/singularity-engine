#pragma once
#include "tier1/stringpool.h"

//-----------------------------------------------------------------------------
// String ID
//-----------------------------------------------------------------------------
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
class TStringId
{
public:
	// Functions to extract the string ID as a key for eastl::unordered_map and eastl::unordered_set
	struct keyFunc_t
	{
		size_t operator()( const TStringId& stringID ) const;
		bool   operator()( const TStringId& a, const TStringId& b ) const;
	};

	TStringId();
	TStringId( const char* pString );
	TStringId( const char* pString, uint64 length );
	TStringId( const eastl::string& string );
	TStringId( const eastl::string_view& string );
	TStringId( const TStringId& copy );

	static TStringId Make( const char* pString );
	static TStringId Make( const char* pString, uint64 length );
	static TStringId Make( const eastl::string& string );
	static TStringId Make( const eastl::string_view& string );
	static TStringId Make( const TStringId& stringId );

	void		Clear();
	const char* ToString() const;

	bool   IsEmpty() const;
	hash_t GetHash() const;

	bool operator==( const char* pString ) const;
	bool operator==( const eastl::string& string ) const;
	bool operator==( const eastl::string_view& string ) const;
	bool operator==( const TStringId& stringId ) const;
	bool operator!=( const char* pString ) const;
	bool operator!=( const eastl::string& string ) const;
	bool operator!=( const eastl::string_view& string ) const;
	bool operator!=( const TStringId& stringId ) const;

	TStringId& operator=( const char* pString );
	TStringId& operator=( const eastl::string& string );
	TStringId& operator=( const eastl::string_view& string );
	TStringId& operator=( const TStringId& stringId );

private:
	TIdType id;
};

typedef TStringId<uint16, GetGlobalStringPool> CStringId;

#include "tier1/stringid.inl"
