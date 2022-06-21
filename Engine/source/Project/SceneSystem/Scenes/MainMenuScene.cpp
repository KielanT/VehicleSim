#include "ppch.h"
#include "MainMenuScene.h"


#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{

	MainMenuScene::MainMenuScene(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour,
		float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;

		m_EnablePhysics = false;
	}

	MainMenuScene::MainMenuScene(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex,
		CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;

	}

	bool MainMenuScene::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();


		/*****************************************************/
		/**			   Create the entities                  **/
		std::string path = "media/";
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", );




		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

			m_PhysicsEntityManager = new EntityManager(m_Renderer, m_PhysicsSystem);
			/*****************************************************/
			/**			   Set up actors and objects            **/
			/*****************************************************/
			// Set Actors and shapes here

			//m_PhysicsEntityManager->CreatePhysicsEntity(/*Name*/, /*EntityMesh*/, /*PhysicsObjectType*/, 
			///*RigidBodyType*/, /*Transform*/, /*CollionSize*/);


		}

		return true;
	}

	bool MainMenuScene::InitScene()
	{
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });

		return true;
	}

	void MainMenuScene::RenderScene()
	{
		GUI();

		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void MainMenuScene::UpdateScene(float frameTime)
	{

		if (m_EnablePhysics && m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->UpdateAllEntities(frameTime);
			m_PhysicsSystem->GetScene()->simulate(frameTime);
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}


		m_EntityManager->UpdateAllEntities(frameTime);

	}

	void MainMenuScene::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	void MainMenuScene::GUI()
	{
		MainMenu();

		if (m_ShowGameModeSelect) GameMode();


	}

	void MainMenuScene::MainMenu()
	{
		ImGuiWindowFlags ButtonWinFlags = 0;
		ButtonWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGui::Begin("Main Menu", nullptr, ButtonWinFlags);
		ImGui::SetWindowSize({ 219, 300 });
		ImGui::SetWindowPos({ 0, 14 });
		ImGui::SetWindowFontScale(5.0f);
		bool bPlayBtn = ImGui::Button("Play", { 200, 100 });
		bool bQuitBtn = ImGui::Button("Quit", { 200, 100 });
		ImGui::End();

		if (bPlayBtn) m_ShowGameModeSelect = !m_ShowGameModeSelect;


		if (bQuitBtn)
			DestroyWindow(m_Renderer->GetWindowsProperties().Hwnd);
	}

	void MainMenuScene::GameMode()
	{
		ImGuiWindowFlags GameModeWinFlags = 0;
		GameModeWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;;
		ImGui::Begin("Game Modes", nullptr, GameModeWinFlags);
		ImGui::SetWindowSize({ 222,252 });
		ImGui::SetWindowPos({ 220, 48 });
		ImGui::SetWindowFontScale(2.0f);
		bool bHotLapBtn = ImGui::Button("Hot Laps", { 200, 100 });
		bool bOepnWorldBtn = ImGui::Button("Open World", { 200, 100 });


		if (bHotLapBtn) { m_IsHotLapSelected = !m_IsHotLapSelected; m_IsOpenWorldSelected = !m_IsHotLapSelected; }
		else if (bOepnWorldBtn) { m_IsOpenWorldSelected = !m_IsOpenWorldSelected; m_IsHotLapSelected = !m_IsOpenWorldSelected; }
		LoadMaps();

		ImGui::End();
	}

	void MainMenuScene::LoadMaps()
	{
		ImGuiWindowFlags LoadMapsWinFlags = 0;
		LoadMapsWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;;
		if (m_IsHotLapSelected && !m_IsOpenWorldSelected)
		{
			ImGui::Begin("Hotlap Maps", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowFontScale(2.0f);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			bool bMapOneBtn = ImGui::Button("HP MAP ONE", { 200, 100 });
			bool bMapTwoBtn = ImGui::Button("HP MAP TWO", { 200, 100 });

			ImGui::End();

			if (bMapOneBtn) m_sceneManager->LoadScene(1);
			if (bMapTwoBtn) m_sceneManager->LoadScene(2);

		}
		if (m_IsOpenWorldSelected && !m_IsHotLapSelected)
		{
			m_IsHotLapSelected = false;
			ImGui::Begin("Open World", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			ImGui::SetWindowFontScale(2.0f);
			bool bHotLapBtn = ImGui::Button("OP MAP ONE", { 200, 100 });
			bool bOepnWorldBtn = ImGui::Button("OP MAP TWO", { 200, 100 });
			ImGui::End();
		}
	}

}