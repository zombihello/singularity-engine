#pragma once
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>

#include "tier1/stl.h"
#include "utils/sentdoc/sentvar.h"

//-----------------------------------------------------------------------------
// SENT entity descriptor component
//-----------------------------------------------------------------------------
class CSENTEntityDescComponent
{
public:
	CSENTEntityDescComponent();
	CSENTEntityDescComponent( const CSENTEntityDescComponent& other );
	~CSENTEntityDescComponent();

	void Copy( const CSENTEntityDescComponent& other );

	void SetType( const char* pType );
	void AddVar( const CSENTEntityDescVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	const char*								 GetType() const;
	uint32									 GetNumVars() const;
	const eastl::vector<CSENTEntityDescVar>& GetVars() const;
	const CSENTEntityDescVar&				 GetVar( uint32 index ) const;
	CSENTEntityDescVar*						 GetVar( const char* pName ) const;

	CSENTEntityDescComponent& operator=( const CSENTEntityDescComponent& right );

private:
	typedef eastl::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> varsDict_t;

	eastl::string					  type;
	eastl::vector<CSENTEntityDescVar> vars;
	varsDict_t						  varsDict;
};

#include "utils/sentdoc/sent_component.inl"
