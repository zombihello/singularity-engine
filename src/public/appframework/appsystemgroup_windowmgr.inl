#pragma once

/*
==================
CAppSystemGroupWindowMgr::Create
==================
*/
FORCEINLINE bool CAppSystemGroupWindowMgr::Create()
{
	AddSystem( CreateWindowMgr(), WINDOWMGR_INTERFACE_VERSION );
	return true;
}

/*
==================
CAppSystemGroupWindowMgr::GetName
==================
*/
FORCEINLINE const char* CAppSystemGroupWindowMgr::GetName() const
{
	return "WindowMgr";
}
