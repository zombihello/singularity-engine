newaction {
    trigger         = "export-thirdparty",
    description     = "Export third parties to JSON file '<MAIN_PREMAKE5_SCRIPT_DIR>/thirdparty/thirdparty.json'",
    execute         = function()
        local   filePath    = root .. "/thirdparty/thirdparty.json"
        local   file        = io.open( root .. "/thirdparty/thirdparty.json", "w+" )
        if file then
            file:write( Utils.ToJSON( ThirdParty.Libs ) )
            file:close()
        else
            error( "Failed to open file '" .. filePath .. "'" )
        end
    end
}

-- -------------------
-- General
-- -------------------
ThirdParty                  = {}
ThirdParty.Libs             = {}

-- Setup include directories for a third party
local function SetupIncludeDirs( table )
    -- Setup include directories for each platform
    for _, platformName in pairs( buildPlatforms ) do
        local       platformData = table[platformName]
        if platformData then
            filter { "platforms:" .. platformName }
                -- Setup include directories
                if platformData.IncludeDirs then   
                    for _, includeDir in ipairs( platformData.IncludeDirs ) do
                        externalincludedirs { table.Path .. "/" .. includeDir }
                    end        
                end

                -- Setup defines for all configurations
                local   definesData = platformData.Defines
                if definesData then
                    for _, defineData in ipairs( definesData ) do
                        defines { defineData }
                    end
                end

                -- Setup defines for specific configurations
                for _, configurationName in pairs( buildConfigurations ) do
                    local   configurationData = platformData[configurationName]
                    if configurationData and configurationData.Defines then
                        filter { "platforms:" .. platformName, "configurations:" .. configurationName }
                            for _, defineData in ipairs( configurationData.Defines ) do
                                defines { defineData }
                            end
                        filter {}
                    end
                end
            filter {}
        end
    end
end

-- Setup projects to compile third parties
function ThirdParty.SetupProjects()
    for name, table in pairs( ThirdParty.Libs ) do
        if table.CreateProject then
            table.CreateProject()
        end
    end
end

-- Link a third party
function ThirdParty.Link( table )
    -- Setup include directories for the library
    SetupIncludeDirs( table )

    -- Mark the third party as linked
    table.IsLinked = true

    -- Link the library
    for _, platformName in pairs( buildPlatforms ) do
        local       platformData = table[platformName]
        if platformData then
            -- Link libraries
            for _, configurationName in pairs( buildConfigurations ) do
                local   configurationData = platformData[configurationName]
                if configurationData and configurationData.Libs then
                    filter { "platforms:" .. platformName, "configurations:" .. configurationName }
                        for _, lib in ipairs( configurationData.Libs ) do
                            if not table.CreateProject then
                                links { table.Path .. "/" .. lib }
                            else
                                links { lib }
                            end
                        end
                    filter {}
                end
            end
        end
    end
end

-- -------------------
-- GLM
-- -------------------
ThirdParty.Libs.GLM     = {
    Path                = root .. "/thirdparty/glm-1.0.0",
    Licenses            = { "copying.txt" },
    IsForGameOnly       = false,
    IsTools             = false,
    Win64               = {
        IncludeDirs     = { "src/glm/" },
        Debug           = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Release         = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Retail          = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Defines         = nil,
        DLLs            = nil,
        Tools           = nil
    },
    CreateProject       = nil
}

-- -------------------
-- Mimalloc
-- -------------------
ThirdParty.Libs.Mimalloc    = {
    Path                    = root .. "/thirdparty/mimalloc-2.1.2",
    Licenses                = { "LICENSE" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/debug/mimalloc.lib" },
            DLLs            = { "bin/win64/debug/mimalloc.dll" }
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/release/mimalloc.lib" },
            DLLs            = { "bin/win64/release/mimalloc.dll" }
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/release/mimalloc.lib" },
            DLLs            = { "bin/win64/release/mimalloc.dll" }
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- RapidJSON
-- -------------------
ThirdParty.Libs.RapidJSON   = {
    Path                    = root .. "/thirdparty/rapidjson-1.1.0",
    Licenses                = { "license.txt" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "src/include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = nil,
            DLLs            = nil
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = nil,
            DLLs            = nil
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = nil,
            DLLs            = nil
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- SDL2
-- -------------------
ThirdParty.Libs.SDL2        = {
    Path                    = root .. "/thirdparty/SDL2-2.30.0",
    Licenses                = { "LICENSE.txt" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/SDL2d.lib" },
            DLLs            = { "bin/win64/SDL2d.dll" }
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/SDL2.lib" },
            DLLs            = { "bin/win64/SDL2.dll" }
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/SDL2.lib" },
            DLLs            = { "bin/win64/SDL2.dll" }
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- ZLib
-- -------------------
ThirdParty.Libs.ZLib        = {
    Path                    = root .. "/thirdparty/zlib-1.2.13",
    Licenses                = { "LICENSE" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/zlibd.lib" },
            DLLs            = { "bin/win64/zlibd.dll" }
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/zlib.lib" },
            DLLs            = { "bin/win64/zlib.dll" }
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/zlib.lib" },
            DLLs            = { "bin/win64/zlib.dll" }
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- Vulkan SDK
-- -------------------
ThirdParty.Libs.VulkanSDK   = {
    Path                    = os.getenv( "VULKAN_SDK" ),
    Licenses                = { "Licenses/LICENSE.txt" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "Include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Lib/vulkan-1.lib" },
            DLLs            = nil
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Lib/vulkan-1.lib" },
            DLLs            = nil
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Lib/vulkan-1.lib" },
            DLLs            = nil
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- Volk
-- -------------------
ThirdParty.Libs.Volk        = {
    Path                    = ThirdParty.Libs.VulkanSDK.Path,
    Licenses                = nil,
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "Include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Volk" },
            DLLs            = nil
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Volk" },
            DLLs            = nil
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "Volk" },
            DLLs            = nil
        },
        Defines             = { "VK_USE_PLATFORM_WIN32_KHR" },
        DLLs                = nil,
        Tools               = nil
    },

    -- Create a project to compile the library
    CreateProject           = function()
        project "Volk"
            kind        "StaticLib"
            language    "C++"
            location( intermediateDir )

            files   { 
                "%{ThirdParty.Libs.Volk.Path}/Include/Volk/volk.c",
                "%{ThirdParty.Libs.Volk.Path}/Include/Volk/volk.h" 
            }
            vpaths  { 
                ["src/*"] = { 
                    "%{ThirdParty.Libs.Volk.Path}/Include/Volk/volk.c", 
                    "%{ThirdParty.Libs.Volk.Path}/Include/Volk/volk.h" 
                } 
            }

            includedirs { 
                ThirdParty.Libs.Volk.Path .. "/Include/",
                "./"
             }

             filter "platforms:Win64"
                defines { "VK_USE_PLATFORM_WIN32_KHR" }
             filter {}
    end
}

-- -------------------
-- SPIRV-Reflect
-- -------------------
ThirdParty.Libs.SPIRVReflect    = {
    Path                        = ThirdParty.Libs.VulkanSDK.Path,
    Licenses                    = nil,
    IsForGameOnly               = false,
    IsTools                     = true,
    Win64                       = {
        IncludeDirs             = { "Include/", "Source/" },
        Debug                   = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "SPIRVReflect" },
            DLLs                = nil
        },
        Release                 = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "SPIRVReflect" },
            DLLs                = nil
        },
        Retail                  = {
            IsUsed              = false,
            Defines             = nil,
            Libs                = nil,
            DLLs                = nil
        },
        Defines                 = nil,
        DLLs                    = nil,
        Tools                   = nil
    },

    -- Create a project to compile the library
    CreateProject           = function()
        project "SPIRVReflect"
            kind        "StaticLib"
            language    "C++"
            location( intermediateDir )
    
            files           { "%{ThirdParty.Libs.SPIRVReflect.Path}/Source/SPIRV-Reflect/spirv_reflect.c" }
            vpaths          { 
                ["src/*"] = { "%{ThirdParty.Libs.SPIRVReflect.Path}/Source/SPIRV-Reflect/spirv_reflect.c" } 
            }
    
            includedirs { 
                ThirdParty.Libs.SPIRVReflect.Path .. "/Include/",
                ThirdParty.Libs.SPIRVReflect.Path .. "/Source/",
                "./"
             }
    end
}

-- -------------------
-- GLSLang
-- -------------------
ThirdParty.Libs.GLSLang = {
    Path                = ThirdParty.Libs.VulkanSDK.Path,
    Licenses            = nil,
    IsForGameOnly       = false,
    IsTools             = true,
    Win64               = {
        IncludeDirs     = { "Include/" },
        Debug           = {
            IsUsed       = true,
            Defines     = nil,
            Libs        = {
                "Lib/glslangd.lib",
                "Lib/OSDependentd.lib",
                "Lib/MachineIndependentd.lib",
                "Lib/GenericCodeGend.lib"
            },
            DLLs        = nil
        },
        Release         = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = {
                "Lib/glslang.lib",
                "Lib/OSDependent.lib",
                "Lib/MachineIndependent.lib",
                "Lib/GenericCodeGen.lib"
            },
            DLLs        = nil
        },
        Retail          = {
            IsUsed      = false,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Defines         = nil,
        DLLs            = nil,
        Tools           = nil
    },
    CreateProject       = nil
}

-- -------------------
-- ShaderC
-- -------------------
ThirdParty.Libs.ShaderC = {
    Path                = ThirdParty.Libs.VulkanSDK.Path,
    Licenses            = nil,
    IsForGameOnly       = false,
    IsTools             = true,
    Win64               = {
        IncludeDirs     = { "Include/" },
        Debug           = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = {
                "Lib/shadercd.lib",
                "Lib/shaderc_utild.lib"
            },
            DLLs        = nil
        },
        Release         = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = {
                "Lib/shaderc.lib",
                "Lib/shaderc_util.lib"
            },
            DLLs        = nil
        },
        Retail          = {
            IsUsed      = false,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Defines         = nil,
        DLLs            = nil,
        Tools           = nil
    },
    CreateProject       = nil
}

-- -------------------
-- SPIRV-Tools
-- -------------------
ThirdParty.Libs.SPIRVTools  = {
    Path                    = ThirdParty.Libs.VulkanSDK.Path,
    Licenses                = nil,
    IsForGameOnly           = false,
    IsTools                 = true,
    Win64                   = {
        IncludeDirs         = { "Include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = {
                "Lib/SPIRVd.lib",
                "Lib/SPIRV-Toolsd.lib",
                "Lib/SPIRV-Tools-diffd.lib",
                "Lib/SPIRV-Tools-linkd.lib",
                "Lib/SPIRV-Tools-lintd.lib",
                "Lib/SPIRV-Tools-optd.lib",
                "Lib/SPIRV-Tools-reduced.lib"
            },
            DLLs            = nil
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = {
                "Lib/SPIRV.lib",
                "Lib/SPIRV-Tools.lib",
                "Lib/SPIRV-Tools-diff.lib",
                "Lib/SPIRV-Tools-link.lib",
                "Lib/SPIRV-Tools-lint.lib",
                "Lib/SPIRV-Tools-opt.lib",
                "Lib/SPIRV-Tools-reduce.lib"
            },
            DLLs            = nil
        },
        Retail              = {
            IsUsed          = false,
            Defines         = nil,
            Libs            = nil,
            DLLs            = nil
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- SPIRV-Cross
-- -------------------
ThirdParty.Libs.SPIRVCross      = {
    Path                        = ThirdParty.Libs.VulkanSDK.Path,
    Licenses                    = nil,
    IsForGameOnly               = false,
    IsTools                     = true,
    Win64                       = {
        IncludeDirs             = { "Include/" },
        Debug                   = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "Lib/spirv-cross-cored.lib" },
            DLLs                = nil
        },
        Release                 = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "Lib/spirv-cross-core.lib" },
            DLLs                = nil
        },
        Retail                  = {
            IsUsed              = false,
            Defines             = nil,
            Libs                = nil,
            DLLs                = nil
        },
        Defines                 = nil,
        DLLs                    = nil,
        Tools                   = nil
    },
    CreateProject               = nil
}

-- -------------------
-- Optick
-- -------------------
ThirdParty.Libs.Optick  = {
    Path                = root .. "/thirdparty/optick-1.4.0",
    Licenses            = { "LICENSE" },
    IsForGameOnly       = false,
    IsTools             = false,
    Win64               = {
        IncludeDirs     = { "include/" },
        Debug           = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = { "lib/win64/debug/OptickCored.lib" },
            DLLs        = { "bin/win64/debug/OptickCored.dll" }
        },
        Release         = {
            IsUsed      = true,
            Defines     = nil,
            Libs        = { "lib/win64/release/OptickCore.lib" },
            DLLs        = { "bin/win64/release/OptickCore.dll" }
        },
        Retail          = {
            IsUsed      = false,
            Defines     = nil,
            Libs        = nil,
            DLLs        = nil
        },
        Defines         = nil,
        DLLs            = nil,
        Tools           = { "bin/win64/Optick.exe" }
    },
    CreateProject       = nil
}

-- -------------------
-- Compressonator
-- -------------------
ThirdParty.Libs.Compressonator  = {
    Path                        = root .. "/thirdparty/compressonator-4.5.52",
    Licenses                    = { 
        "license/OpenEXR/README",
        "license/OpenEXR/LICENSE",
        "license/FrameworkLicense.txt",
        "license/README.md",
        "license/SDKLicense.txt"
    },
    IsForGameOnly               = false,
    IsTools                     = true,
    Win64                       = {
        IncludeDirs             = { "include/" },
        Debug                   = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { 
                "lib/win64/debug/Compressonator_MDd_DLL.lib",
                "lib/win64/debug/CMP_Framework_MDd_DLL.lib"
            },
            DLLs                = { 
                "bin/win64/debug/Compressonator_MDd_DLL.dll",
                "bin/win64/debug/CMP_Framework_MDd_DLL.dll"
            }
        },
        Release                 = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { 
                "lib/win64/release/Compressonator_MD_DLL.lib",
                "lib/win64/release/CMP_Framework_MD_DLL.lib"
            },
            DLLs                = { 
                "bin/win64/release/Compressonator_MD_DLL.dll",
                "bin/win64/release/CMP_Framework_MD_DLL.dll"
            }
        },
        Retail                  = {
            IsUsed              = false,
            Defines             = nil,
            Libs                = nil,
            DLLs                = nil
        },
        Defines                 = nil,
        DLLs                    = nil,
        Tools                   = nil
    },
    CreateProject               = nil
}

-- -------------------
-- BugTrap
-- -------------------
ThirdParty.Libs.BugTrap     = {
    Path                    = root .. "/thirdparty/bugtrap-1.4.9",
    Licenses                = { "LICENSE" },
    IsForGameOnly           = false,
    IsTools                 = false,
    Win64                   = {
        IncludeDirs         = { "include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/debug/BugTrapD-x64.lib" },
            DLLs            = { "bin/win64/debug/BugTrapD-x64.dll" }
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/release/BugTrap-x64.lib" },
            DLLs            = { "bin/win64/release/BugTrap-x64.dll" }
        },
        Retail              = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/release/BugTrap-x64.lib" },
            DLLs            = { "bin/win64/release/BugTrap-x64.dll" }
        },
        Defines             = nil,
        DLLs                = { "bin/win64/dbghelp.dll" },
        Tools               = { "bin/win64/CrashExplorer.exe" }
    },
    CreateProject           = nil
}

-- -------------------
-- Assimp
-- -------------------
ThirdParty.Libs.Assimp      = {
    Path                    = root .. "/thirdparty/assimp-5.4.3",
    Licenses                = { "LICENSE" },
    IsForGameOnly           = false,
    IsTools                 = true,
    Win64                   = {
        IncludeDirs         = { "include/" },
        Debug               = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/debug/assimpd.lib" },
            DLLs            = { "bin/win64/debug/assimpd.dll" }
        },
        Release             = {
            IsUsed          = true,
            Defines         = nil,
            Libs            = { "lib/win64/release/assimp.lib" },
            DLLs            = { "bin/win64/release/assimp.dll" }
        },
        Retail              = {
            IsUsed          = false,
            Defines         = nil,
            Libs            = nil,
            DLLs            = nil
        },
        Defines             = nil,
        DLLs                = nil,
        Tools               = nil
    },
    CreateProject           = nil
}

-- -------------------
-- MeshOptimizer
-- -------------------
ThirdParty.Libs.MeshOptimizer   = {
    Path                        = root .. "/thirdparty/meshoptimizer-0.23",
    Licenses                    = { "LICENSE.md" },
    IsForGameOnly               = false,
    IsTools                     = true,
    Win64                       = {
        IncludeDirs             = { "include/" },
        Debug                   = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "lib/win64/debug/meshoptimizer.lib" },
            DLLs                = { "bin/win64/debug/meshoptimizer.dll" }
        },
        Release                 = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "lib/win64/release/meshoptimizer.lib" },
            DLLs                = { "bin/win64/release/meshoptimizer.dll" }
        },
        Retail                  = {
            IsUsed              = false,
            Defines             = nil,
            Libs                = nil,
            DLLs                = nil
        },
        Defines                 = nil,
        DLLs                    = nil,
        Tools                   = nil
    },
    CreateProject               = nil
}

-- -------------------
-- Flecs
-- -------------------
ThirdParty.Libs.Flecs           = {
    Path                        = root .. "/thirdparty/flecs-4.0.5",
    Licenses                    = { "LICENSE" },
    IsForGameOnly               = true,
    IsTools                     = false,
    Win64                       = {
        IncludeDirs             = { "include/" },
        Debug                   = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "lib/win64/debug/flecs.lib" },
            DLLs                = { "bin/win64/debug/flecs.dll" }
        },
        Release                 = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "lib/win64/release/flecs.lib" },
            DLLs                = { "bin/win64/release/flecs.dll" }
        },
        Retail                  = {
            IsUsed              = true,
            Defines             = nil,
            Libs                = { "lib/win64/release/flecs.lib" },
            DLLs                = { "bin/win64/release/flecs.dll" }
        },
        Defines                 = { "FLECS_CPP_NO_AUTO_REGISTRATION" },
        DLLs                    = nil,
        Tools                   = nil
    },
    CreateProject               = nil
}