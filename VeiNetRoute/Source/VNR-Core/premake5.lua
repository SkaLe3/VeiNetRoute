project "VNR-Core"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (vnr_bin_out)
    objdir (vnr_int_out)

    location "%{wks.location}/VeiNetRoute/Build/ProjectFiles"

    files
    {
        "%{wks.location}/VeiNetRoute/Source/VNR-Core/**.h",
        "%{wks.location}/VeiNetRoute/Source/VNR-Core/**.cpp",
		"%{wks.location}/VeiNetRoute/ThirdParty/stb_image/**.h",
		"%{wks.location}/VeiNetRoute/ThirdParty/stb_image/**cpp",
		"%{wks.location}/VeiNetRoute/ThirdParty/glm/glm/**.hpp",
		"%{wks.location}/VeiNetRoute/ThirdParty/glm/glm/**.inl"
    }

    includedirs
    {
        "%{wks.location}/VeiNetRoute/Source/VNR-Core",
		"%{wks.location}/VeiNetRoute/ThirdParty",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.stb_image}"
    }

	defines
	{
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE"
	}
    

	links 
	{ 
		"Glad",
		"GLFW",
		"ImGui",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VNR_DEBUG"
		runtime "Debug"
		symbols "on"		

	filter "configurations:Shipping"
		defines "VNR_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"

