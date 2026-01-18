#pragma once
#include <vector>
#include <unordered_map>

#include "stdlib/stl.h"
#include "sentdoc/sentvar.h"

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

	const char*						   GetType() const;
	uint32								   GetNumVars() const;
	const std::vector<CSENTEntityDescVar>& GetVars() const;
	const CSENTEntityDescVar&			   GetVar( uint32 index ) const;
	CSENTEntityDescVar*					   GetVar( const char* pName ) const;

	CSENTEntityDescComponent& operator=( const CSENTEntityDescComponent& right );

private:
	typedef std::unordered_map<const char*, uint32, stlInsensitiveStringHash_t, stlInsensitiveCompareString_t> varsDict_t;

	std::string						type;
	std::vector<CSENTEntityDescVar> vars;
	varsDict_t						varsDict;
};

#include "sentdoc/sent_component.inl"
