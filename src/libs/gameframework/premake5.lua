project "gameframework"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/gameframework/**.h",
        "../../public/libs/gameframework/**.inl",
    }

    -- Enable PCH file
    pchheader       "pch_gameframework.h"
    pchsource       "pch_gameframework.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/libs/gameframework/**.h", "../../public/libs/gameframework/**.inl" }
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )