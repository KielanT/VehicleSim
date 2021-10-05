#include "ppch.h"

#include "WindowsWindow.h"

#include "Utility/Lab/Input.h"
#include "Utility/ErrorLogger.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "Graphics/DirectX11/DirectX11Renderer.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Project
{
    IWindow* IWindow::Create(WindowProperties& props)
    {
        return new WindowsWindow(props);
    }

    WindowsWindow::WindowsWindow(WindowProperties& props)
    {

        Init(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        Shutdown();
    }


    BOOL WindowsWindow::Init(WindowProperties& props)
    {
        m_windowClassName = L"WindowClass";
        m_Window = S_OK;

        m_Window = CreateDesktopWindow(props);

        return TRUE;
    }


    void WindowsWindow::Update(ISceneManager* m_SceneManager)
    {
        if (SUCCEEDED(m_Window))
        {
            m_Window = Run(m_SceneManager);
        }
    }

    void WindowsWindow::Shutdown()
    {
        DestroyWindow(m_hWnd);
    }

    HRESULT WindowsWindow::CreateDesktopWindow(WindowProperties& props)
    {
        // Get a stock icon to show on the taskbar for this program.
        SHSTOCKICONINFO stockIcon;
        stockIcon.cbSize = sizeof(stockIcon);
        if (SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON, &stockIcon) != S_OK) // Returns false on failure
        {
            return false;
        }

        // Register window class. Defines various UI features of the window for our application.
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowsWindow::WindowProc;    // Which function deals with windows messages
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0; SIID_APPLICATION;
        wcex.hInstance = m_hInstance;
        wcex.hIcon = stockIcon.hIcon; // Which icon to use for the window
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // What cursor appears over the window
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = m_windowClassName.c_str();
        wcex.hIconSm = stockIcon.hIcon;
        if (!RegisterClassEx(&wcex)) // Returns false on failure
        {
            return false;
        }


        // Select the type of window to show our application in
        DWORD windowStyle = WS_OVERLAPPEDWINDOW; // Standard window
        //DWORD windowStyle = WS_POPUP;          // Alternative: borderless. If you also set the viewport size to the monitor resolution, you 
                                                 // get a "fullscreen borderless" window, which works better with alt-tab than DirectX fullscreen,
                                                 // which is an option in Direct3DSetup.cpp. DirectX fullscreen has slight better performance though.

        // Calculate overall dimensions for the window. We will render to the *inside* of the window. But the
        // overall winder will be larger because it includes the borders, title bar etc. This code calculates
        // the overall size of the window given our choice of viewport size.
        RECT rc = { 0, 0, props.Width, props.Height };
        AdjustWindowRect(&rc, windowStyle, FALSE);

        // Create window, the second parameter is the text that appears in the title bar

        std::wstring wTitle = std::wstring(props.Title.begin(), props.Title.end());
        const wchar_t* wcharTitle = wTitle.c_str();
        m_hWnd = CreateWindow(m_windowClassName.c_str(), wcharTitle, windowStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, nullptr);
        if (!m_hWnd)
        {
            return false;
        }

        props.Hwnd = m_hWnd;
        m_Props = props;

        ShowWindow(m_hWnd, 1);
        UpdateWindow(m_hWnd);

        return TRUE;
    }

    HRESULT WindowsWindow::Run(ISceneManager* m_SceneManager)
    {
        HRESULT hr = S_OK;


        //// Prepare TL-Engine style input functions
        InitInput();

        //IMGUI
        //*******************************
        // Initialise ImGui
        //*******************************

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplWin32_Init(m_Props.Hwnd);

        DirectX11Renderer* renderer = static_cast<DirectX11Renderer*>(m_SceneManager->GetRenderer());
        ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetDeviceContext());

        // Initialise scene
        if (!m_SceneManager->LoadFirstScene())
        {
            ErrorLogger errorLog;
            errorLog.ErrorMessage(m_Props, "Error Loading first scene");
            return 0;
        }


        myTimer.Start();
        // Main message loop - this is a Windows equivalent of the loop in a TL-Engine application
        MSG msg = {};
        while (msg.message != WM_QUIT) // As long as window is open
        {
            // Check for and deal with any window messages (input, window resizing, minimizing, etc.).
            // The actual message processing happens in the function WndProc below
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                // Deal with messages
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            else // When no windows messages left to process then render & update our scene
            {
                // Update the scene by the amount of time since the last frame
                float frameTime = myTimer.GetLapTime();
                m_SceneManager->SceneLoop(frameTime);
                /*if (KeyHit(Key_R))
                {
                    m_SceneManager->LoadScene(1);
                    m_SceneManager->RemoveSceneAtIndex(0);
                }*/
            }
        }

        //IMGUI
         //*******************************
        // Shutdown ImGui
        //*******************************

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        //*******************************
    }

   
    LRESULT WindowsWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) // IMGUI this line passes user input to ImGUI
            return true;

        switch (msg)
        {
        case WM_PAINT: // A necessary message to ensure the window content is displayed
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY: // Another necessary message to deal with the window being closed
            PostQuitMessage(0);
            break;


            // The WM_KEYXXXX messages report keyboard input to our window.
            // This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
            // so you don't need to change this code. Instead simply use KeyHit, KeyHeld etc.
        case WM_KEYDOWN:
            KeyDownEvent(static_cast<KeyCode>(wParam));
            break;

        case WM_KEYUP:
            KeyUpEvent(static_cast<KeyCode>(wParam));
            break;


            // The following WM_XXXX messages report mouse movement and button presses
            // Use KeyHit to get mouse buttons, GetMouseX, GetMouseY for its position
        case WM_MOUSEMOVE:
        {
            MouseMoveEvent(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_LBUTTONDOWN:
        {
            KeyDownEvent(Mouse_LButton);
            break;
        }
        case WM_LBUTTONUP:
        {
            KeyUpEvent(Mouse_LButton);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            KeyDownEvent(Mouse_RButton);
            break;
        }
        case WM_RBUTTONUP:
        {
            KeyUpEvent(Mouse_RButton);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            KeyDownEvent(Mouse_MButton);
            break;
        }
        case WM_MBUTTONUP:
        {
            KeyUpEvent(Mouse_MButton);
            break;
        }


        // Any messages we don't handle are passed back to Windows default handling
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

}