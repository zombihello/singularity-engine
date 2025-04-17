project "parserlib"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/parserlib/**.h",
        "../../public/libs/parserlib/**.inl",
    }

    -- Enable PCH file
    pchheader       "pch_parserlib.h"
    pchsource       "pch_parserlib.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/libs/parserlib/**.h", "../../public/libs/parserlib/**.inl" }
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
    filter "platforms:not Win64"
        excludes { "**/platforms/sdl/**.*" }
    filter {}