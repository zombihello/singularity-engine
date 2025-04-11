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

    GLM.Link()
    RapidJson.Link()