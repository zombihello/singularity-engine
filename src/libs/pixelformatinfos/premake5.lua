project "pixelformatinfos"
    kind        "StaticLib"
    language    "C++"
    location( intermediateDir )

    ----------- PROJECT SETTINGS --------

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../../public/libs/pixelformatinfos/**.h",
        "../../public/libs/pixelformatinfos/**.inl",
    }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../../public/**.h", "../../public/**.inl", "../../public/**.cpp" }
    }
	
	----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )