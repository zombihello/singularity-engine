project "resourcesystem"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../public/resourcesystem/**.h",
        "../public/resourcesystem/**.inl",
        "../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_resourcesystem.h"
    pchsource       "pch_resourcesystem.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces",
		"smatdoc",
		"stexdoc"
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        files   { "**.rc" }
        vpaths  { ["src/*"] = { "**.rc" } }
    filter {}