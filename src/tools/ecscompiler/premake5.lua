project "ecscompiler"
    kind        "ConsoleApp"
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",
        "**.flex",
        "**.bison",
        "../../public/core/**.cpp"
    }
	excludes { "test/**.*" }

    includedirs     {
        flexOutputDir,
        bisonOutputDir,
        "./"
     }

    rules       { "flex", "bison" }
    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp", "**.flex", "**.bison" },
        ["public/*"]        = { "../../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
        "appframework",
        "interfaces",
        "parserlib"
    }

    dependson   {
        "filesystem",
		"engine"
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )

	---------- PLATFORM SPECIFIC SETTINGS ---------
	
	-- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        files       { "**.rc" }
        vpaths      { ["src/*"] = { "**.rc" } }
    filter {}