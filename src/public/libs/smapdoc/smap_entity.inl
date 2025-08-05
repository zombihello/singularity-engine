#pragma once

/*
==================
CSMAPEntity::Copy
==================
*/
FORCEINLINE void CSMAPEntity::Copy( const CSMAPEntity& other )
{
	entityDesc = other.entityDesc;
	name = other.name;
}

/*
==================
CSMAPEntity::SetEntityDesc
==================
*/
FORCEINLINE void CSMAPEntity::SetEntityDesc( const achar* pEntityDesc )
{
	entityDesc = pEntityDesc;
}

/*
==================
CSMAPEntity::SetName
==================
*/
FORCEINLINE void CSMAPEntity::SetName( const achar* pName )
{
	name = pName;
}

/*
==================
CSMAPEntity::GetEntityDesc
==================
*/
FORCEINLINE const achar* CSMAPEntity::GetEntityDesc() const
{
	return entityDesc.c_str();
}

/*
==================
CSMAPEntity::GetName
==================
*/
FORCEINLINE const achar* CSMAPEntity::GetName() const
{
	return name.c_str();
}

/*
==================
CSMAPEntity::Clear
==================
*/
FORCEINLINE void CSMAPEntity::Clear()
{
	entityDesc = "";
	name = "";
}

/*
==================
CSMAPEntity::operator=
==================
*/
FORCEINLINE CSMAPEntity& CSMAPEntity::operator=( const CSMAPEntity& right )
{
	Copy( right );
	return *this;
}