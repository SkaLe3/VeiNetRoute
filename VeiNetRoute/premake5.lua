-- premake5.lua
include "Dependencies.lua"
include "Common.lua"

workspace "VeiNetRoute"
	architecture "x64"
	startproject "VNR-App"
	configurations { "Debug", "Shipping" }
	platforms {"Win64"}
	buildoptions { "/MP" }

	location "..\\"
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
vnr_bin_out = "%{wks.location}/VeiNetRoute/Binaries/VeiNetRoute/" .. outputdir .. "/VeiNetRoute"
vnr_int_out = "%{wks.location}/VeiNetRoute/Intermediate/VeiNetRoute/" .. outputdir .. "/VeiNetRoute"
tp_bin_out = "%{wks.location}/VeiNetRoute/Binaries/ThirdParty/" .. outputdir .. "/%{prj.name}"
tp_int_out = "%{wks.location}/VeiNetRoute/Intermediate/ThirdParty/" .. outputdir .. "/%{prj.name}"

group "Dependencies"
	include "ThirdParty/GLFW"
	include "ThirdParty/Glad"
	include "ThirdParty/imgui"
	include "ThirdParty/yaml-cpp"
group ""

group "VeiNetRoute"
	include "Source/VNR-Core"
    include "Source/VNR-App"
group ""
