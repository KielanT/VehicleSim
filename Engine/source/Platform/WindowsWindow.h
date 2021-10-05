#pragma once
#include "Project/Interfaces/IWindow.h"

#include "Project/Core.h"

#include "Project/Interfaces/IRenderer.h"
#include "Utility/Lab/Timer.h"


namespace Project
{
	class P_API WindowsWindow : public IWindow
	{
	public:
		WindowsWindow(WindowProperties& props);
		virtual ~WindowsWindow();

		// Updates the window with the scene
		virtual void Update(ISceneManager* m_SceneManager) override;

		// Returns the window width
		virtual unsigned int GetWidth() const override { return 0; }
		// Returns the window height
		virtual unsigned int GetHeight() const override { return 0; }

		// Returns the window propities
		virtual WindowProperties GetWindowProperties() const override { return m_Props; }

		// Function for creating the window proc
		static LRESULT CALLBACK WindowProc(
			HWND hWnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam
		);

		// Gets the window handle
		const HWND GetWindowHandle() { return m_hWnd; };

	private:
		// Initalize the window
		virtual BOOL Init(WindowProperties& props);
		// Shutdown the window
		virtual void Shutdown();

		// Creates the window
		HRESULT CreateDesktopWindow(WindowProperties& props);

		// Runs the window with the scene
		HRESULT Run(ISceneManager* m_SceneManager);

	private:
		HWND      m_hWnd; // Window handle member varibale

		HRESULT m_Window; // Member variable for the window

	private:
		WindowProperties m_Props; // Window props member variable
		Timer myTimer; // Timer for the frame time

	};

	static HINSTANCE m_hInstance;
	static std::wstring m_windowClassName;

}