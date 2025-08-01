#pragma once
#include <string>
#include "core/core.h"

//-----------------------------------------------------------------------------
// SMAP entity
//-----------------------------------------------------------------------------
class CSMAPEntity
{
public:
	CSMAPEntity();
	CSMAPEntity( const CSMAPEntity& other );
	~CSMAPEntity();

	void Copy( const CSMAPEntity& other );
	void SetEntityDesc( const achar* pEntityDesc );
	void SetName( const achar* pName );
	const achar* GetEntityDesc() const;
	const achar* GetName() const;
	void Clear();

	CSMAPEntity& operator=( const CSMAPEntity& right );

private:
	std::string		entityDesc;
	std::string		name;
};

#include "smapdoc/smap_entity.inl"