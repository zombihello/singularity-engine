-- Path to src folder
root = _MAIN_SCRIPT_DIR .. "/"

newoption {
	trigger		    = "game",
	value		    = "game",
	default         = "sandbox",
	description     = "Game which the launcher will be depends on"
}

newoption {
    trigger         = "intermediate-dir",
    value           = "PATH",
    default         = "intermediate/",
    description     = "Intermediate directory for compiling of the engine and the game in 'src' directory"
}

newoption {
    trigger         = "build-dir",
    value           = "PATH",
    default         = root .. "/../game/",
    description     = "Path to build directory"
}

newoption {
    trigger         = "workspace-name",
    value           = "VALUE",
    default         = "singularity_engine",
    description     = "Workspace name"
}

newoption {
    trigger     = "build-monolithic-engine",
    description = "Assemble the engine monolithically"       
}

newoption {
    trigger     = "workspace-on-root",
    description = "Place the workspace file in the root"
}

--------- GLOBAL VARIABLES -----------------

-- Path to intermediate directory for compiling the engine and the game
intermediateDir				= root .. "/" .. _OPTIONS["intermediate-dir"] .. "/" .. _ACTION .. "/"

-- Path to intermediate directory for third parties
intermediateThirdPartyDir	= intermediateDir .. "thirdparty/"

-- Path to intermediate output directory
intermediateOutputDir       = "%{string.lower(cfg.platform)}/%{string.lower(cfg.buildcfg)}/"

-- Path to build directory
buildDir                    = _OPTIONS["build-dir"] .. "/"

-- Path to binaries directory
binariesDir				    = "bin/"

-- Output directory name
outputDir			        = "%{string.lower(cfg.platform)}_%{string.lower(cfg.buildcfg)}/"

-- Workspace name
workspaceName               = _OPTIONS["workspace-name"]

-- Game which the launcher depends on
game                        = _OPTIONS["game"]

-- Print some information about configuration
print( "Intermediate directory: " .. intermediateDir )
print( "Build directory: " .. buildDir )
print( "Workspace name: " .. workspaceName )

-- Is need assemble the engine monolithically
if _OPTIONS["build-monolithic-engine"] then
    buildMonolithicEngine = true
    print( "Build type: Monolithically" )
else
    buildMonolithicEngine = false
    print( "Build type: Modular" )
end

-- Is need place the workspace file in the root
if _OPTIONS["workspace-on-root"] then
    workspaceLocation = root
else
    workspaceLocation = intermediateDir
end 
print( "Workspace file path: " .. workspaceLocation )
print( "" )

--------------- MODULES ---------------
launcher                    = "launcher/"
core                        = "core/"
stdlib                      = "libs/stdlib/"
appframework                = "libs/appframework/"
inputsystem                 = "inputsystem/"
filesystem                  = "filesystem/"
interfaces                  = "libs/interfaces/"
engine						= "engine/"
game_sandbox				= "game/sandbox"
gameinfo					= "libs/gameinfo/"
studiorender                = "studiorender/"
studioapi_vk 				= "studiorender/studioapi/vk/"
resourcesystem				= "resourcesystem/"
shaderlib                   = "libs/shaderlib/"
stdshaders                  = "materialsystem/stdshaders/"
materialsystem              = "materialsystem/"
shadercompiler              = "tools/shadercompiler/"
shadercompiler_vk 			= "tools/shadercompiler/vk/"
shadercache					= "libs/shadercache/"
smatdoc						= "libs/smatdoc/"
resourcecompiler            = "tools/resourcecompiler/"
texture_tool                = "tools/texture_tool/"
stexdoc                     = "libs/stexdoc/"
pixelformatinfos			= "libs/pixelformatinfos/"
material_tool               = "tools/material_tool/"
smdldoc                     = "libs/smdldoc/"
model_tool                  = "tools/model_tool/"

--------------- THIRD PARTIES ---------
include( root .. "/thirdparty/thirdparty.lua" )

workspace( workspaceName )
    location( workspaceLocation )
    configurations 	    { "Debug", "Release", "Retail" }
    platforms 		    { "Win64" }
    defaultplatform	    "Win64"

    ---------------- GLOBAL SETTINGS ---------------

    characterset 		"ASCII"
    floatingpoint 		"Fast"
	vectorextensions 	"SSE2"	
	externalwarnings 	"Off"
	
    targetdir( buildDir .. binariesDir .. outputDir )
	objdir( intermediateDir .. intermediateOutputDir .. "%{prj.name}/" )

    flags           { "MultiProcessorCompile" }
    filter { "files:public/**.cpp" }
        flags       { "NoPCH" }
	filter { "files:public/**.c" }
        flags       { "NoPCH" }
    filter {}

    includedirs         {
        "public/",
        "public/libs/",
        "./"
    }

    defines 		    {
        "PLATFORM_SUBDIR=\"%{string.lower(cfg.platform)}_%{string.lower(cfg.buildcfg)}\""
    }

    if buildMonolithicEngine then
        defines {
            "BUILD_MONOLITHIC_ENGINE"
        }
    end

    --------------- PLATFORM SETTINGS --------------

    filter "platforms:Win64"
        system 			"Windows"
        architecture 	"x64"
        cppdialect 		"C++17"
        staticruntime 	"Off"
        debugdir( buildDir .. binariesDir .. outputDir )

        defines 		{
            "_WIN64",
            "_CRT_SECURE_NO_WARNINGS",		-- For remove on unsafe C functions
            "_CRT_NONSTDC_NO_DEPRECATE",
            "PLATFORM_64BIT=1"
        }

        buildoptions 	{ 
            "/FC"     -- Set full path of source code file when using the __FILE__ macro					
        }
	filter {}

    --------------- CONFIGURATION SETTINGS --------------

	filter "configurations:Debug"
        defines 	        { "DEBUG=1",  "RELEASE=0", "RETAIL=0" }
        symbols 	        "On"
        inlining            "Disabled"
        runtime 	        "Debug"

    filter "configurations:Release"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=1", "RETAIL=0" }
        optimize 	        "Speed"
        runtime 			"Release"
        inlining            "Auto"

    filter "configurations:Retail"
        defines 	        { "NDEBUG", "DEBUG=0", "RELEASE=0", "RETAIL=1" }
        optimize            "Speed"
        runtime 			"Release"
        inlining            "Auto"
    filter {}

    -------------- BUILD TYPE SETTINGS -------------
	
	-- StaticLib
	filter "kind:StaticLib"
        targetdir( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/" )
        defines { "BUILD_STATIC_LIB" }
    filter { "kind:StaticLib", "platforms:Win64" }
        symbolspath( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/%{cfg.buildtarget.basename}.pdb" )

    -- SharedLib
    filter "kind:SharedLib"
        symbolspath( intermediateDir .. intermediateOutputDir .. "%{prj.name}/lib/%{cfg.buildtarget.basename}.pdb" )
    filter {}

    ----------------- MODULES ------------------

    group "/Engine"
        include( launcher )
		include( core )
        include( inputsystem )
        include( filesystem )
		include( engine )
        include( studiorender )
        include( materialsystem )
		include( studioapi_vk )
		include( resourcesystem )
        group "/Engine/Shaders"
            include( stdshaders )
        group "/Engine/Libraries"
            include( stdlib )
            include( appframework )
            include( interfaces )
			include( gameinfo )
            include( shaderlib )
			include( shadercache )
			include( smatdoc )
            include( stexdoc )
			include( pixelformatinfos )
            include( smdldoc )
    group "/Tools"
        group "/Tools/Shader Compile"
            include( shadercompiler )
			include( shadercompiler_vk )
        group "/Tools/Resource Compiler"
            include( resourcecompiler )
            include( texture_tool )
			include( material_tool )
            include( model_tool )
	group "/Game"
		include( game_sandbox )