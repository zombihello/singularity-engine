project "stdlib"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/stdlib/**.h",
        "../../public/libs/stdlib/**.inl",
    }

    -- Enable PCH file
    pchheader       "pch_stdlib.h"
    pchsource       "pch_stdlib.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }
	
    ----------- LINK THIRD PARTIES -----------------

    GLM.Link()
	RapidJson.Link()

	---------- PLATFORM SPECIFIC SETTINGS ---------
	
	-- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}