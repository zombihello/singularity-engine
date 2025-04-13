project "gameinfo"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/gameinfo/**.h",
        "../../public/libs/gameinfo/**.inl",
    }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.RapidJSON )