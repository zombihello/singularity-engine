project "sandbox_launcher"
    kind        "WindowedApp"
    language    "C++"
    targetname	"sandbox"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    files       {
        "**.h", 
        "**.inl", 
        "**.cpp",

        "../../public/launcher/**.h",
        "../../public/core/**.cpp"
    }
    includedirs     { "./" }

    vpaths      {
        ["src/*"]           = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]        = { "../../public/**.cpp" }
    }

    links {
        "core",
        "stdlib"
    }

    dependson   {
        "copy_thirdparty_files",
        "launcher",
		"sandbox"
    }

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