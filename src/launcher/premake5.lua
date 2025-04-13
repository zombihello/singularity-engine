project "launcher"
    kind        "WindowedApp"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_launcher.h"
    pchsource       "pch_launcher.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]        = { "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
        "appframework",
        "interfaces"
    }

    dependson   {
        "copy_thirdparty_files",
        "inputsystem",
        "filesystem",
		"engine",
        "studiorender",
        "materialsystem",
		"resourcesystem",
		"stdshaders",
		game
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