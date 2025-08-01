#pragma once

/*
==================
TConCmdBase::IsCommand
==================
*/
template<class TBaseClass>
bool TConCmdBase<TBaseClass>::IsCommand() const
{
	Assert( false );
	return false;
}

/*
==================
TConCmdBase::IsFlagSet
==================
*/
template<class TBaseClass>
bool TConCmdBase<TBaseClass>::IsFlagSet( uint32 flag ) const
{
	return ( flags & flag ) ? true : false;
}

/*
==================
TConCmdBase::GetFlags
==================
*/
template<class TBaseClass>
uint32 TConCmdBase<TBaseClass>::GetFlags() const
{
	return flags;
}

/*
==================
TConCmdBase::AddFlags
==================
*/
template<class TBaseClass>
void TConCmdBase<TBaseClass>::AddFlags( uint32 flags )
{
	TConCmdBase::flags |= flags;
}

/*
==================
TConCmdBase::GetName
==================
*/
template<class TBaseClass>
const achar* TConCmdBase<TBaseClass>::GetName() const
{
	return pName;
}

/*
==================
TConCmdBase::SetHelpText
==================
*/
template<class TBaseClass>
void TConCmdBase<TBaseClass>::SetHelpText( const achar* pHelpText )
{
	TConCmdBase::pHelpText = pHelpText ? pHelpText : "";
}

/*
==================
TConCmdBase::GetHelpText
==================
*/
template<class TBaseClass>
const achar* TConCmdBase<TBaseClass>::GetHelpText() const
{
	return pHelpText;
}

/*
==================
TConCmdBase::SetNext
==================
*/
template<class TBaseClass>
void TConCmdBase<TBaseClass>::SetNext( IConCmdBase* pNext )
{
	TConCmdBase::pNext = pNext;
}

/*
==================
TConCmdBase::GetNext
==================
*/
template<class TBaseClass>
IConCmdBase* TConCmdBase<TBaseClass>::GetNext() const
{
	return pNext;
}

/*
==================
TConCmdBase::SetRegistered
==================
*/
template<class TBaseClass>
void TConCmdBase<TBaseClass>::SetRegistered( bool bRegistered )
{
	TConCmdBase::bRegistered = bRegistered;
}

/*
==================
TConCmdBase::IsRegistered
==================
*/
template<class TBaseClass>
bool TConCmdBase<TBaseClass>::IsRegistered() const
{
	return bRegistered;
}

/*
==================
TConCmdBase::GetDLLIdentifier
==================
*/
template<class TBaseClass>
cvarDLLIdentifier_t TConCmdBase<TBaseClass>::GetDLLIdentifier() const
{
	return CCvarLocalRegister::s_dllIdentifier;
}


/*
==================
CConVarRef::operator=
==================
*/
FORCEINLINE CConVarRef& CConVarRef::operator=( IConVar* pConVar )
{
	CConVarRef::pConVar = pConVar;
	return *this;
}

/*
==================
CConVarRef::operator=
==================
*/
FORCEINLINE CConVarRef& CConVarRef::operator=( const CConVarRef& copy )
{
	pConVar = copy.pConVar;
	return *this;
}

/*
==================
CConVarRef::operator==
==================
*/
FORCEINLINE bool CConVarRef::operator==( const CConVarRef& right ) const
{
	return pConVar == right.pConVar;
}

/*
==================
CConVarRef::operator==
==================
*/
FORCEINLINE bool CConVarRef::operator==( IConVar* pRight ) const
{
	return pConVar == pRight;
}

/*
==================
CConVarRef::operator!=
==================
*/
FORCEINLINE bool CConVarRef::operator!=( const CConVarRef& right ) const
{
	return pConVar != right.pConVar;
}

/*
==================
CConVarRef::operator!=
==================
*/
FORCEINLINE bool CConVarRef::operator!=( IConVar* pRight ) const
{
	return pConVar != pRight;
}

/*
==================
CConVarRef::IsValid
==================
*/
FORCEINLINE bool CConVarRef::IsValid() const
{
	return !!pConVar;
}

/*
==================
CConVarRef::operator bool
==================
*/
FORCEINLINE CConVarRef::operator bool() const
{
	return IsValid();
}

/*
==================
CConVarRef::operator IConVar*
==================
*/
FORCEINLINE CConVarRef::operator IConVar*() const
{
	return pConVar;
}

/*
==================
CConVarRef::operator IConVar*&
==================
*/
FORCEINLINE CConVarRef::operator IConVar*&()
{
	return pConVar;
}

/*
==================
CConVarRef::operator->
==================
*/
FORCEINLINE IConVar* CConVarRef::operator->() const
{
	Assert( pConVar );
	return pConVar;
}