project "shadercompiler"
    kind        "ConsoleApp"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/tools/shadercompiler/**.h",
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_shadercompiler.h"
    pchsource       "pch_shadercompiler.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]        = { "../../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
        "appframework",
        "interfaces",
		"shadercache"
    }

    dependson   {
        "filesystem",
		"cvar",
		"shadercompiler_vk"
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.RapidJSON )

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

	excludes { "vk/**" }