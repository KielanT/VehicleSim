#include "ppch.h"
#include "OpenWorldSceneTwo.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{

	OpenWorldSceneTwo::OpenWorldSceneTwo(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour,
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

	OpenWorldSceneTwo::OpenWorldSceneTwo(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex,
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

	}

	bool OpenWorldSceneTwo::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera(true);


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);

		std::string path = "media/";

		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);


			/*****************************************************/
			/**			   Set up actors and objects            **/
			/*****************************************************/
			// Set Actors and shapes here

			m_PhysicsEntityManager = new EntityManager(m_Renderer, m_PhysicsSystem);
			SEntityTransform transform = SEntityTransform();
			transform.Scale = { 2.0f, 0.0f, 2.0f };
			
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Plane", PhysicsStaticObjectType::Plane, path + "Ground.x", transform, { 1.0f, 1.0f, 1.0f }, true, path + "BasicTexGreen.png");
		
			transform.Scale = { 1.0f, 1.0f, 1.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Pitch", PhysicsStaticObjectType::TriangleMesh, path + "Pitch.obj", transform, { 1.0f, 1.0f, 1.0f }, false, path + "BasicTexWhite.png");
			
			transform.Position = { 0.0f, 0.0f, 160.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("GoalOne", PhysicsStaticObjectType::TriangleMesh, path + "GoalOne.obj", transform, { 1.0f, 1.0f, 1.0f }, false, path + "BasicTexYellow.png");
			
			transform.Position = { 0.0f, 0.0f, -160.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("GoalTwo", PhysicsStaticObjectType::TriangleMesh, path + "GoalTwo.obj", transform, { 1.0f, 1.0f, 1.0f }, false, path + "BasicTexYellow.png");
			
			transform.Position = { 0.0f, 0.0f, -30.0f };
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png", transform);
			
			transform.Position = { 0.0f, 0.0f, 0.0f };
			transform.Scale = { 0.3f, 0.3f, 0.3f };
			m_PhysicsEntityManager->CreatePhysicsDynamicEntity("Sphere", PhysicsDynmaicObjectType::Sphere, path + "Sphere.x", transform, { transform.Scale.x * 10, transform.Scale.y, transform.Scale.z }, false, path + "BasicTexBlue.png");
			if (m_PhysicsEntityManager->GetEntity("Sphere") && m_PhysicsEntityManager->GetEntity("Sphere")->GetComponent("PhysicsDynamicObject"));
			{
				PhysicsDynamicObjectComponent* comp = static_cast<PhysicsDynamicObjectComponent*>(m_PhysicsEntityManager->GetEntity("Sphere")->GetComponent("PhysicsDynamicObject"));
				comp->GetActor()->setMass(50.0f);
				comp->GetActor()->setMassSpaceInertiaTensor(physx::PxVec3(50.0f));
			
			}
		}

		return true;
	}

	bool OpenWorldSceneTwo::InitScene()
	{
		if (m_PhysicsEntityManager->GetEntity("MainCar")->GetComponent("VehicleComponent"))
		{
			VehicleComponent* comp = static_cast<VehicleComponent*>(m_PhysicsEntityManager->GetEntity("MainCar")->GetComponent("VehicleComponent"));
			comp->AttachMainCamera(m_SceneCamera);
		}

		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });

		return true;
	}

	void OpenWorldSceneTwo::RenderScene()
	{
		Gui();

		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void OpenWorldSceneTwo::UpdateScene(float frameTime)
	{
		
		
		

		if (!m_IsPaused)
		{

			if (m_EnablePhysics && m_PhysicsEntityManager != nullptr && m_PhysicsSystem != nullptr)
			{
				BallInGoalOne();
				BallInGoalTwo();
			
				m_PhysicsEntityManager->UpdateAllEntities(frameTime);
				m_PhysicsSystem->GetScene()->simulate(frameTime);
				m_PhysicsSystem->GetScene()->fetchResults(true);
			}

			m_SceneCamera->Control(frameTime);

			m_EntityManager->UpdateAllEntities(frameTime);
		}

	}

	void OpenWorldSceneTwo::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	bool OpenWorldSceneTwo::BallInGoalOne()
	{
		if (m_PhysicsEntityManager->GetEntity("Sphere"))
		{
			Entity* entity = m_PhysicsEntityManager->GetEntity("Sphere");
			if (entity->GetComponent("Transform"))
			{
				TransformComponent* comp = static_cast<TransformComponent*>(entity->GetComponent("Transform"));
				if (comp->GetPosition().z > 160.0f && comp->GetPosition().z < 165.0f &&
					comp->GetPosition().x > -12.0f && comp->GetPosition().x < 12.0f)
				{
					m_Score++;
					Reset();
					return true;
				}
			}
		}
		return false;
	}

	bool OpenWorldSceneTwo::BallInGoalTwo()
	{
		if (m_PhysicsEntityManager->GetEntity("Sphere"))
		{
			Entity* entity = m_PhysicsEntityManager->GetEntity("Sphere");
			if (entity->GetComponent("Transform"))
			{
				TransformComponent* comp = static_cast<TransformComponent*>(entity->GetComponent("Transform"));
				if (comp->GetPosition().z < -150.0f && comp->GetPosition().z > -155.0f &&
					comp->GetPosition().x > -12.0f && comp->GetPosition().x < 12.0f)
				{
		
					if(m_Score >= 0) m_Score--;
					Reset();
					return true;
				}
			}
		}
		return false;
	}

	void OpenWorldSceneTwo::Reset()
	{
		if(m_PhysicsEntityManager->GetEntity("MainCar"))
		{
			Entity* entity = m_PhysicsEntityManager->GetEntity("MainCar");
			VehicleComponent* comp = static_cast<VehicleComponent*>(entity->GetComponent("VehicleComponent"));
			comp->Reset();
			comp->GetActor()->setGlobalPose({ 0.0f, 0.0f, -20.0f });

		}
		if (m_PhysicsEntityManager->GetEntity("Sphere") && m_PhysicsEntityManager->GetEntity("Sphere")->GetComponent("PhysicsDynamicObject"));
		{
			PhysicsDynamicObjectComponent* comp = static_cast<PhysicsDynamicObjectComponent*>(m_PhysicsEntityManager->GetEntity("Sphere")->GetComponent("PhysicsDynamicObject"));
			comp->GetActor()->setGlobalPose({ 0.0f, 0.0f, 0.0f });
			comp->GetActor()->setLinearVelocity({ 0.0f, 0.0f, 0.0f });
			comp->GetActor()->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
		}
	}

	void OpenWorldSceneTwo::Gui()
	{
		if (m_PhysicsEntityManager->GetEntity("MainCar"))
		{
			Entity* entity = m_PhysicsEntityManager->GetEntity("MainCar");
			if (entity->GetComponent("VehicleComponent"))
			{
				VehicleComponent* comp = static_cast<VehicleComponent*>(entity->GetComponent("VehicleComponent"));
				comp->GearsUI();
			}
		}

		Scores();

		if (KeyHit(KeyCode::Key_Escape))
		{
			m_IsPaused = !m_IsPaused;
			if (m_IsPaused)
				ImGui::OpenPopup("PauseMenuPopUp");
		}
		PauseMenu();

	}

	void OpenWorldSceneTwo::Scores()
	{
		ImGuiWindowFlags flag = 0;
		flag = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		ImGui::Begin("Scores", nullptr, flag);
		float avail = ImGui::GetContentRegionAvail().x;
		float off = (avail - 160) * 0.5f;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		ImGui::SetWindowFontScale(2.0f);
		ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), " Score: %d", m_Score);


		ImGui::End();
	}

	void OpenWorldSceneTwo::PauseMenu()
	{
		ImGuiWindowFlags popupFlags = 0;
		//popupFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;


		if (ImGui::BeginPopupModal("PauseMenuPopUp", nullptr, popupFlags))
		{
			m_IsPaused = true;
			std::string t = "Paused";
			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth = ImGui::CalcTextSize(t.c_str()).x;
			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::Text(t.c_str());

			ImGui::NewLine();
			ImGui::NewLine();

			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - 80) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button("Resume", ImVec2(80, 0)))
			{
				m_IsPaused = false;
				ImGui::CloseCurrentPopup();
			}
			 avail = ImGui::GetContentRegionAvail().x;
			 off = (avail - 80) * 0.5f;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button("Quit", ImVec2(80, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_sceneManager->LoadScene(0);
			}

			ImGui::EndPopup();
		}
	}



}