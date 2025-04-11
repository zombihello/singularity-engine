project "sandbox"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	targetname	"game"
	targetdir( buildDir .. "sandbox/" .. binariesDir .. outputDir )
	
    ----------- PROJECT SETTINGS --------

	defines { "SANDBOX_GAME_DLL" }

    files       {
		-- Shared game code
		"../*.inl", 
        "../*.cpp",
        "../*.h",
		
		-- Sandbox game code
		"**.inl", 
        "**.cpp",
        "**.h",
		
		-- Public interfaces and shared code
        "../../public/game/**.h",
        "../../public/game/**.inl",
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_game.h"
    pchsource       "../pch_game.cpp"
    includedirs     { "./", "../" }

    vpaths      {
        ["src/*"]           = { "../*.h", "../*.inl", "../*.cpp" },
        ["src/sandbox/*"]	= { "**.h", "**.inl", "**.cpp" },
		["public/*"]        = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces"
    }

	dependson   {
        "inputsystem",
        "filesystem",
		"engine",
        "studiorender",
        "materialsystem",
		"resourcesystem"
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
        files   { "**.rc" }
        vpaths  { ["src/sandbox/*"] = { "**.rc" } }
    filter {}