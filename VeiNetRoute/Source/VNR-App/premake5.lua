local target_name = "VeiNetRoute"
local target_prefix = ""
local extension = "exe"
local app_name = "VeiNetRoute"
local resource_defines = {}

project "VNR-App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

    targetdir (vnr_bin_out)
	objdir (vnr_int_out)
    targetname (target_name)
    targetprefix (target_prefix)

    location "%{wks.location}/VeiNetRoute/Build/ProjectFiles"

    files
    {
        "%{wks.location}/VeiNetRoute/Source/VNR-App/**.h",
        "%{wks.location}/VeiNetRoute/Source/VNR-App/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/VeiNetRoute/Source/VNR-App",
        "%{wks.location}/VeiNetRoute/Source/VNR-Core",
		"%{wks.location}/VeiNetRoute/ThirdParty", 
		"%{wks.location}/VeiNetRoute/ThirdParty/glm",
		"%{wks.location}/VeiNetRoute/ThirdParty/imgui",
		"%{wks.location}/VeiNetRoute/ThirdParty/GLFW/include",
		"%{wks.location}/VeiNetRoute/ThirdParty/spdlog/include",
		"%{wks.location}/VeiNetRoute/ThirdParty/yaml-cpp/include"  
    }

	links 
	{ 
		"VNR-Core"
	}
	defines 
	{
		"YAML_CPP_STATIC_DEFINE",
        "VNR_APP_NAME=\"" .. app_name .. "\""
	}
    ------------------ resources ------------------------
    table.insert(resource_defines, "BUILD_ICON_FILE_NAME=\\\"Default.ico\\\"")
    table.insert(resource_defines, "VNR_APP_NAME=\\\"".. app_name .. "\\\"")
    table.insert(resource_defines, "ORIGINAL_FILE_NAME=\\\"".. target_prefix ..target_name .. ".".. extension .."\\\"")

	filter "system:windows"
		systemversion "latest"
        local resource_build_options = constructResourceDefineFlags(resource_defines)  
        prebuildcommands {
            "rc /fo " .. vnr_int_out .. "/VeiM.rc.res %{wks.location}/VeiNetRoute/Source/VNR-Core/VNR-Core/Resources/VeiM.rc",
            "rc /fo " .. vnr_int_out .."/Default.rc2.res" .. resource_build_options .. " %{wks.location}/VeiNetRoute/Build/Resources/Default.rc2"
        }

        linkoptions{
            vnr_int_out .. "/VeiM.rc.res",
            vnr_int_out .. "/Default.rc2.res"
        }
	-----------------------------------------------------	
	filter "configurations:Debug"
		defines "VNR_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Shipping"
		defines "VNR_SHIPPING"
		runtime "Release"
		optimize "on"
        symbols "off"

