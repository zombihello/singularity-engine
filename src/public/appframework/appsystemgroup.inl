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
TDefaultAppSystemGroup::Create
==================
*/
template<class TBaseClass>
FORCEINLINE bool TDefaultAppSystemGroup<TBaseClass>::Create()
{
	return true;
}

/*
==================
TDefaultAppSystemGroup::Destroy
==================
*/
template<class TBaseClass>
FORCEINLINE void TDefaultAppSystemGroup<TBaseClass>::Destroy()
{
}
