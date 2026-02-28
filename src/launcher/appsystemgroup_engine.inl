#pragma once

/*
==================
CAppSystemGroupEngine::Create
==================
*/
FORCEINLINE bool CAppSystemGroupEngine::Create()
{
	appSystemInfo_t appSystemInfos[] = {
		{ "inputsystem" DLL_EXT_STRING, INPUTSYSTEM_INTERFACE_VERSION },
		{ "studioapi_vk" DLL_EXT_STRING, STUDIOAPI_INTERFACE_VERSION },
		{ "studiorender" DLL_EXT_STRING, STUDIORENDER_INTERFACE_VERSION },
		{ "resourcesystem" DLL_EXT_STRING, RESOURCESYSTEM_INTERFACE_VERSION },
		{ "materialsystem" DLL_EXT_STRING, MATERIALSYSTEM_INTERFACE_VERSION },
		{ "", "" }	// Required to terminate the list
	};
	return AddSystems( appSystemInfos );
}

/*
==================
CAppSystemGroupEngine::GetName
==================
*/
FORCEINLINE const char* CAppSystemGroupEngine::GetName() const
{
	return "Engine";
}
