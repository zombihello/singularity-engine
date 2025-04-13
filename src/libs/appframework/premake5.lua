project "appframework"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/appframework/**.h",
        "../../public/libs/appframework/**.inl",
    }

    -- Enable PCH file
    pchheader       "pch_appframework.h"
    pchsource       "pch_appframework.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/libs/appframework/**.h", "../../public/libs/appframework/**.inl" }
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.SDL2 )

    ---------- PLATFORM SPECIFIC SETTINGS ---------

    -- Exclude platform specific for other platforms
    filter "platforms:not Win64"
        excludes { "**/platforms/sdl/**.*" }
    filter {}