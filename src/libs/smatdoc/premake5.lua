project "smatdoc"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/smatdoc/**.h",
        "../../public/libs/smatdoc/**.inl",
    }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.RapidJSON )