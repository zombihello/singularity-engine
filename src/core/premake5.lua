project "core"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    
    language    "C++"
    location( intermediateDir )

	----------- PROJECT SETTINGS --------

    if not buildMonolithicEngine then
        defines { "CORE_DLL_EXPORT" }
    end

    files       { 
        "**.h", 
        "**.inl", 
        "**.cpp",
        "../public/core/**.cpp",
        "../public/core/**.h",
        "../public/core/**.inl"
    }

    -- Enable PCH file
    pchheader       "pch_core.h"
    pchsource       "pch_core.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    links       {
        "stdlib"
    }
	
    ----------- LINK THIRD PARTIES -----------------

    ThirdParty.Link( ThirdParty.Libs.Mimalloc )
    ThirdParty.Link( ThirdParty.Libs.ZLib )
    ThirdParty.Link( ThirdParty.Libs.SDL2 )
    ThirdParty.Link( ThirdParty.Libs.Optick )
    ThirdParty.Link( ThirdParty.Libs.GLM )
    ThirdParty.Link( ThirdParty.Libs.BugTrap )

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