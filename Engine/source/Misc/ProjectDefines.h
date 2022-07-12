#pragma once

#include <string>
#include <Windows.h>

// Files for project defines and variables
namespace Project
{
	// Variables
	typedef unsigned int TInt32; 
	typedef unsigned char TInt8;

	// Entity variables
	typedef TInt32 TEntityUID;
	const TEntityUID SystemUID = 0xffffffff; // The system UID

	enum class ERendererType // Enum class for the renderer type
	{
		None = 0, 
		DirectX11
	};

	enum class EPhysicsType // Enum class for the physics type
	{
		None = 0,
		PhysX4 // NVIDIA PhysX SDK 4.1
	};

	enum class WindowType
	{
		Windowed = 0,
		Fullscreen,
		FullscreenBorderless
	};

	struct WindowProperties // Window properties struct (in this file to help stop include errors
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		ERendererType RendererType;
		HWND Hwnd = NULL;
		EPhysicsType PhysicsType;
		WindowType windowType;

		WindowProperties(const std::string& title = "GraphicStarterProject",
			unsigned int width = 1280, unsigned int height = 720,
			ERendererType rendererType = ERendererType::DirectX11, EPhysicsType physicsType = EPhysicsType::PhysX4,
			WindowType type = WindowType::Windowed) : Title(title), Width(width), Height(height), RendererType(rendererType), PhysicsType(physicsType),
			windowType(type)
		{
		}
	};
}