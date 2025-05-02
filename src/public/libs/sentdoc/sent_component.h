#ifndef SENT_COMPONENT_H
#define SENT_COMPONENT_H

#include <vector>

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

	void SetType( const achar* pType );
	void AddVar( const CSENTEntityDescVar& var );
	void RemoveVar( uint32 index );
	void Clear();

	const achar* GetType() const;
	uint32 GetNumVars() const;
	const std::vector<CSENTEntityDescVar>& GetVars() const;

	CSENTEntityDescComponent& operator=( const CSENTEntityDescComponent& right );

private:
	std::string							type;
	std::vector<CSENTEntityDescVar>		vars;
};

#include "sentdoc/sent_component.inl"

#endif // !SENT_COMPONENT_H
