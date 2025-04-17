project "sandbox"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	targetname	"game"
	targetdir( buildDir .. "sandbox/" .. outputBinDirSuffix )
	
    ----------- PROJECT SETTINGS --------

    files       {
		-- Sandbox game code
		"**.inl", 
        "**.cpp",
        "**.h",
		"**.ecs",

		-- Public interfaces and shared code
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_sandbox.h"
    pchsource       "pch_sandbox.cpp"
    includedirs     { "./", intermediateDir .. "/generated/" }

    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp", "**.ecs" },
		["public/*"]        = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }

    -- Build commands for specific files
    filter { "files:**.ecs" }
        buildaction "None" 
    filter {}

    links       {
        "core",
        "stdlib",
		"interfaces",
        "gameframework"
    }

	----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.Flecs )

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