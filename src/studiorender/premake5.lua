project "studiorender"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	
    ----------- PROJECT SETTINGS --------

    files       {
		"**.inl", 
        "**.cpp",
        "**.h",
        "../public/studiorender/**.h",
        "../public/studiorender/**.inl",
        "../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_studiorender.h"
    pchsource       "pch_studiorender.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces",
		"shadercache"
    }

	dependson   {
		"studioapi_vk"
    }

	----------- LINK THIRD PARTIES -----------------

    GLM.Link()

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        files       { "**.rc" }
        vpaths      { ["src/*"] = { "**.rc" } }
    filter {}

    ---------- EXCLUDES SUBPROJECT'S FILES ---------

    excludes        { 
        "studioapi/**", 
        "../public/studiorender/studioapi/**" 
    }