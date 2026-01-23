#include "utils/sentdoc/sent_component.h"

/*
==================
CSENTEntityDescComponent::CSENTEntityDescComponent
==================
*/
CSENTEntityDescComponent::CSENTEntityDescComponent()
{
}

/*
==================
CSENTEntityDescComponent::CSENTEntityDescComponent
==================
*/
CSENTEntityDescComponent::CSENTEntityDescComponent( const CSENTEntityDescComponent& other )
	: type( other.type )
{
	Copy( other );
}

/*
==================
CSENTEntityDescComponent::~CSENTEntityDescComponent
==================
*/
CSENTEntityDescComponent::~CSENTEntityDescComponent()
{
}
