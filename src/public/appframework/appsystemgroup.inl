#pragma once

/*
==================
CAppSystemGroup::CAppSystemGroup
==================
*/
FORCEINLINE CAppSystemGroup::CAppSystemGroup()
	: state( APPSYSTEM_GROUP_STATE_NONE )
{
}

/*
==================
CAppSystemGroup::~CAppSystemGroup
==================
*/
FORCEINLINE CAppSystemGroup::~CAppSystemGroup()
{
	Shutdown();
}

/*
==================
CAppSystemGroup::GetState
==================
*/
FORCEINLINE appSystemGroupState_t CAppSystemGroup::GetState() const
{
	return state;
}

/*
==================
CDefaultAppSystemGroup::Create
==================
*/
template<class TBaseClass>
FORCEINLINE bool CDefaultAppSystemGroup<TBaseClass>::Create()
{
	return true;
}

/*
==================
CDefaultAppSystemGroup::Destroy
==================
*/
template<class TBaseClass>
FORCEINLINE void CDefaultAppSystemGroup<TBaseClass>::Destroy()
{
}
