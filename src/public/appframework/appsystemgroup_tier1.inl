#pragma once

/*
==================
CAppSystemGroupTier1::Create
==================
*/
FORCEINLINE bool CAppSystemGroupTier1::Create()
{
	appSystemInfo_t appSystemInfos[] = {
		{ "filesystem" DLL_EXT_STRING, FILESYSTEM_INTERFACE_VERSION },
		{ "cvar" DLL_EXT_STRING, CMDSYSTEM_INTERFACE_VERSION },
		{ "cvar" DLL_EXT_STRING, CVARSYSTEM_INTERFACE_VERSION },
		{ "", "" }	// Required to terminate the list
	};
	return AddSystems( appSystemInfos );
}

/*
==================
CAppSystemGroupTier1::GetName
==================
*/
FORCEINLINE const char* CAppSystemGroupTier1::GetName() const
{
	return "Tier1";
}
