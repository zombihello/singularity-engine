#include "pch_tier0.h"
#include <psapi.h>

/*
==================
Mem_SystemMemoryInfo
==================
*/
systemMemoryInfo_t Mem_SystemMemoryInfo()
{
	systemMemoryInfo_t result = {};
	MEMORYSTATUSEX	   systemMemoryInfo;
	systemMemoryInfo.dwLength = sizeof( MEMORYSTATUSEX );
	if ( GlobalMemoryStatusEx( &systemMemoryInfo ) )
	{
		result.totalSize = systemMemoryInfo.ullTotalPhys;
		result.freeSize	 = systemMemoryInfo.ullAvailPhys;
		result.usedSize	 = result.totalSize - result.freeSize;
	}
	else
	{
		AssertMsg( false, "GlobalMemoryStatusEx is failed (GetLastError 0x%X)", GetLastError() );
	}

	return result;
}

/*
==================
Mem_SystemMemoryInfo
==================
*/
appMemoryInfo_t Mem_AppMemoryInfo()
{
	appMemoryInfo_t			result = {};
	PROCESS_MEMORY_COUNTERS processMemoryInfo;
	if ( GetProcessMemoryInfo( GetCurrentProcess(), &processMemoryInfo, sizeof( PROCESS_MEMORY_COUNTERS ) ) )
	{
		result.usedSize = processMemoryInfo.WorkingSetSize;
	}
	else
	{
		AssertMsg( false, "GetProcessMemoryInfo is failed (GetLastError 0x%X)", GetLastError() );
	}

	return result;
}
