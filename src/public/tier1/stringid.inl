#pragma once

/*
==================
TStringId::keyFunc_t::operator()
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE size_t TStringId<TIdType, GetStringPool>::keyFunc_t::operator()( const TStringId& stringID ) const
{
	return stringID.GetHash();
}

/*
==================
TStringId::keyFunc_t::operator()
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::keyFunc_t::operator()( const TStringId& a, const TStringId& b ) const
{
	return a.id < b.id;
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId()
	: id( INVALID_INDEX )
{
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId( const char* pString )
	: id( GetStringPool().FindOrAdd( pString, S_Strlen( pString ) ) )
{
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId( const char* pString, uint64 length )
	: id( GetStringPool().FindOrAdd( pString, length ) )
{
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId( const eastl::string& string )
	: id( GetStringPool().FindOrAdd( string.c_str(), string.size() ) )
{
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId( const eastl::string_view& string )
	: id( GetStringPool().FindOrAdd( string.data(), string.size() ) )
{
}

/*
==================
TStringId::TStringId
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>::TStringId( const TStringId& copy )
	: id( copy.id )
{
}

/*
==================
TStringId::Make
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool> TStringId<TIdType, GetStringPool>::Make( const char* pString )
{
	return TStringId( pString );
}

/*
==================
TStringId::Make
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool> TStringId<TIdType, GetStringPool>::Make( const char* pString, uint64 length )
{
	return TStringId( pString, length );
}

/*
==================
TStringId::Make
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool> TStringId<TIdType, GetStringPool>::Make( const eastl::string& string )
{
	return TStringId( string );
}

/*
==================
TStringId::Make
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool> TStringId<TIdType, GetStringPool>::Make( const eastl::string_view& string )
{
	return TStringId( string );
}

/*
==================
TStringId::Make
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool> TStringId<TIdType, GetStringPool>::Make( const TStringId& stringId )
{
	return TStringId( stringId );
}

/*
==================
TStringId::Clear
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE void TStringId<TIdType, GetStringPool>::Clear()
{
	id = INVALID_INDEX;
}

/*
==================
TStringId::ToString
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE const char* TStringId<TIdType, GetStringPool>::ToString() const
{
	return !IsEmpty() ? GetStringPool().GetString( id ) : "";
}

/*
==================
TStringId::IsEmpty
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::IsEmpty() const
{
	return id == INVALID_INDEX;
}

/*
==================
TStringId::GetHash
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE hash_t TStringId<TIdType, GetStringPool>::GetHash() const
{
	return FastHash( *this );
}

/*
==================
TStringId::operator==
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator==( const char* pString ) const
{
	return GetStringPool().Find( pString, S_Strlen( pString ) ) == id;
}

/*
==================
TStringId::operator==
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator==( const eastl::string& string ) const
{
	return operator==( string.c_str() );
}

/*
==================
TStringId::operator==
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator==( const eastl::string_view& string ) const
{
	return GetStringPool().Find( string.data(), string.size() ) == id;
}

/*
==================
TStringId::operator==
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator==( const TStringId& stringId ) const
{
	id == stringId.id;
}

/*
==================
TStringId::operator!=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator!=( const char* pString ) const
{
	return GetStringPool().Find( pString, S_Strlen( pString ) ) != id;
}

/*
==================
TStringId::operator!=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator!=( const eastl::string& string ) const
{
	return operator!=( string.c_str() );
}

/*
==================
TStringId::operator!=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator!=( const eastl::string_view& string ) const
{
	return GetStringPool().Find( string.data(), string.size() ) != id;
}

/*
==================
TStringId::operator!=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE bool TStringId<TIdType, GetStringPool>::operator!=( const TStringId& stringId ) const
{
	id != stringId.id;
}

/*
==================
TStringId::operator=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>& TStringId<TIdType, GetStringPool>::operator=( const char* pString )
{
	id = GetStringPool().FindOrAdd( pString, S_Strlen( pString ) );
	return *this;
}

/*
==================
TStringId::operator=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>& TStringId<TIdType, GetStringPool>::operator=( const eastl::string& string )
{
	id = GetStringPool().FindOrAdd( string.c_str(), string.size() );
	return *this;
}

/*
==================
TStringId::operator=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>& TStringId<TIdType, GetStringPool>::operator=( const eastl::string_view& string )
{
	id = GetStringPool().FindOrAdd( string.data(), string.size() );
	return *this;
}

/*
==================
TStringId::operator=
==================
*/
template<typename TIdType, TStringPool<TIdType>& ( *GetStringPool )()>
FORCEINLINE TStringId<TIdType, GetStringPool>& TStringId<TIdType, GetStringPool>::operator=( const TStringId& stringId )
{
	id = stringId.id;
	return *this;
}
