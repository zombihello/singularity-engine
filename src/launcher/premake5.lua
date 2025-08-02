project "launcher"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",

        "../public/launcher/**.h",
        "../public/core/**.cpp"
    }
    includedirs     { "./" }

    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]        = { "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
        "appframework",
        "interfaces",
		"gameinfo"
    }

    dependson   {
        "inputsystem",
        "filesystem",
		"cvar",
        "studiorender",
        "materialsystem",
		"resourcesystem",
		"stdshaders"
    }

    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.GLM )
	ThirdParty.Link( ThirdParty.Libs.RapidJSON )

	---------- PLATFORM SPECIFIC SETTINGS ---------
	
	-- Exclude platform specific for other platforms
	filter "platforms:not Win64"
        excludes { "**/platforms/windows/**.*" }
    filter {}

    -- Windows
    filter "platforms:Win64"
        files   { "**.rc" }
        vpaths  { ["src/*"] = { "**.rc" } }
    filter {}