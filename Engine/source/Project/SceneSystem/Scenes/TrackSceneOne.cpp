#include "ppch.h"
#include "TrackSceneOne.h"



#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Data/SaveVehicleData.h"

namespace Project
{

	TrackSceneOne::TrackSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour,
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

	TrackSceneOne::TrackSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex,
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



	bool TrackSceneOne::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera(true);
		m_Timer = new Timer();
		m_Timer->Stop();

		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		std::string path = "media/";
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);

		SaveVehicleData data;
		VehicleSettings vehicleSettings;
		if (!data.LoadVehicleData(vehicleSettings))
		{
			vehicleSettings = VehicleSettings();
		}

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
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Plane", PhysicsStaticObjectType::Plane, path + "Ground.x");
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("TrackOuter", PhysicsStaticObjectType::TriangleMesh, path + "TrackOneOuter.obj", { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, false, path + "brick1.jpg");
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("TrackInner", PhysicsStaticObjectType::TriangleMesh, path + "TrackOneInner.obj", { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, false, path + "brick1.jpg");

			SEntityTransform transform = SEntityTransform();
			transform.Position = { 0.0f, 2.0f, 0.0f };
			transform.Scale = { 10.0f, 10.0f, 2.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("TrackFinishLine", PhysicsStaticObjectType::TriangleMesh, path + "TrackOneFinishLine.obj", { 0.0f, 0.0f, 40.0f }, { 1.0f, 1.0f, 1.0f }, false, path + "BasicTexWhite.png", true, transform);
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", vehicleSettings, path + "Compact/CompactBlue.png");
		}

		return true;
	}

	bool TrackSceneOne::InitScene()
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

	void TrackSceneOne::RenderScene()
	{
		GUI();

		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void TrackSceneOne::UpdateScene(float frameTime)
	{
		bool b = VehicleOverFinishLine();
		if (VehicleOverFinishLine())
		{
			if(!m_Timer->IsRunning())
				m_Timer->Start();
			else
			{
				test++;
				if (test == 1)
				{
					previousTimer = currentTimer;
					if (bestLap > previousTimer || bestLap <= 1)
						bestLap = previousTimer;

					m_Timer->Reset();
					m_Timer->Start();
				}
			}
		}
		else
		{
			test = 0;
		}

		currentTimer = m_Timer->GetTime();

		if (m_EnablePhysics && m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->UpdateAllEntities(frameTime);
			m_PhysicsSystem->GetScene()->simulate(frameTime);
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}

		m_SceneCamera->Control(frameTime);

		m_EntityManager->UpdateAllEntities(frameTime);

	}

	void TrackSceneOne::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	bool TrackSceneOne::VehicleOverFinishLine()
	{
		if (m_PhysicsEntityManager->GetEntity("MainCar"))
		{
			Entity* entity = m_PhysicsEntityManager->GetEntity("MainCar");
			if (entity->GetComponent("Transform"))
			{
				TransformComponent* comp = static_cast<TransformComponent*>(entity->GetComponent("Transform"));
				if (comp->GetPosition().z > 40.0f && comp->GetPosition().z < 40.5f &&
					comp->GetPosition().x > -6.0f && comp->GetPosition().x < 6.0f)
				{
					return true;
				}
				else
					return false;
			}
		}
		return false;
	}

	void TrackSceneOne::GUI()
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

		TimerUI();
	}

	void TrackSceneOne::TimerUI()
	{

		// Current Lap, Previous Lap, best Lap


		//currentMins = currentTimer / 60;
		////currentSeconds = (currentTimer / 60) * 60;
		////currentSeconds += m_Timer->GetLapTime() * 1;

		//ImGui::Begin("Times");
		//ImGui::Text("Best Lap %f", currentTimer);
		//ImGui::Text("Current Lap %d:%f", currentMins, currentSeconds);
		//ImGui::Text("Previous Lap %f", currentSeconds);



		ImGui::Begin("Times");

		if (bestLap <= 0.1)
			ImGui::Text("    Best: --.--");
		else
			ImGui::Text("    Best: %f", bestLap);

		if (currentTimer <= 0.1)
			ImGui::Text(" Current: --.--");
		else
			ImGui::Text(" Current: %f", currentTimer);
		
		if (previousTimer <= 0.1)
			ImGui::Text("Previous: --.--");
		else
			ImGui::Text("Previous: %f", previousTimer);
		ImGui::End();
	}

	
}