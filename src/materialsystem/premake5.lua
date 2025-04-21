project "materialsystem"
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
        "**.hlsl",
		"shaderlist.makefile",
        "../public/materialsystem/**.h",
        "../public/materialsystem/**.inl",
		"../public/materialsystem/**.hlsl",
        "../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_materialsystem.h"
    pchsource       "pch_materialsystem.cpp"
    includedirs     { "./", shadersGeneratedDir }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp", "shaderlist.makefile", "**.hlsl" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

     -- Build commands for specific files
    filter { "files:shaderlist.makefile" }
        buildaction "None" 
    filter { "files:**.hlsl" }
        buildaction "None" 
    filter {}

    links       {
        "core",
        "stdlib",
		"interfaces",
		"shaderlib",
		"shadercache",
		"smatdoc",
		"stexdoc"
    }

    dependson   { 
        "stdshaders",
        "shadercompile",
		"studiorender"
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
        files   { "**.rc" }
        vpaths  { ["src/*"] = { "**.rc" } }
    filter {}

    ---------- EXCLUDES SUBPROJECT'S FILES ---------

    excludes { "stdshaders/**" }