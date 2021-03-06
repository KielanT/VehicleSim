workspace "VehicleSim"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["DirectXTK"] = "Engine/external/DirectXTK"
	IncludeDir["assimp"] = "Engine/external/assimp/include"
	IncludeDir["ImGui"] = "Engine/external/imgui"
	IncludeDir["ImGuiBackends"] = "Engine/external/imgui/backends"
	IncludeDir["PhysX"] = "Engine/external/PhysX/include"


	LibDir = {}
	LibDir["DirectXTK"] = "Engine/external/DirectXTK/%{cfg.buildcfg}"
	LibDir["assimp"] = "Engine/external/assimp/lib/x64"
	LibDir["PhysX"] = "Engine/external/PhysX/lib/%{cfg.buildcfg}"

	include "Engine/external/imgui"

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ppch.h"
	pchsource "Engine/source/ppch.cpp"

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp",
		"%{prj.name}/external/imgui/backends/imgui_impl_win32.h",
		"%{prj.name}/external/imgui/backends/imgui_impl_win32.cpp",
		"%{prj.name}/external/imgui/backends/imgui_impl_dx11.h",
		"%{prj.name}/external/imgui/backends/imgui_impl_dx11.cpp",
	}

	includedirs 
	{
		"%{prj.name}/source",
		"%{IncludeDir.DirectXTK}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiBackends}",
		"%{IncludeDir.PhysX}",

	}

	libdirs
	{
		"%{LibDir.DirectXTK}",
		"%{LibDir.assimp}",
		"%{LibDir.PhysX}",
		
	}

	links 
	{
		"d3d11",
		"DirectXTK",
		"d3dcompiler",
		"winmm",
		"assimp-vc140-mt",
		"ImGui",

		"PhysX_64.lib",
		"PhysXCommon_64.lib",
		"PhysXCooking_64.lib",
		"PhysXFoundation_64.lib",
		"PhysXExtensions_static_64.lib",
		"PhysXPvdSDK_static_64.lib",
		"PhysXVehicle_static_64.lib"

	}

	
	files("Engine/source/Graphics/Shaders/HLSL/*.hlsl")
	shadermodel("5.0")
	
	local shader_dir = "source/Graphics/Shaders/HLSL/"

	filter("files:**.hlsl")
      flags("ExcludeFromBuild")
      shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")

   filter("files:**_ps.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Pixel")

   filter("files:**_vs.hlsl")
      removeflags("ExcludeFromBuild")
      shadertype("Vertex")

	 shaderoptions({"/WX"})

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"P_PLATFORM_WINDOWS",
			"P_BUILD_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/KielansVehicleSim")
		}

	filter "configurations:Debug"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:checked"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:profile"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "On"

project "KielansVehicleSim"
	location "KielansVehicleSim"
	kind "WindowedApp"
	language "C++"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"Engine/source"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"P_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:checked"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"
		
	filter "configurations:profile"
		defines "P_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "On"