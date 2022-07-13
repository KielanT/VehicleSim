#include "ppch.h"
#include "MainMenuScene.h"

#include "Data/SaveVehicleData.h"
#include "Data/ParseWindowSettings.h"



namespace Project
{

	MainMenuScene::MainMenuScene(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour,
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

		m_WindowsSettings = m_Renderer->GetWindowsProperties();
	}

	MainMenuScene::MainMenuScene(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex,
		CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;

		m_EnablePhysics = enablePhysics;

		m_WindowsSettings = m_Renderer->GetWindowsProperties();
	}

	bool MainMenuScene::InitGeometry()
	{
		m_EntityManager = std::make_unique<EntityManager>(m_Renderer);

		m_SceneCamera = std::make_shared<Camera>();

		std::string path = "media/";
	
		
		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", );

		
		


		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);

			m_PhysicsEntityManager = std::make_unique<EntityManager>(m_Renderer, m_PhysicsSystem);
			/*****************************************************/
			/**			   Set up actors and objects            **/
			/*****************************************************/
			// Set Actors and shapes here

			//m_PhysicsEntityManager->CreatePhysicsEntity(/*Name*/, /*EntityMesh*/, /*PhysicsObjectType*/, 
			///*RigidBodyType*/, /*Transform*/, /*CollionSize*/);


		}

		m_VehicleSettings = VehicleSettings();

		m_IsAuto = m_VehicleSettings.GetAuto();
		m_ChassisMass = m_VehicleSettings.GetChassisMass();
		m_WheelMass = m_VehicleSettings.GetWheelMass();
		m_MaxSteer = m_VehicleSettings.GetMaxSteer();
		m_Diff = m_VehicleSettings.GetDiff();
		m_Engine = m_VehicleSettings.GetEngine();
		m_Gears = m_VehicleSettings.GetGears();
		m_Clutch = m_VehicleSettings.GetClutch();
		m_FrontAntiStiffness = m_VehicleSettings.GetFrontAntiRollBarStiffness();
		m_RearAntiStiffness = m_VehicleSettings.GetRearAntiRollBarStiffness();
		m_ToeAngle = m_VehicleSettings.GetToeAngle();
		

		m_PeakTorque = m_Engine.mPeakTorque;
		m_RPM = m_Engine.mMaxOmega * 10;
		m_GearSwitchTime = m_Gears.mSwitchTime;
		m_NumGears = (int)m_Gears.mFinalRatio;
		m_ClutchStrength = m_Clutch.mStrength;

		for (int i = 0; i < 4; ++i)
		{
			m_Suspension[i] = m_VehicleSettings.GetSuspension(i);
		}
		


		/*****************************************************/
		/**			   Load images		                    **/
		/*****************************************************/

		if (m_Renderer->GetRenderType() == ERendererType::DirectX11)
		{
			std::shared_ptr<DirectX11Renderer> render = std::static_pointer_cast<DirectX11Renderer>(m_Renderer);

			if (render != nullptr)
			{
				for (int i = 0; i < LEVEL_IMAGE_COUNT; ++i)
				{
					std::string fileType = ".PNG";
					std::string image = "media/LevelImages/Level" + std::to_string(i) + fileType;
					bool success = render->LoadTextureFromFile(image.c_str(), &m_LevelImages[i].image, &m_LevelImages[i].width, &m_LevelImages[i].height);
					IM_ASSERT(success);
				}


				bool b = render->LoadTextureFromFile("media/CarImageOne.PNG", &m_CarImage, &m_CarImage_Width, &m_CarImage_Height);
				IM_ASSERT(b);
			}
		}
		return true;
	}

	bool MainMenuScene::InitScene()
	{
		PlayerSettings settings;
		PlayerControls controls;

		if(settings.LoadPlayerControls(controls))
			m_Controls = controls;
		else
			m_Controls = PlayerControls();

		SetDefaultControls();

		windSize[0].width = 1280; windSize[0].height = 720; windSize[0].winString = std::to_string(windSize[0].width) + " x " + std::to_string(windSize[0].height);
		windSize[1].width = 1920; windSize[1].height = 1080; windSize[1].winString = std::to_string(windSize[1].width) + " x " + std::to_string(windSize[1].height);

		window[0] = windSize[0].winString.c_str();
		window[1] = windSize[1].winString.c_str();

		for (int i = 0; i < 2; ++i)
		{
			if (m_WindowsSettings.Width == windSize[i].width && m_WindowsSettings.Height == windSize[i].height)
			{
				currentWindowIndex = i;
				break;
			}
		}

		m_WindowMode[0] = "Windowed";
		m_WindowMode[1] = "Fullscreen Bordeless";
		m_CurrentWindowModeIndex = static_cast<int>(m_WindowsSettings.windowType);


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

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	void MainMenuScene::GUI()
	{
		MainMenu();
		//ImGui::ShowDemoWindow();
		if (m_ShowGameModeSelect) GameMode();
		if(m_SettingsMenu) SettingsMenu();
	}

	void MainMenuScene::MainMenu()
	{
		ImGuiWindowFlags ButtonWinFlags = 0;
		ButtonWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGui::Begin("Main Menu", nullptr, ButtonWinFlags);
		ImGui::SetWindowSize({ 219, 400 });
		ImGui::SetWindowPos({ 0, 14 });
		ImGui::SetWindowFontScale(3.0f);
		bool bPlayBtn = ImGui::Button("Play", { 200, 100 });
		bool bSettingsBtn = ImGui::Button("Settings", { 200, 100 });
		bool bQuitBtn = ImGui::Button("Quit", { 200, 100 });
		ImGui::End();

		if (bPlayBtn)
		{
			m_SettingsMenu = false;
			m_ShowGameModeSelect = !m_ShowGameModeSelect;

			if (!m_ShowGameModeSelect)
			{
				m_IsHotLapSelected = false;
				m_IsOpenWorldSelected = false;
				m_IsMiniGamesSelected = false;
			}
		}

		if (bSettingsBtn)
		{
			m_ShowGameModeSelect = false;
			m_SettingsMenu = !m_SettingsMenu;
		}

		if (bQuitBtn)
		{
			DestroyWindow(m_Renderer->GetWindowsProperties().Hwnd);
		}
	}

	void MainMenuScene::GameMode()
	{
		ImGuiWindowFlags GameModeWinFlags = 0;
		GameModeWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Game Modes", nullptr, GameModeWinFlags);
		ImGui::SetWindowSize({ 222,500 });
		ImGui::SetWindowPos({ 220, 48 });
		ImGui::SetWindowFontScale(2.0f);
		bool bHotLapBtn = ImGui::Button("Hot Laps", { 200, 100 });
		bool bOepnWorldBtn = ImGui::Button("Open World", { 200, 100 });
		bool bGamesBtn = ImGui::Button("Mini Games", { 200, 100 });


		if (bHotLapBtn)
		{
			m_IsHotLapSelected = !m_IsHotLapSelected;
			if (m_IsHotLapSelected)
			{
				m_IsOpenWorldSelected = false;
				m_IsMiniGamesSelected = false;
			}
		}
		else if (bOepnWorldBtn)
		{
			m_IsOpenWorldSelected = !m_IsOpenWorldSelected;
			if (m_IsOpenWorldSelected)
			{
				m_IsHotLapSelected = false;
				m_IsMiniGamesSelected = false;
			}
		}
		else if(bGamesBtn)
		{
			m_IsMiniGamesSelected = !m_IsMiniGamesSelected;
			if (m_IsMiniGamesSelected)
			{
				m_IsOpenWorldSelected = false;
				m_IsHotLapSelected = false;
			}
		}
		LoadMaps();

		ImGui::End();
	}

	void MainMenuScene::LoadMaps()
	{
		ImGuiWindowFlags LoadMapsWinFlags = 0;
		LoadMapsWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;;
		if (m_IsHotLapSelected)
		{
			m_IsOpenWorldSelected = false;
			m_IsMiniGamesSelected = false;
			ImGui::Begin("Hotlap Maps", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowFontScale(2.0f);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			bool bMapOneBtn = ImGui::Button("Short Track", { 200, 100 });
			bool bMapTwoBtn = ImGui::Button("Long Track", { 200, 100 });

			ImGui::End();

			if (bMapOneBtn)
			{
				m_IsMapSelected = !m_IsMapSelected;
				m_MapIndex = 1;
			}
			if (bMapTwoBtn)
			{
				m_IsMapSelected = !m_IsMapSelected;
				m_MapIndex = 2;
			}

			if (m_IsMapSelected)
			{
				VehicleSetup();
			}

		}
		if (m_IsOpenWorldSelected)
		{
			m_IsHotLapSelected = false;
			m_IsMiniGamesSelected = false;
			ImGui::Begin("Open World", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			ImGui::SetWindowFontScale(2.0f);
			bool bMapOneBtn = ImGui::Button("OW MAP ONE", { 200, 100 });
			//bool bMapTwoBtn = ImGui::Button("OW MAP TWO", { 200, 100 });

			ImGui::End();
			if (bMapOneBtn)
			{
				m_IsMapSelected = !m_IsMapSelected;
				m_MapIndex = 3;
			}

			if (m_IsMapSelected)
			{
				VehicleSetup();
			}
			
		}

		if (m_IsMiniGamesSelected)
		{
			m_IsHotLapSelected = false;
			m_IsOpenWorldSelected = false;
			ImGui::Begin("MiniGames", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			ImGui::SetWindowFontScale(2.0f);
			bool bMapOneBtn = ImGui::Button("Car Ball", { 200, 100 });
			//bool bMapTwoBtn = ImGui::Button("OW MAP TWO", { 200, 100 });

			ImGui::End();
			if (bMapOneBtn)
			{
				m_IsMapSelected = !m_IsMapSelected;
				m_MapIndex = 4;
			}

			if (m_IsMapSelected)
			{
				VehicleSetup();
			}
		}
	}

	void MainMenuScene::VehicleSetup()
	{
		ImGuiWindowFlags VehicleSetupWinFlags = 0;
		VehicleSetupWinFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Level Settings", nullptr, VehicleSetupWinFlags);
		ImGui::SetWindowSize({ 597,701 });
		ImGui::SetWindowPos({ 670,15 });
		
		ImGuiWindowFlags treeFlags = 0;
		treeFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMouseInputs | ImGuiTreeNodeFlags_Leaf;

		if (ImGui::TreeNodeEx("Map: ", treeFlags))
		{
			ImGui::Unindent();
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - m_LevelImages[m_MapIndex].width / 3) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			ImGui::Image(m_LevelImages[m_MapIndex].image, ImVec2(m_LevelImages[m_MapIndex].width/3, m_LevelImages[m_MapIndex].height/3));

			ImGui::TreePop();
			ImGui::Separator();
		}
		ImGui::Indent();
		if (ImGui::TreeNodeEx("Vehicle Select: ", treeFlags))
		{
			ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;



			ImGui::BeginChild("ScrollBar", ImVec2(-100, 100), true, child_flags);
			
			auto pos = ImGui::GetCursorPos();
			for (int n = 0; n < 1; n++) // Only have one vehicle for now
			{
				
				ImGui::PushID(n);

				char buf[32];
				sprintf(buf, "##Object %d", n); // Sets the id of the selectable but doesn't show the label on the selectable

				ImGui::SetCursorPos(ImVec2(pos.x, pos.y));

				ImGuiSelectableFlags flags = 0;
				if (ImGui::Selectable(buf, n == m_Selected, flags, ImVec2(m_CarImage_Width / 6, m_CarImage_Height / 6 )))
				{
					m_Selected = n;
				}
				if (!ImGui::IsItemHovered() && n == m_Selected)
				{
					SelectableColor(IM_COL32(255, 0, 0, 200));
				}

				ImGui::SetItemAllowOverlap();

				ImGui::SetCursorPos(ImVec2(pos.x, pos.y));
				ImGui::Image(m_CarImage, ImVec2(m_CarImage_Width / 6, m_CarImage_Height / 6));

				pos.x += 160;
				
				ImGui::PopID();
				
			}
			ImGui::EndChild();

			
			ImGui::TreePop();
			ImGui::Separator();

		}
		
		if (ImGui::TreeNodeEx("Vehicle Setup: ", treeFlags))
		{
			// Set Chassis Mass
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::Checkbox, "Automatic:", &m_IsAuto);
			m_VehicleSettings.SetAutomatic(m_IsAuto);
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Chassis Mass:", &m_ChassisMass, 0);
			m_VehicleSettings.SetChassisMass(m_ChassisMass);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			
			// Set WheelMass
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Wheel Mass:", &m_WheelMass, 0);
			m_VehicleSettings.SetWheelMass(m_WheelMass);
			ImGui::PopItemWidth();

			ImGui::SameLine();

			// Set Max Steer
			ImGui::BeginDisabled(true);
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Max Steer:", &m_MaxSteer, 0);
			m_VehicleSettings.SetMaxSteer(m_MaxSteer);
			ImGui::PopItemWidth();
			ImGui::EndDisabled();
			

			
			TireSettings();

			DifferentialSettings();

			EngineSettings();
			
			GearSettings();
			
			ClutchSettings();
			
			SuspensionSettings();
			

			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNodeEx("Go ", treeFlags))
		{
			ImGui::SetWindowFontScale(2.0f);
			bool Go = ImGui::Button("Go", { 100, 100 });
			ImGui::SetWindowFontScale(1.0f);

			SaveVehicleData data;
			
			if (Go && m_MapIndex != 0)
			{
				data.SaveVehicleDataToFile(m_VehicleSettings);
				m_sceneManager->LoadScene(m_MapIndex);
			}
				//if (Go) m_sceneManager->LoadScene(1);

			ImGui::TreePop();
			ImGui::Separator();
		}
		ImGui::End();
	}

	void MainMenuScene::TireSettings()
	{
		if (ImGui::TreeNodeEx("Tires: ", 0))
		{
			const char* items[MAX_NUM_TIRE_TYPES];
			items[0] = "Normal";
			items[1] = "Worn";
			items[2] = "Wets";
			items[3] = "Slicks";
			items[4] = "Ice";
			items[5] = "MUD";

			static int item_current_idx = 0;
			const char* combo_preview_value = items[item_current_idx];
			ImGui::PushItemWidth(310);
			if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "Type: ", combo_preview_value))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n], is_selected))
						item_current_idx = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			m_VehicleSettings.SetTires(item_current_idx);

			ImGui::BeginDisabled(true);
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputFloat, "FL Toe:", &m_ToeAngle.frontLeftAngle, 0);
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputFloat, "FR Toe:", &m_ToeAngle.frontRightAngle, 0);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputFloat, "RL Toe:", &m_ToeAngle.rearLeftAngle, 0);
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputFloat, "RR Toe:", &m_ToeAngle.rearRightAngle, 0);
			ImGui::PopItemWidth();
			m_VehicleSettings.SetToeAngle(m_ToeAngle);
			ImGui::EndDisabled();

			ImGui::TreePop();
		}
	}

	void MainMenuScene::DifferentialSettings()
	{
		if (ImGui::TreeNodeEx("Differential: ", 0))
		{
			const char* items[physx::PxVehicleDifferential4WData::eMAX_NB_DIFF_TYPES];
			items[0] = "Limited Slip for 4 wheel drive";
			items[1] = "Limted Slip for front wheel drive";
			items[2] = "Limited Slip for rear wheel drive";
			items[3] = "Open Differential for 4 wheel drive";
			items[4] = "Open Differential for 4 front wheel drive";
			items[5] = "Open Differential for 4 rear wheel drive";

			static int item_current_idx = 0;
			const char* combo_preview_value = items[item_current_idx];
			ImGui::PushItemWidth(310);
			if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "Type: ", combo_preview_value))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n], is_selected))
						item_current_idx = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();


			ImGui::Unindent();
			if (ImGui::TreeNodeEx("Advanced: ", 0))
			{
				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Front Bias:", &m_Diff.mFrontBias, 0);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rear Bias: ", &m_Diff.mRearBias, 0);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Centre Bias:", &m_Diff.mCentreBias, 0);
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Front Rear Split:", &m_Diff.mFrontRearSplit, 0);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Front Left Front Split:", &m_Diff.mFrontLeftRightSplit, 0);
				ImGui::PopItemWidth();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rear Left Front Split:", &m_Diff.mRearLeftRightSplit, 0);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			m_Diff.mType = (physx::PxVehicleDifferential4WData::Enum)item_current_idx;
			m_VehicleSettings.SetDifferential(m_Diff);
			ImGui::TreePop();
		}
	}

	void MainMenuScene::EngineSettings()
	{
		if (ImGui::TreeNodeEx("Engine: ", 0))
		{
			// Set Engine Peak Torque
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Peak Torque:", &m_PeakTorque, 0);
			m_Engine.mPeakTorque = m_PeakTorque;
			ImGui::PopItemWidth();

			ImGui::SameLine();

			// Set Engine Max RPM
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Max RPM:", &m_RPM, 0);
			m_Engine.mMaxOmega = m_RPM / 10;
			ImGui::PopItemWidth();

			m_VehicleSettings.SetEngine(m_Engine);
			
			ImGui::TreePop();
		}
	}

	void MainMenuScene::GearSettings()
	{
		if (ImGui::TreeNodeEx("Gears: ", 0))
		{
			// Set Swtich time
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputFloat, "Switch Time:", &m_GearSwitchTime, 0);
			m_Gears.mSwitchTime = m_GearSwitchTime;
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::BeginDisabled(true);
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Number of gears:", &m_NumGears, 0);
			ImGui::PopItemWidth();
			ImGui::EndDisabled();

			m_VehicleSettings.SetGears(m_Gears);
			ImGui::TreePop();
		}
	}

	void MainMenuScene::ClutchSettings()
	{
		if (ImGui::TreeNodeEx("Clutch: ", 0))
		{
			// Set Clutch strength
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Strength:", &m_ClutchStrength, 0);
			m_Clutch.mStrength = m_ClutchStrength;
			ImGui::PopItemWidth();


			m_VehicleSettings.SetClutch(m_Clutch);

			ImGui::TreePop();
		}
	}

	void MainMenuScene::SuspensionSettings()
	{
		if (ImGui::TreeNodeEx("Suspension: ", 0))
		{
			// Set Suspension 
			SuspensionSettingHelper("Suspension FL: ", physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT);
			SuspensionSettingHelper("Suspension FR: ", physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT);
			SuspensionSettingHelper("Suspension RL: ", physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT);
			SuspensionSettingHelper("Suspension RR: ", physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT);

			if (ImGui::TreeNodeEx("Antiroll bar: ", 0))
			{
				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Front Stiffness:", &m_FrontAntiStiffness, 0);
				m_VehicleSettings.SetFrontAntiRollBarStiffness(m_FrontAntiStiffness);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Rear Stiffness:", &m_RearAntiStiffness, 0);
				m_VehicleSettings.SetRearAntiRollBarStiffness(m_RearAntiStiffness);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			m_VehicleSettings.SetSuspension(m_Suspension, 4);


			ImGui::TreePop();
		}
	}

	void MainMenuScene::SuspensionSettingHelper(std::string nodeName, physx::PxVehicleDrive4WWheelOrder::Enum wheelNum)
	{
		if (ImGui::TreeNodeEx(nodeName.c_str(), 0)) 
		{
			if (ImGui::TreeNodeEx("Springs: ", 0))
			{

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Compression:", &m_Suspension[wheelNum].mMaxCompression, 0);
				ImGui::PopItemWidth();


				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Droop:", &m_Suspension[wheelNum].mMaxDroop, 0);
				ImGui::PopItemWidth();


				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Strength:", &m_Suspension[wheelNum].mSpringStrength, 0);
				ImGui::PopItemWidth();

				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Damper Rate:", &m_Suspension[wheelNum].mSpringDamperRate, 0);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Camber: ", 0))
			{
				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Camber At Rest:", &m_Suspension[wheelNum].mCamberAtRest, 0);
				ImGui::PopItemWidth();


				ImGui::SameLine();

				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Camber At Max Droop:", &m_Suspension[wheelNum].mCamberAtMaxDroop, 0);
				ImGui::PopItemWidth();


				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Camber At Max Compression:", &m_Suspension[wheelNum].mCamberAtMaxCompression, 0);
				ImGui::PopItemWidth();


				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

	void MainMenuScene::SettingsMenu()
	{
		ImGuiWindowFlags SettingsWinFlags = 0;
		SettingsWinFlags = /*ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |*/ ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;
		ImGui::Begin("SettingsMenu", nullptr, SettingsWinFlags);

		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Controls"))
			{
				ControlSettings();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Window"))
			{
				WindowSettings();
				ImGui::EndTabItem();
			}
			//if (ImGui::BeginTabItem("Camera"))
			//{
			//	
			//	ImGui::EndTabItem();
			//}
			ImGui::EndTabBar();
		}
		
		ImGui::End();
	}

	void MainMenuScene::ControlSettings()
	{
		ImGui::NewLine();

		// Accelerate
		ButtonChangeInputText(" Accelerate: ", m_AcclerateBuffer, 1, m_Controls.accelerate);
		ImGui::NewLine();

		// Steer Right
		ButtonChangeInputText("Steer Right: ", m_SteerRightBuffer, 2, m_Controls.steerRight);
		ImGui::NewLine();

		// Steer Left
		ButtonChangeInputText(" Steer Left: ", m_SteerLeftBuffer, 3, m_Controls.steerLeft);
		ImGui::NewLine();

		// Brake
		ButtonChangeInputText("      Brake: ", m_BrakeBuffer, 4, m_Controls.brake);
		ImGui::NewLine();

		// Gear up
		ButtonChangeInputText("    Gear up: ", m_GearUpBuffer, 5, m_Controls.gearUp);
		ImGui::NewLine();
		
		// Gear Down
		ButtonChangeInputText("  Gear Down: ", m_GearDownBuffer, 6, m_Controls.gearDown);
		ImGui::NewLine();

		// Gear Down
		ButtonChangeInputText(" Hand Brake: ", m_HandBrakeBuffer, 7, m_Controls.handBrake);
		ImGui::NewLine();

		// Gear Down
		ButtonChangeInputText("      Reset: ", m_ResetBuffer, 8, m_Controls.reset);
		ImGui::NewLine();
		

		bool b = ImGui::Button("Apply", ImVec2(80, 30));
		if (b)
		{
			PlayerSettings settings;
			settings.SavePlayerControlsToFile(m_Controls);
		}

	}

	void MainMenuScene::WindowSettings()
	{
		ImGui::NewLine();
		windowPreviewValue = window[currentWindowIndex];

		ImGui::PushItemWidth(310);
		if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "Window Size: ", windowPreviewValue))
		{
			for (int n = 0; n < IM_ARRAYSIZE(window); n++)
			{
				const bool is_selected = (currentWindowIndex == n);
				if (ImGui::Selectable(window[n], is_selected))
					currentWindowIndex = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::NewLine();

		m_WindowsSettings.Height = windSize[currentWindowIndex].height;
		m_WindowsSettings.Width = windSize[currentWindowIndex].width;

		m_WindowModePreviewValue = m_WindowMode[m_CurrentWindowModeIndex];
		ImGui::PushItemWidth(310);
		if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "Window Mode: ", m_WindowModePreviewValue))
		{
			for (int n = 0; n < IM_ARRAYSIZE(m_WindowMode); n++)
			{
				const bool is_selected = (currentWindowIndex == n);
				if (ImGui::Selectable(m_WindowMode[n], is_selected))
					m_CurrentWindowModeIndex = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			m_WindowsSettings.windowType = static_cast<WindowType>(m_CurrentWindowModeIndex);

			ImGui::EndCombo();
		}
		ImGui::NewLine();

		const char* renderType[1];
		renderType[0] = "DirectX 11";

		m_Renderer->GetWindowsProperties().Width;
		static int currentRenderTypeIndex = 0;
		ImGui::BeginDisabled();
		const char* renderTypePreviewValue = renderType[currentRenderTypeIndex];
		ImGui::PushItemWidth(310);
		if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "   Renderer: ", renderTypePreviewValue))
		{
			for (int n = 0; n < IM_ARRAYSIZE(renderType); n++)
			{
				const bool is_selected = (currentRenderTypeIndex == n);
				if (ImGui::Selectable(renderType[n], is_selected))
					currentRenderTypeIndex = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::NewLine();

		const char* physicsType[1];
		physicsType[0] = "PhysX 4";

		m_Renderer->GetWindowsProperties().Width;
		static int currentPhysicsTypeIndex = 0;
		
		const char* physicsTypePreviewValue = physicsType[currentRenderTypeIndex];
		ImGui::PushItemWidth(310);
		if (IMGUI_LEFT_LABEL(ImGui::BeginCombo, "    Physics: ", physicsTypePreviewValue))
		{
			for (int n = 0; n < IM_ARRAYSIZE(physicsType); n++)
			{
				const bool is_selected = (currentPhysicsTypeIndex == n);
				if (ImGui::Selectable(physicsType[n], is_selected))
					currentPhysicsTypeIndex = n;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::EndDisabled();

		ImGui::NewLine();

		bool b = ImGui::Button("Apply", ImVec2(80, 30));

		if (b)
		{
			
			ImGui::OpenPopup("MessageBox");
		}
		PopUpWindow("Changes Will Be Applied Next Launch");

	}

	void MainMenuScene::PopUpWindow(std::string message)
	{
		ImGuiWindowFlags popupFlags = 0;
		popupFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		
		
		if (ImGui::BeginPopupModal("MessageBox", nullptr, popupFlags))
		{
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(message.c_str()).x;
			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::Text(message.c_str());

			ImGui::NewLine();
			ImGui::NewLine();

			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - 80) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button("OK", ImVec2(80, 0)))
			{
				ParseWindowSettings settings;
				settings.SaveWindowSettings(m_WindowsSettings);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void MainMenuScene::SetDefaultControls()
	{
		m_AcclerateBuffer = KeyValueToString(m_Controls.accelerate);
		m_SteerRightBuffer = KeyValueToString(m_Controls.steerRight);
		m_SteerLeftBuffer = KeyValueToString(m_Controls.steerLeft);
		m_BrakeBuffer = KeyValueToString(m_Controls.brake);
		m_GearUpBuffer = KeyValueToString(m_Controls.gearUp);
		m_GearDownBuffer = KeyValueToString(m_Controls.gearDown);
		m_HandBrakeBuffer = KeyValueToString(m_Controls.handBrake);
		m_ResetBuffer = KeyValueToString(m_Controls.reset);
	}

	void MainMenuScene::ButtonChangeInputText(std::string s, std::string& keyBuffer, int id, KeyCode& keycode)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiInputTextFlags flag = 0;
		flag = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_NoHorizontalScroll;

		char a[128];
		std::strcpy(a, keyBuffer.c_str());

		ImGui::Text(s.c_str()); ImGui::SameLine();

		ImGui::PushID(id);  ImGui::PushItemWidth(50.0f);
		ImGui::InputText("##", a, 128, flag);
		ImGui::PopID(); ImGui::PopItemWidth();

		if(ImGui::IsItemActive())
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		{
			if (ImGui::IsKeyDown(i))
			{
				std::string s = std::to_string(i);
				const char* c = s.c_str();
				keyBuffer = KeyValueToString(i);

				keycode = static_cast<KeyCode>(i);
			}
		}
	}

}