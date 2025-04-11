/**
 * @file
 * @addtogroup studioapi studioapi
 */

// STL
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

// Interfaces
#include "interfaces/interfaces.h"

// StdLib
#include "stdlib/stdlib.h"
#include "stdlib/convar.h"

// Core
#include "core/core.h"
#include "core/debug.h"
#include "core/profile.h"

// Studio API
#include "studiorender/studioapi/istudioapi.h"

// DirectX 11
#if defined( STUDIOAPI_DX11 )
	#include "studiorender/studioapi/dx11/pch_studioapi_dx11.h"
// Vulkan
#elif defined( STUDIOAPI_VK )
	#include "studiorender/studioapi/vk/pch_studioapi_vk.h"
#endif // STUDIOAPI_DX11 || STUDIOAPI_VK