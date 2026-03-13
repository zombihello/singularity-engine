#pragma once

/*
==================
CConCmdBase::IsCommand
==================
*/
template<class TBaseClass>
bool CConCmdBase<TBaseClass>::IsCommand() const
{
	Assert( false );
	return false;
}

/*
==================
CConCmdBase::IsFlagSet
==================
*/
template<class TBaseClass>
bool CConCmdBase<TBaseClass>::IsFlagSet( uint32 flag ) const
{
	return ( flags & flag ) ? true : false;
}

/*
==================
CConCmdBase::GetFlags
==================
*/
template<class TBaseClass>
uint32 CConCmdBase<TBaseClass>::GetFlags() const
{
	return flags;
}

/*
==================
CConCmdBase::AddFlags
==================
*/
template<class TBaseClass>
void CConCmdBase<TBaseClass>::AddFlags( uint32 flags )
{
	CConCmdBase::flags |= flags;
}

/*
==================
CConCmdBase::GetName
==================
*/
template<class TBaseClass>
const char* CConCmdBase<TBaseClass>::GetName() const
{
	return pName;
}

/*
==================
CConCmdBase::SetHelpText
==================
*/
template<class TBaseClass>
void CConCmdBase<TBaseClass>::SetHelpText( const char* pHelpText )
{
	CConCmdBase::pHelpText = pHelpText ? pHelpText : "";
}

/*
==================
CConCmdBase::GetHelpText
==================
*/
template<class TBaseClass>
const char* CConCmdBase<TBaseClass>::GetHelpText() const
{
	return pHelpText;
}

/*
==================
CConCmdBase::SetNext
==================
*/
template<class TBaseClass>
void CConCmdBase<TBaseClass>::SetNext( IConCmdBase* pNext )
{
	CConCmdBase::pNext = pNext;
}

/*
==================
CConCmdBase::GetNext
==================
*/
template<class TBaseClass>
IConCmdBase* CConCmdBase<TBaseClass>::GetNext() const
{
	return pNext;
}

/*
==================
CConCmdBase::SetRegistered
==================
*/
template<class TBaseClass>
void CConCmdBase<TBaseClass>::SetRegistered( bool bRegistered )
{
	CConCmdBase::bRegistered = bRegistered;
}

/*
==================
CConCmdBase::IsRegistered
==================
*/
template<class TBaseClass>
bool CConCmdBase<TBaseClass>::IsRegistered() const
{
	return bRegistered;
}

/*
==================
CConCmdBase::GetDLLIdentifier
==================
*/
template<class TBaseClass>
cvarDLLIdentifier_t CConCmdBase<TBaseClass>::GetDLLIdentifier() const
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