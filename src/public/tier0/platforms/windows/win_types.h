#pragma once

//-----------------------------------------------------------------------------
// Platform specific types
//-----------------------------------------------------------------------------
typedef signed char int8;
typedef short		int16;
typedef int			int32;
typedef long long	int64;

typedef unsigned char	   uint8;
typedef unsigned short	   uint16;
typedef unsigned int	   uint32;
typedef unsigned long long uint64;

typedef HMODULE	  dllHandle_t;
typedef HANDLE	  procHandle_t;
typedef HANDLE	  windowHandle_t;
typedef HINSTANCE appInstanceHandle_t;
typedef HANDLE	  threadHandle_t;
typedef DWORD	  threadId_t;
