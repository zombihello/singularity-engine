project "studioapi_vk"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	
    ----------- PROJECT SETTINGS --------

    defines     { "STUDIOAPI_VK" }

    files       {
        -- Shared studioapi code
        "../*.inl", 
        "../*.cpp",
        "../*.h",

        -- DirectX 11 code
		"**.inl", 
        "**.cpp",
        "**.h",
        "**.rc",

        -- Public interfaces and shared code
        "../../../public/studiorender/studioapi/**.h",
        "../../../public/studiorender/studioapi/**.inl",
        "../../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_studioapi.h"
    pchsource       "../pch_studioapi.cpp"
    includedirs     { "./", "../" }

    vpaths      {
        ["src/*"]       = { "../*.h", "../*.inl", "../*.cpp" },
        ["src/vk/*"]	= { "**.h", "**.inl", "**.cpp", "**.rc" },
		["public/*"]    = { "../../../public/**.h", "../../../public/**.inl", "../../../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces",
		"shadercache",
		"pixelformatinfos"
    }

	defines      {
        "VK_NO_PROTOTYPES"
    }

	----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.Volk )
	
	---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
    filter "platforms:not Win64"
        excludes { "**/windows/**.*" }
    filter {}