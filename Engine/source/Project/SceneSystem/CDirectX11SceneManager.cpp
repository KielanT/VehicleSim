#include "ppch.h"
#include "CDirectX11SceneManager.h"
#include "Utility/Lab/GraphicsHelpers.h"
#include "Graphics/DirectX11/DirectX11Renderer.h"
#include "Project/SceneSystem/TempScenes/TempSceneOne.h"
#include "Project/SceneSystem/TempScenes/TempSceneTwo.h"
#include "Project/SceneSystem/TempScenes/TempSceneThree.h"


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{
	CDirectX11SceneManager::CDirectX11SceneManager(IRenderer* renderer, WindowProperties& props)
	{
		m_Renderer = renderer; // Sets the renderer
		m_Props = props; // Sets the properties

		m_SceneIndex = 0; // Sets the scene index 

		// Creates the scene
		CreateScenesFromFile();
		CreateSceneFromObject(); // TEMP
	}

	CDirectX11SceneManager::~CDirectX11SceneManager()
	{
		delete m_Renderer; // Deletes the renderer

		for (int i = 0; i < m_Scenes.size(); ++i)
			 delete m_Scenes[i]; // Deletes each scene in the renderer
	}

	void CDirectX11SceneManager::Release()
	{
		for (int i = 0; i < m_Scenes.size(); ++i)
		{
			m_Scenes[i]->ReleaseResources();
		}
		
	}

	void CDirectX11SceneManager::CreateScenesFromFile()
	{

	}

	void CDirectX11SceneManager::CreateSceneFromObject()
	{
		//IScene* scene = new TempSceneOne(this, m_Renderer, 0); // Creates a new temp scene
		//auto pos = m_Scenes.begin() + scene->GetSceneIndex(); // Used to add the scene at the correct position
		//m_Scenes.insert(pos, scene); // Adds scene to the array

		//scene = new TempSceneTwo(this, m_Renderer, 1); // Creates a new temp scene
		//pos = m_Scenes.begin() + scene->GetSceneIndex(); // Used to add the scene at the correct position
		//m_Scenes.insert(pos, scene); // Adds scene to the array

		IScene* scene = new TempSceneThree(this, m_Renderer, true, 0); // Creates a new temp scene
		auto pos = m_Scenes.begin() + scene->GetSceneIndex(); // Used to add the scene at the correct position
		m_Scenes.insert(pos, scene); // Adds scene to the array

		scene = new TempSceneOne(this, m_Renderer, true, 1); // Creates a new temp scene
		pos = m_Scenes.begin() + scene->GetSceneIndex(); // Used to add the scene at the correct position
		m_Scenes.insert(pos, scene); // Adds scene to the array

		//scene = new TempSceneTwo(this, m_Renderer, 2); // Creates a new temp scene
		//pos = m_Scenes.begin() + scene->GetSceneIndex(); // Used to add the scene at the correct position
		//m_Scenes.insert(pos, scene); // Adds scene to the array
	}

	bool CDirectX11SceneManager::LoadFirstScene()
	{
		if (m_Scenes[m_SceneIndex]->InitGeometry() && m_Scenes[m_SceneIndex]->InitScene()) // Loads the first scenes 
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void CDirectX11SceneManager::LoadScene(int index)
	{
		m_SceneIndex = index; // Sets the scene index then Initializes the correct scene
		m_Scenes[m_SceneIndex]->InitGeometry();
		m_Scenes[m_SceneIndex]->InitScene();

	}

	void CDirectX11SceneManager::RemoveCurrentScene()
	{
		m_Scenes[m_SceneIndex]->ReleaseResources(); // Releases the current scenes resources
	}

	void CDirectX11SceneManager::RemoveSceneAtIndex(int index)
	{
		m_Scenes[index]->ReleaseResources(); // Releases the scenes resources at index
	}

	void CDirectX11SceneManager::SceneLoop(float frameTime)
	{
		RenderScene(); // Renders the correct scene
		m_Scenes[m_SceneIndex]->UpdateScene(frameTime); // Updates the correct scene
	}

	void CDirectX11SceneManager::RenderScene()
	{
		//IMGUI
		//*******************************
		// Prepare ImGUI for this frame
		//*******************************

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//*******************************

		
		//// Common settings ////
		if (m_Renderer->GetRenderType() == ERendererType::DirectX11) // Checks the correct renderer
		{
			DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer); // Casts the renderer to the correct renderer

			// Sets the correct scene settings
			d11Renderer->PerFrameConstants.ambientColour = m_Scenes[m_SceneIndex]->GetAmbientColour();
			d11Renderer->PerFrameConstants.specularPower = m_Scenes[m_SceneIndex]->GetSpecularPower();
			d11Renderer->PerFrameConstants.cameraPosition = m_Scenes[m_SceneIndex]->GetCamera()->Position();

			//// Main scene rendering ////

			// Set the back buffer as the target for rendering and select the main depth buffer.
			// When finished the back buffer is sent to the "front buffer" - which is the monitor.
			ID3D11RenderTargetView* backBuffer = d11Renderer->GetBackBuffer();
			d11Renderer->GetDeviceContext()->OMSetRenderTargets(1, &backBuffer, d11Renderer->GetDepthStencil());

			// Clear the back buffer to a fixed colour and the depth buffer to the far distance
			ColourRGBA backgroundColour = m_Scenes[m_SceneIndex]->GetBackgroundColour();
			d11Renderer->GetDeviceContext()->ClearRenderTargetView(d11Renderer->GetBackBuffer(), &backgroundColour.r);
			d11Renderer->GetDeviceContext()->ClearDepthStencilView(d11Renderer->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			// Setup the viewport to the size of the main window
			D3D11_VIEWPORT vp;
			vp.Width = static_cast<FLOAT>(m_Props.Width);
			vp.Height = static_cast<FLOAT>(m_Props.Height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			d11Renderer->GetDeviceContext()->RSSetViewports(1, &vp);

			// Render the scene from the main camera
			RenderSceneFromCamera();

			

			ImGui::Render();
			d11Renderer->GetDeviceContext()->OMSetRenderTargets(1, &backBuffer, nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			//// Scene completion ////

			// When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
			// Set first parameter to 1 to lock to vsync (typically 60fps)
			d11Renderer->GetSwapChain()->Present(m_Scenes[m_SceneIndex]->GetVSync() ? 1 : 0, 0);
			
		}
		
	}

	void CDirectX11SceneManager::RenderSceneFromCamera()
	{
		if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
		{
			DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer);
			// Set camera matrices in the constant buffer and send over to GPU
			d11Renderer->PerFrameConstants.viewMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ViewMatrix();
			d11Renderer->PerFrameConstants.projectionMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ProjectionMatrix();
			d11Renderer->PerFrameConstants.viewProjectionMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ViewProjectionMatrix();
			UpdateConstantBuffer(d11Renderer->GetDeviceContext(), d11Renderer->PerFrameConstantBuffer, d11Renderer->PerFrameConstants);

			// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
			d11Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &d11Renderer->PerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
			d11Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &d11Renderer->PerFrameConstantBuffer);

			m_Scenes[m_SceneIndex]->RenderScene();
		}
	}
}