project "shadercompiler_vk"
    if not buildMonolithicEngine then
        kind "SharedLib"
    else
        kind "StaticLib"
    end
    language    "C++"
    location( intermediateDir )
	
    ----------- PROJECT SETTINGS --------

    files       {
		"**.inl", 
        "**.cpp",
        "**.h",
        "**.rc",
        "../../../public/tools/shadercompiler/**.h",
        "../../../public/core/**.cpp"
    }

    vpaths      {
        ["src/*"]       = { "**.h", "**.inl", "**.cpp", "**.rc" },
		["public/*"]    = { "../../../public/**.h", "../../../public/**.inl", "../../../public/**.cpp" }
    }

    links       {
        "core",
        "stdlib",
		"interfaces",
		"shadercache"
    }

	----------- LINK THIRD PARTIES -----------------

    GLM.Link()
	VulkanSDK.SPIRV_Cross.Link()
	VulkanSDK.GLSLang.Link()
    VulkanSDK.SPIRV_Tools.Link()
    VulkanSDK.ShaderC.Link()