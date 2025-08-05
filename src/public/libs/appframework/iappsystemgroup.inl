#pragma once

/*
==================
CApplication::PostInit
==================
*/
FORCEINLINE bool CAppSystemGroup::PostInit()
{
	return true;
}

/*
==================
CApplication::PreShutdown
==================
*/
FORCEINLINE void CAppSystemGroup::PreShutdown()
{}

/*
==================
CApplication::Setup
==================
*/
FORCEINLINE void CAppSystemGroup::Setup( CAppSystemGroup* pParentAppSystem )
{
	pParentAppSystemGroup = pParentAppSystem;
}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::Create
==================
*/
template<class CBaseClass>
FORCEINLINE bool CDefaultAppSystemGroup<CBaseClass>::Create()
{
	return true;
}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::PreInit
==================
*/
template<class CBaseClass>
FORCEINLINE bool CDefaultAppSystemGroup<CBaseClass>::PreInit()
{
	return true;
}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::PostInit
==================
*/
template<class CBaseClass>
FORCEINLINE bool CDefaultAppSystemGroup<CBaseClass>::PostInit()
{
	return true;
}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::PreShutdown
==================
*/
template<class CBaseClass>
FORCEINLINE void CDefaultAppSystemGroup<CBaseClass>::PreShutdown()
{}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::PostShutdown
==================
*/
template<class CBaseClass>
FORCEINLINE void CDefaultAppSystemGroup<CBaseClass>::PostShutdown()
{}

/*
==================
CDefaultAppSystemGroup<CBaseClass>::Destroy
==================
*/
template<class CBaseClass>
FORCEINLINE void CDefaultAppSystemGroup<CBaseClass>::Destroy()
{}