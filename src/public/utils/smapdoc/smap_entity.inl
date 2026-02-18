#pragma once

/*
==================
CSMAPEntity::Copy
==================
*/
FORCEINLINE void CSMAPEntity::Copy( const CSMAPEntity& other )
{
	classname = other.classname;
	name	  = other.name;
}

/*
==================
CSMAPEntity::SetClassName
==================
*/
FORCEINLINE void CSMAPEntity::SetClassName( const char* pClassName )
{
	classname = pClassName;
}

/*
==================
CSMAPEntity::SetName
==================
*/
FORCEINLINE void CSMAPEntity::SetName( const char* pName )
{
	name = pName;
}

/*
==================
CSMAPEntity::GetClassName
==================
*/
FORCEINLINE const char* CSMAPEntity::GetClassName() const
{
	return classname.c_str();
}

/*
==================
CSMAPEntity::GetName
==================
*/
FORCEINLINE const char* CSMAPEntity::GetName() const
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
	classname = "";
	name	  = "";
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
