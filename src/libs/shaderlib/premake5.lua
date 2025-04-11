project "shaderlib"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/shaderlib/**.h",
        "../../public/libs/shaderlib/**.inl",
    }

    -- Enable PCH file
    pchheader       "pch_shaderlib.h"
    pchsource       "pch_shaderlib.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }
	
    ----------- LINK THIRD PARTIES -----------------

    GLM.Link()
    
	---------- PLATFORM SPECIFIC SETTINGS ---------
	
	-- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}