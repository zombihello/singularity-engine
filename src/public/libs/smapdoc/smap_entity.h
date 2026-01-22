#pragma once
#include <EASTL/string.h>
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

	void		Copy( const CSMAPEntity& other );
	void		SetEntityDesc( const char* pEntityDesc );
	void		SetName( const char* pName );
	const char* GetEntityDesc() const;
	const char* GetName() const;
	void		Clear();

	CSMAPEntity& operator=( const CSMAPEntity& right );

private:
	eastl::string entityDesc;
	eastl::string name;
};

#include "smapdoc/smap_entity.inl"
