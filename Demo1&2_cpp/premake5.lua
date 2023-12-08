workspace "TexOptimization"
  architecture "x64"
	startproject "SoftSampler"

  configurations
  {
    "Debug",
    "Release",
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["glm"] = "vendor/glm"

include "vendor/Glad"
include "vendor/GLFW"

project "SoftSampler"
	location "SoftSampler"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	includedirs
	{
		"SoftSampler/src",
		"SoftSampler/vendor",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"

project "GradientDescent"
	location "GradientDescent"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"SoftSampler/src",
		"SoftSampler/vendor",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"
