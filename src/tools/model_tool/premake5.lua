project "model_tool"
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
        "../../public/tools/resource_tools/**.h",
        "../../public/core/**.cpp"
    }

    -- Enable PCH file
    pchheader       "pch_model_tool.h"
    pchsource       "pch_model_tool.cpp"
    includedirs     { "./" }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp" },
        ["public/*"]    = { "../public/**.h", "../public/**.inl", "../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces",
        "smdldoc"
    }

	----------- LINK THIRD PARTIES -----------------

    GLM.Link()
	RapidJson.Link()
    Assimp.Link()
    MeshOptimizer.Link()

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