#pragma once
#include <EASTL/string.h>
#include "tier0/tier0.h"

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
	void		SetClassName( const char* pClassName );
	void		SetName( const char* pName );
	const char* GetClassName() const;
	const char* GetName() const;
	void		Clear();

	CSMAPEntity& operator=( const CSMAPEntity& right );

private:
	eastl::string classname;
	eastl::string name;
};

#include "utils/smapdoc/smap_entity.inl"
