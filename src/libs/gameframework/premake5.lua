project "gameframework"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "**.ecs",

        "../../public/libs/gameframework/**.h",
        "../../public/libs/gameframework/**.inl",
        "../../public/libs/gameframework/**.ecs",
    }

    -- Enable PCH file
    pchheader       "pch_gameframework.h"
    pchsource       "pch_gameframework.cpp"
    includedirs     { "./", intermediateDir .. "/generated/" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp", "**.ecs" },
        ["public/*"]    = { "../../public/libs/gameframework/**.h", "../../public/libs/gameframework/**.inl", "../../public/libs/gameframework/**.ecs" }
    }

    -- Build commands for specific files
    filter { "files:**.ecs" }
        buildaction "None" 
    filter {}

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.Flecs )