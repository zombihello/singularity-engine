-- -------------------
-- GLM
-- -------------------
GLM                 = {}
GLM.Path            = root .. "/thirdparty/glm-1.0.0"

function GLM.Link()
	-- Set include dirs
    externalincludedirs {
        GLM.Path .. "/src/glm/"
    }

	-- Copy license
    GLM.CopyLicense()
end

function GLM.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. GLM.Path .. "/copying.txt " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/copying.txt " .. buildDir .. "/licenses/glm.txt",
    }
end

-- -------------------
-- Mimalloc
-- -------------------
Mimalloc            = {}
Mimalloc.Path       = root .. "/thirdparty/mimalloc-2.1.2"

function Mimalloc.Link()
    -- Set include dirs
    externalincludedirs {
        Mimalloc.Path .. "/include/"
    }

    -- Set library name
    local   libName
    if buildMonolithicEngine then
        libName = "mimalloc-static"
    else
        libName = "mimalloc"
    end

    -- Link library and copy dlls and license
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            Mimalloc.Path .. "/lib/win64/debug/" .. libName .. ".lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. Mimalloc.Path .. "/bin/win64/debug/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            Mimalloc.Path .. "/lib/win64/release/" .. libName .. ".lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. Mimalloc.Path .. "/bin/win64/release/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter {}

    Mimalloc.CopyLicense()
end

function Mimalloc.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. Mimalloc.Path .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/mimalloc.txt",
    }
end

-- -------------------
-- RapidJSON
-- -------------------
RapidJson           = {}
RapidJson.Path      = root .. "/thirdparty/rapidjson-1.1.0"

function RapidJson.Link()
	-- Set include dirs
    externalincludedirs {
        RapidJson.Path .. "/src/include/"
    }

	-- Copy license
    RapidJson.CopyLicense()
end

function RapidJson.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. RapidJson.Path .. "/license.txt " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/license.txt " .. buildDir .. "/licenses/rapidjson.txt",
    }
end

-- -------------------
-- SDL2
-- -------------------
SDL2                = {}
SDL2.Path           = root .. "/thirdparty/SDL2-2.30.0"

function SDL2.Link()
    -- Set include dirs
    externalincludedirs {
        SDL2.Path .. "/include/"
    }

    -- Set library name
    local   libName
    if buildMonolithicEngine then
        libName = "SDL2-static"
    else
        libName = "SDL2"
    end

    -- Link library and copy dlls and license
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            SDL2.Path .. "/lib/win64/" .. libName .. "d.lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. SDL2.Path .. "/bin/win64/" .. libName .. "d.dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            SDL2.Path .. "/lib/win64/" .. libName .. ".lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. SDL2.Path .. "/bin/win64/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter {}

    SDL2.CopyLicense()
end

function SDL2.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. SDL2.Path .. "/LICENSE.txt " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE.txt " .. buildDir .. "/licenses/sdl2.txt",
    }
end

-- -------------------
-- ZLib
-- -------------------
ZLib            = {}
ZLib.Path       = root .. "/thirdparty/zlib-1.2.13"

function ZLib.Link()
    -- Set include dirs
    externalincludedirs {
        ZLib.Path .. "/include/"
    }

    -- Set library name
    local   libName
    if buildMonolithicEngine then
        libName = "zlibstatic"
    else
        libName = "zlib"
    end

    -- Link library and copy dlls and license
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            ZLib.Path .. "/lib/win64/" .. libName .. "d.lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. ZLib.Path .. "/bin/win64/" .. libName .. "d.dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            ZLib.Path .. "/lib/win64/" .. libName .. ".lib" 
        }

        if not buildMonolithicEngine then
            postbuildcommands { 
                "{COPY} " .. ZLib.Path .. "/bin/win64/" .. libName .. ".dll " .. buildDir .. binariesDir .. outputDir .. "/" 
            }
        end
    filter {}

    ZLib.CopyLicense()
end

function ZLib.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. ZLib.Path .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/zlib.txt",
    }
end

-- -------------------
-- Vulkan SDK
-- -------------------
VulkanSDK 					                = {}
VulkanSDK.Path 	                            = os.getenv( "VULKAN_SDK" )
VulkanSDK.Volk 				                = {}
VulkanSDK.SPIRV_Reflect 	                = {}
VulkanSDK.GLSLang			                = {}
VulkanSDK.ShaderC                           = {}
VulkanSDK.SPIRV_Tools                       = {}
VulkanSDK.SPIRV_Cross                       = {}

function VulkanSDK.Link()
	-- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }

	-- Set library name
    filter "platforms:Win64"  
        links       {
            "%{VulkanSDK.Path}/Lib/vulkan-1.lib"
        }
    filter {}

    -- Copy Vulkan SDK's license text to build directory
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. "%{VulkanSDK.Path}/Licenses/LICENSE.txt " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE.txt " .. buildDir .. "/licenses/vulkansdk.txt",
    }
end

function VulkanSDK.Volk.Link()
	-- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }
	
	-- Compile Volk
	files 			{
		"%{VulkanSDK.Path}/Include/Volk/volk.c"
	}

	vpaths      	{
        ["thirdparty/vulkansdk/volk/*"] = { "%{VulkanSDK.Path}/Include/Volk/volk.c" }
    }
	
	filter { "files:**/Include/Volk/volk.c" }
        flags       { "NoPCH" }
	filter {}
end

function VulkanSDK.SPIRV_Reflect.Link()
	-- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/",
		"%{VulkanSDK.Path}/Source/"
    }
	
	-- Compile SPIRV-Reflect
	files 			{
		"%{VulkanSDK.Path}/Source/SPIRV-Reflect/spirv_reflect.c"
	}

	vpaths      	{
        ["thirdparty/vulkansdk/spirv-reflect/*"] = { "%{VulkanSDK.Path}/Source/SPIRV-Reflect/spirv_reflect.c" }
    }

	filter { "files:**/Source/SPIRV-Reflect/spirv_reflect.c" }
        flags       { "NoPCH" }
	filter {}
end

function VulkanSDK.GLSLang.Link()
	-- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }
	
	-- Set library name
	-- Windows
    filter { "platforms:Win64", "configurations:Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/glslangd.lib",
			"%{VulkanSDK.Path}/Lib/OSDependentd.lib",
			"%{VulkanSDK.Path}/Lib/MachineIndependentd.lib",
			"%{VulkanSDK.Path}/Lib/GenericCodeGend.lib"
        }
	filter { "platforms:Win64", "configurations:not Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/glslang.lib",
			"%{VulkanSDK.Path}/Lib/OSDependent.lib",
			"%{VulkanSDK.Path}/Lib/MachineIndependent.lib",
			"%{VulkanSDK.Path}/Lib/GenericCodeGen.lib"
        }
    filter {}
end

function VulkanSDK.ShaderC.Link()
    -- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }
	
	-- Set library name
	-- Windows
    filter { "platforms:Win64", "configurations:Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/shadercd.lib",
			"%{VulkanSDK.Path}/Lib/shaderc_utild.lib"
        }
	filter { "platforms:Win64", "configurations:not Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/shaderc.lib",
			"%{VulkanSDK.Path}/Lib/shaderc_util.lib"
        }
    filter {}
end

function VulkanSDK.SPIRV_Tools.Link()
	-- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }
	
	-- Set library name
	-- Windows
    filter { "platforms:Win64", "configurations:Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/SPIRVd.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Toolsd.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Tools-diffd.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Tools-linkd.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-lintd.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-optd.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-reduced.lib"
        }
	filter { "platforms:Win64", "configurations:not Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/SPIRV.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Tools.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Tools-diff.lib",
			"%{VulkanSDK.Path}/Lib/SPIRV-Tools-link.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-lint.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-opt.lib",
            "%{VulkanSDK.Path}/Lib/SPIRV-Tools-reduce.lib"
        }
    filter {}
end

function VulkanSDK.SPIRV_Cross.Link()
    -- Set include dirs
    externalincludedirs     {
        "%{VulkanSDK.Path}/Include/"
    }

    -- Set library name
	-- Windows
    filter { "platforms:Win64", "configurations:Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/spirv-cross-cored.lib"
        }
	filter { "platforms:Win64", "configurations:not Debug" }  
        links       {
            "%{VulkanSDK.Path}/Lib/spirv-cross-core.lib"
        }
    filter {}
end

-- -------------------
-- Optick
-- -------------------
Optick            = {}
Optick.Path       = root .. "/thirdparty/optick-1.4.0"

function Optick.Link()
    -- Set include dirs
    externalincludedirs {
        Optick.Path .. "/include/"
    }

    -- Link library and copy dlls
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            Optick.Path .. "/lib/win64/debug/OptickCored.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. Optick.Path .. "/bin/win64/debug/OptickCored.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. Optick.Path .. "/bin/win64/Optick.exe " .. buildDir .. binariesDir .. outputDir .. "/" 
        }
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            Optick.Path .. "/lib/win64/release/OptickCore.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. Optick.Path .. "/bin/win64/release/OptickCore.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. Optick.Path .. "/bin/win64/Optick.exe " .. buildDir .. binariesDir .. outputDir .. "/" 
        }
    filter {}

    -- Copy lincese
    Optick.CopyLicense()
end

function Optick.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. Optick.Path .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/optick.txt",
    }
end

-- -------------------
-- Compressonator
-- -------------------
Compressonator            = {}
Compressonator.Path       = root .. "/thirdparty/compressonator-4.5.52"

function Compressonator.Link()
    -- Set include dirs
    externalincludedirs {
        Compressonator.Path .. "/include/"
    }

    -- Link library and copy dlls
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            Compressonator.Path .. "/lib/win64/debug/Compressonator_MDd_DLL.lib",
            Compressonator.Path .. "/lib/win64/debug/CMP_Framework_MDd_DLL.lib"
        }

        postbuildcommands { 
            "{COPY} " .. Compressonator.Path .. "/bin/win64/debug/Compressonator_MDd_DLL.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. Compressonator.Path .. "/bin/win64/debug/CMP_Framework_MDd_DLL.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            Compressonator.Path .. "/lib/win64/release/Compressonator_MD_DLL.lib",
            Compressonator.Path .. "/lib/win64/release/CMP_Framework_MD_DLL.lib"
        }

        postbuildcommands { 
            "{COPY} " .. Compressonator.Path .. "/bin/win64/release/Compressonator_MD_DLL.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. Compressonator.Path .. "/bin/win64/release/CMP_Framework_MD_DLL.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter {}

    -- Copy lincese
    Compressonator.CopyLicense()
end

function Compressonator.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{MKDIR} " .. buildDir .. "/licenses/compressonator",
        "{COPY} " .. Compressonator.Path .. "/license " .. buildDir .. "/licenses/compressonator"
    }
end

-- -------------------
-- BugTrap
-- -------------------
BugTrap            = {}
BugTrap.Path       = root .. "/thirdparty/bugtrap-1.4.9"

function BugTrap.Link()
    -- Set include dirs
    externalincludedirs {
        BugTrap.Path .. "/include/"
    }

    -- Link library and copy dlls
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            BugTrap.Path .. "/lib/win64/debug/BugTrapD-x64.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. BugTrap.Path .. "/bin/win64/debug/BugTrapD-x64.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. BugTrap.Path .. "/bin/win64/CrashExplorer.exe " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. BugTrap.Path .. "/bin/win64/dbghelp.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            BugTrap.Path .. "/lib/win64/release/BugTrap-x64.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. BugTrap.Path .. "/bin/win64/release/BugTrap-x64.dll " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. BugTrap.Path .. "/bin/win64/CrashExplorer.exe " .. buildDir .. binariesDir .. outputDir .. "/",
            "{COPY} " .. BugTrap.Path .. "/bin/win64/dbghelp.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter {}

    -- Copy lincese
    BugTrap.CopyLicense()
end

function BugTrap.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. BugTrap.Path .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/bugtrap.txt",
    }
end

-- -------------------
-- Assimp
-- -------------------
Assimp            = {}
Assimp.Path       = root .. "/thirdparty/assimp-5.4.3"

function Assimp.Link()
    -- Set include dirs
    externalincludedirs {
        Assimp.Path .. "/include/"
    }

    -- Link library and copy dlls
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            Assimp.Path .. "/lib/win64/debug/assimpd.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. Assimp.Path .. "/bin/win64/debug/assimpd.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            Assimp.Path .. "/lib/win64/release/assimp.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. Assimp.Path .. "/bin/win64/release/assimp.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter {}

    -- Copy lincese
    Assimp.CopyLicense()
end

function Assimp.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. Assimp.Path .. "/LICENSE " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE " .. buildDir .. "/licenses/assimp.txt",
    }
end

-- -------------------
-- MeshOptimizer
-- -------------------
MeshOptimizer            = {}
MeshOptimizer.Path       = root .. "/thirdparty/meshoptimizer-0.23"

function MeshOptimizer.Link()
    -- Set include dirs
    externalincludedirs {
        MeshOptimizer.Path .. "/include/"
    }

    -- Link library and copy dlls
    -- Windows
    filter { "platforms:Win64", "configurations:Debug" }
        links { 
            MeshOptimizer.Path .. "/lib/win64/debug/meshoptimizer.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. MeshOptimizer.Path .. "/bin/win64/debug/meshoptimizer.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter { "platforms:Win64", "configurations:not Debug" }
        links   { 
            MeshOptimizer.Path .. "/lib/win64/release/meshoptimizer.lib" 
        }

        postbuildcommands { 
            "{COPY} " .. MeshOptimizer.Path .. "/bin/win64/release/meshoptimizer.dll " .. buildDir .. binariesDir .. outputDir .. "/"
        }
    filter {}

    -- Copy lincese
    MeshOptimizer.CopyLicense()
end

function MeshOptimizer.CopyLicense()
    postbuildcommands    {
        "{MKDIR} " .. buildDir .. "/licenses",
        "{COPY} " .. MeshOptimizer.Path .. "/LICENSE.md " .. buildDir .. "/licenses",
        "{MOVE} " .. buildDir .. "/licenses/LICENSE.md " .. buildDir .. "/licenses/meshoptimizer.txt",
    }
end