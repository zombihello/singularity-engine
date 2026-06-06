#pragma once

/*
==================
CAppSystemGroupTier1::Create
==================
*/
FORCEINLINE bool CAppSystemGroupTier1::Create()
{
	appSystemInfo_t appSystemInfos[] = {
		{ "cvar" DLL_EXT_STRING, CVAR_QUERY_INTERFACE_VERSION },  // This one must be first
		{ "filesystem" DLL_EXT_STRING, FILESYSTEM_INTERFACE_VERSION },
		{ "cvar" DLL_EXT_STRING, CVAR_INTERFACE_VERSION },
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
