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
	IncludeDir["TinyXML2"] = "Engine/external/TinyXML2/include"
	IncludeDir["ImGui"] = "Engine/external/imgui"
	IncludeDir["ImGuiBackends"] = "Engine/external/imgui/backends"


	LibDir = {}
	LibDir["DirectXTK"] = "Engine/external/DirectXTK/%{cfg.buildcfg}"
	LibDir["assimp"] = "Engine/external/assimp/lib/x64"
	LibDir["TinyXML2"] = "Engine/external/TinyXML2/lib/%{cfg.buildcfg}"

	include "Engine/external/imgui"

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"
	staticruntime "Off"

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
		"%{IncludeDir.TinyXML2}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiBackends}",
	}

	libdirs
	{
		"%{LibDir.DirectXTK}",
		"%{LibDir.assimp}",
		"%{LibDir.TinyXML2}",
	}

	links 
	{
		"d3d11.lib",
		"DirectXTK.lib",
		"d3dcompiler.lib",
		"winmm.lib",
		"assimp-vc140-mt.lib",
		"tinyxml2.lib",
		"ImGui"
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

	filter "configurations:Release"
		defines "P_RELEASE"
		runtime "Release"
		optimize "On"

project "KielansVehicleSim"
	location "KielansVehicleSim"
	kind "WindowedApp"
	language "C++"
	staticruntime "Off"

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

	filter "configurations:Release"
		defines "P_RELEASE"
		runtime "Release"
		optimize "On"