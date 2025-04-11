project "resourcecompiler"
    kind        "ConsoleApp"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_resourcecompiler.h"
    pchsource       "pch_resourcecompiler.cpp"
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
		"smatdoc",
		"stexdoc",
        "smdldoc"
    }

    dependson   {
        "filesystem",
		"engine",
		"texture_tool",
		"material_tool",
        "model_tool"
    }

    ----------- LINK THIRD PARTIES -----------------

    GLM.Link()
    RapidJson.Link()

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