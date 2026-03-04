#pragma once

/*
==================
CAppSystemGroupResourceTools::Create
==================
*/
FORCEINLINE bool CAppSystemGroupResourceTools::Create()
{
	appSystemInfo_t appSystemInfos[] = {
		{ "texture_tool" DLL_EXT_STRING, TEXTURE_TOOL_INTERFACE_VERSION },
		{ "material_tool" DLL_EXT_STRING, MATERIAL_TOOL_INTERFACE_VERSION },
		{ "model_tool" DLL_EXT_STRING, MODEL_TOOL_INTERFACE_VERSION },
		{ "entitydesc_tool" DLL_EXT_STRING, ENTITYDESC_TOOL_INTERFACE_VERSION },
		{ "map_tool" DLL_EXT_STRING, MAP_TOOL_INTERFACE_VERSION },
		{ "", "" }	// Required to terminate the list
	};
	return AddSystems( appSystemInfos );
}

/*
==================
CAppSystemGroupResourceTools::GetName
==================
*/
FORCEINLINE const char* CAppSystemGroupResourceTools::GetName() const
{
	return "Resource Tools";
}
