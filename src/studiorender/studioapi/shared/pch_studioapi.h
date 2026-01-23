#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/set.h>
#include <EASTL/unordered_map.h>

#include "utils/interfaces/interfaces.h"
#include "stdlib/stdlib.h"
#include "stdlib/convar.h"
#include "core/core.h"
#include "core/debug.h"
#include "core/profile.h"
#include "studiorender/studioapi/istudioapi.h"

#if defined( STUDIOAPI_DX11 )
	#include "studiorender/studioapi/dx11/pch_studioapi_dx11.h"
#elif defined( STUDIOAPI_VK )
	#include "studiorender/studioapi/vk/pch_studioapi_vk.h"
#endif	// STUDIOAPI_DX11 || STUDIOAPI_VK
