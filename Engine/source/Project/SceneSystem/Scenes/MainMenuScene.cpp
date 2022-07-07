#include "ppch.h"
#include "MainMenuScene.h"

#include "Data/SaveVehicleData.h"


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

			m_PhysicsEntityManager = new EntityManager(m_Renderer, m_PhysicsSystem);
			/*****************************************************/
			/**			   Set up actors and objects            **/
			/*****************************************************/
			// Set Actors and shapes here

			//m_PhysicsEntityManager->CreatePhysicsEntity(/*Name*/, /*EntityMesh*/, /*PhysicsObjectType*/, 
			///*RigidBodyType*/, /*Transform*/, /*CollionSize*/);


		}

		m_VehicleSettings = VehicleSettings();

		m_ChassisMass = m_VehicleSettings.GetChassisMass();
		m_WheelMass = m_VehicleSettings.GetWheelMass();
		m_MaxSteer = m_VehicleSettings.GetMaxSteer();
		m_Diff = m_VehicleSettings.GetDiff();
		m_Engine = m_VehicleSettings.GetEngine();
		m_Gears = m_VehicleSettings.GetGears();
		m_Clutch = m_VehicleSettings.GetClutch();

		m_PeakTorque = m_Engine.mPeakTorque;
		m_RPM = m_Engine.mMaxOmega * 10;
		m_GearSwitchTime = m_Gears.mSwitchTime;
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
			DirectX11Renderer* render = static_cast<DirectX11Renderer*>(m_Renderer);

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
			bool bMapOneBtn = ImGui::Button("HL MAP ONE", { 200, 100 });
			bool bMapTwoBtn = ImGui::Button("HL MAP TWO", { 200, 100 });

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
		if (m_IsOpenWorldSelected && !m_IsHotLapSelected)
		{
			m_IsHotLapSelected = false;
			ImGui::Begin("Open World", nullptr, LoadMapsWinFlags);
			ImGui::SetWindowSize({ 216,253 });
			ImGui::SetWindowPos({ 448,63 });
			ImGui::SetWindowFontScale(2.0f);
			bool bMapOneBtn = ImGui::Button("OW MAP ONE", { 200, 100 });
			bool bMapTwoBtn = ImGui::Button("OW MAP TWO", { 200, 100 });

			if (bMapOneBtn)
			{
				m_IsMapSelected = !m_IsMapSelected;
				m_MapIndex = 3;
			}			
			
			if (m_IsMapSelected)
			{
				VehicleSetup();
			}
			
			ImGui::End();
		}
	}

	void MainMenuScene::VehicleSetup()
	{
		ImGuiWindowFlags VehicleSetupWinFlags = 0;
		//VehicleSetupWinFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;;
		ImGui::Begin("LevelSettings", nullptr, VehicleSetupWinFlags);
		/*ImGui::SetWindowSize({ 216,253 });
		ImGui::SetWindowPos({ 448,63 });*/
		//ImGui::SetWindowFontScale(2.0f);
		
		ImGuiWindowFlags treeFlags = 0;
		treeFlags = ImGuiWindowFlags_NoCollapse;
		
		if (ImGui::TreeNodeEx("Map: ", treeFlags))
		{
			
			//ImGui::Image(m_Image, ImVec2(m_Image_Width / 3, m_Image_Height / 3));
			ImGui::Image(m_LevelImages[m_MapIndex].image, ImVec2(m_LevelImages[m_MapIndex].width/3, m_LevelImages[m_MapIndex].height/3));

			ImGui::TreePop();
			ImGui::Separator();
		}

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
			ImGui::PushItemWidth(50);
			IMGUI_LEFT_LABEL(ImGui::InputInt, "Max Steer:", &m_MaxSteer, 0);
			m_VehicleSettings.SetMaxSteer(m_MaxSteer);
			ImGui::PopItemWidth();
			
			
			
			if(ImGui::TreeNodeEx("Differential: ", 0))
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
				
				m_Diff.mType = (physx::PxVehicleDifferential4WData::Enum)item_current_idx;
				m_VehicleSettings.SetDifferential(m_Diff);
				ImGui::TreePop();
			}


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
			
			if (ImGui::TreeNodeEx("Gears: ", 0))
			{
				// Set Swtich time
				ImGui::PushItemWidth(50);
				IMGUI_LEFT_LABEL(ImGui::InputFloat, "Switch Time:", &m_GearSwitchTime, 0);
				m_Gears.mSwitchTime = m_GearSwitchTime;
				ImGui::PopItemWidth();

				
				m_VehicleSettings.SetGears(m_Gears);
				ImGui::TreePop();
			}

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
			
			if (ImGui::TreeNodeEx("Suspension: ", 0))
			{
				// Set Suspension 
				if (ImGui::TreeNodeEx("Suspension FL: ", 0)) // Front Left
				{
					if (ImGui::TreeNodeEx("Springs: ", 0))
					{
						
						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Compression:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxCompression, 0);
						ImGui::PopItemWidth();
						

						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Droop:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxDroop, 0);
						ImGui::PopItemWidth();


						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Strength:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mSpringStrength, 0);
						ImGui::PopItemWidth();

						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Damper Rate:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mSpringDamperRate, 0);
						ImGui::PopItemWidth();

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Suspension FR: ", 0)) // Front Right 
				{
					if (ImGui::TreeNodeEx("Springs: ", 0))
					{

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Compression:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxCompression, 0);
						ImGui::PopItemWidth();


						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Droop:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxDroop, 0);
						ImGui::PopItemWidth();


						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Strength:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mSpringStrength, 0);
						ImGui::PopItemWidth();

						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Damper Rate:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mSpringDamperRate, 0);
						ImGui::PopItemWidth();

						ImGui::TreePop();
					}
					ImGui::TreePop(); 
				}

				if (ImGui::TreeNodeEx("Suspension RL: ", 0)) // Rear Left
				{
					if (ImGui::TreeNodeEx("Springs: ", 0))
					{

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Compression:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxCompression, 0);
						ImGui::PopItemWidth();


						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Droop:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxDroop, 0);
						ImGui::PopItemWidth();


						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Strength:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mSpringStrength, 0);
						ImGui::PopItemWidth();

						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Damper Rate:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mSpringDamperRate, 0);
						ImGui::PopItemWidth();

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
				
				if (ImGui::TreeNodeEx("Suspension RR: ", 0)) // Rear Right
				{
					if (ImGui::TreeNodeEx("Springs: ", 0))
					{

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Compression:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxCompression, 0);
						ImGui::PopItemWidth();


						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Max Droop:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxDroop, 0);
						ImGui::PopItemWidth();


						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Strength:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mSpringStrength, 0);
						ImGui::PopItemWidth();

						ImGui::SameLine();

						ImGui::PushItemWidth(50);
						IMGUI_LEFT_LABEL(ImGui::InputFloat, "Spring Damper Rate:", &m_Suspension[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mSpringDamperRate, 0);
						ImGui::PopItemWidth();

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
				

				m_VehicleSettings.SetSuspension(m_Suspension, 4);


				ImGui::TreePop();
			}

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

}