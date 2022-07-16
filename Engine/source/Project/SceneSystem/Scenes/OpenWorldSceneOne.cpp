#include "ppch.h"
#include "OpenWorldSceneOne.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Data/SaveVehicleData.h"

namespace Project
{

	OpenWorldSceneOne::OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour,
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

	OpenWorldSceneOne::OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex,
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

	bool OpenWorldSceneOne::InitGeometry()
	{
		m_EntityManager = std::make_unique<EntityManager>(m_Renderer);

		m_SceneCamera = std::make_shared<Camera>(true);


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);

		std::string path = "media/";

		SaveVehicleData data;
		VehicleSettings vehicleSettings;
		if (!data.LoadVehicleData(vehicleSettings))
		{
			vehicleSettings = VehicleSettings();
		}
		m_IsPaused = false;
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

			m_PhysicsEntityManager = std::make_unique<EntityManager>(m_Renderer, m_PhysicsSystem);
			SEntityTransform transform = SEntityTransform();
			transform.Scale = { 2.0f, 0.0f, 2.0f };

			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Plane", PhysicsStaticObjectType::Plane, path + "Ground.x", transform, {1.0f, 1.0f, 1.0f}, true, path + "BasicTexOrange.png");
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/MainCompact.obj", path + "Compact/CollisionCompact.obj", vehicleSettings, path + "Compact/CompactBlue.png");

			transform.Scale = { 1.0f, 1.0f, 1.0f };
			transform.Position = { 0.0f, 0.0f, 200.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("RampOne", PhysicsStaticObjectType::TriangleMesh, path + "Ramp.obj", transform, transform.Scale, true, path + "BasicTexBlue.png");
			
			transform.Position = { 30.0f, -5.0f, 200.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("RampTwo", PhysicsStaticObjectType::TriangleMesh, path + "Ramp.obj", transform, transform.Scale, true, path + "BasicTexBlue.png");
			
			transform.Position = { 30.0f, -5.0f, -700.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("RampThree", PhysicsStaticObjectType::TriangleMesh, path + "DoubleRampR45.obj", transform, transform.Scale, true, path + "BasicTexBlue.png");

			transform.Position = { 50.0f, -5.0f, 200.0f };
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("RampFour", PhysicsStaticObjectType::TriangleMesh, path + "DoubleRamp.obj", transform, transform.Scale, true, path + "BasicTexBlue.png");
		
			CreateDynamicCubes(CUBE_DYNAMIC_AMOUNT);
			CreateStaticCubes(CUBE_STATIC_AMOUNT);
			CreateDynamicSpheres(SPHERE_DYNAMIC_AMOUNT);
			CreateStaticSpheres(SPHERE_STATIC_AMOUNT);
		}

		return true;
	}

	bool OpenWorldSceneOne::InitScene()
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

	void OpenWorldSceneOne::RenderScene()
	{
		GUI();


		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void OpenWorldSceneOne::UpdateScene(float frameTime)
	{
		if (!m_IsPaused)
		{
			if (m_EnablePhysics && m_PhysicsEntityManager != nullptr)
			{
				m_PhysicsEntityManager->UpdateAllEntities(frameTime);
				m_PhysicsSystem->GetScene()->simulate(frameTime);
				m_PhysicsSystem->GetScene()->fetchResults(true);
			}

			m_SceneCamera->Control(frameTime);

			m_EntityManager->UpdateAllEntities(frameTime);
		}
	}

	void OpenWorldSceneOne::ReleaseResources()
	{
		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	void OpenWorldSceneOne::CreateDynamicCubes(int amount)
	{
		static std::string path = "media/";
		SEntityTransform transform = SEntityTransform();

		for (int i = 0; i < amount; ++i)
		{
			transform.Position = m_CubedDynamicSettings[i].position;
			transform.Rotation = m_CubedDynamicSettings[i].rotation;
			transform.Scale = m_CubedDynamicSettings[i].scale;
			m_PhysicsEntityManager->CreatePhysicsDynamicEntity("Cube" + i, PhysicsDynmaicObjectType::Box, path + "Cube.x", transform, { transform.Scale.x * 5, transform.Scale.y * 5, transform.Scale.z * 5 }, false, path + "BasicTexYellow.png");
			if (m_PhysicsEntityManager->GetEntity("Cube" + i) && m_PhysicsEntityManager->GetEntity("Cube" + i)->GetComponent("PhysicsDynamicObject"));
			{
				PhysicsDynamicObjectComponent* comp = static_cast<PhysicsDynamicObjectComponent*>(m_PhysicsEntityManager->GetEntity("Cube" + i)->GetComponent("PhysicsDynamicObject"));
				comp->GetActor()->setMass(m_CubedDynamicSettings[i].mass);
				comp->GetActor()->setMassSpaceInertiaTensor(physx::PxVec3(m_CubedDynamicSettings[i].mass));

			}
		}
	}

	void OpenWorldSceneOne::CreateStaticCubes(int amount)
	{
		static std::string path = "media/";
		SEntityTransform transform = SEntityTransform();

		for (int i = 0; i < amount; ++i)
		{
			transform.Position = m_CubedStaticSettings[i].position;
			transform.Rotation = m_CubedStaticSettings[i].rotation;
			transform.Scale = m_CubedStaticSettings[i].scale;
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Cube" + i, PhysicsStaticObjectType::Box, path + "Cube.x", transform, { transform.Scale.x * 5, transform.Scale.y * 5, transform.Scale.z * 5 }, false, path + "BasicTexGreen.png");

		}
	}

	void OpenWorldSceneOne::CreateDynamicSpheres(int amount)
	{
		static std::string path = "media/";
		SEntityTransform transform = SEntityTransform();

		for (int i = 0; i < amount; ++i)
		{
			transform.Position = m_SphereDynamicSettings[i].position;
			transform.Rotation = m_SphereDynamicSettings[i].rotation;
			transform.Scale = m_SphereDynamicSettings[i].scale;
			m_PhysicsEntityManager->CreatePhysicsDynamicEntity("Sphere" + i, PhysicsDynmaicObjectType::Sphere, path + "Sphere.x", transform, { transform.Scale.x * 10, transform.Scale.y, transform.Scale.z }, false, path + "BasicTexBlue.png");
			if (m_PhysicsEntityManager->GetEntity("Sphere" + i) && m_PhysicsEntityManager->GetEntity("Sphere" + i)->GetComponent("PhysicsDynamicObject"));
			{
				PhysicsDynamicObjectComponent* comp = static_cast<PhysicsDynamicObjectComponent*>(m_PhysicsEntityManager->GetEntity("Sphere" + i)->GetComponent("PhysicsDynamicObject"));
				comp->GetActor()->setMass(m_SphereDynamicSettings[i].mass);
				comp->GetActor()->setMassSpaceInertiaTensor(physx::PxVec3(m_SphereDynamicSettings[i].mass));
			
			}
		}
	}

	void OpenWorldSceneOne::CreateStaticSpheres(int amount)
	{
		static std::string path = "media/";
		SEntityTransform transform = SEntityTransform();

		for (int i = 0; i < amount; ++i)
		{
			transform.Position = m_SphereStaticSettings[i].position;
			transform.Rotation = m_SphereStaticSettings[i].rotation;
			transform.Scale = m_SphereStaticSettings[i].scale;
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Sphere" + i, PhysicsStaticObjectType::Sphere, path + "Sphere.x", transform, { transform.Scale.x * 10, transform.Scale.y, transform.Scale.z }, false, path + "BasicTexGreen.png");
		}
	}

	void OpenWorldSceneOne::GUI()
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

		if (KeyHit(KeyCode::Key_Escape))
		{
			m_IsPaused = !m_IsPaused;
			if (m_IsPaused)
				ImGui::OpenPopup("PauseMenuPopUp");
		}
		PauseMenu();
	}

	void OpenWorldSceneOne::PauseMenu()
	{
		ImGuiWindowFlags popupFlags = 0;
		popupFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;


		if (ImGui::BeginPopupModal("PauseMenuPopUp", nullptr, popupFlags))
		{
			ImVec2 vec = ImGui::GetMainViewport()->GetCenter();
			ImVec2 size = ImGui::GetWindowSize();
			ImGui::SetWindowPos({ vec.x - (size.x / 2), vec.y - (size.y / 2) });

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

	physx::PxConvexMesh* OpenWorldSceneOne::MakeTrack(int index, Entity* entity)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;
		if (entity != nullptr)
		{

			if (entity->GetComponent("Renderer"))
			{
				RendererComponent* comp = static_cast<RendererComponent*>(entity->GetComponent("Renderer"));
				vertexCount = comp->GetNumberOfVertices(index);

				std::vector<CVector3> trackVertices = comp->GetVertices();
				for (int i = 0; i < vertexCount; i++)
				{
					vertices.push_back(physx::PxVec3(trackVertices[i].x, trackVertices[i].y, trackVertices[i].z));
				}

				physx::PxVec3* v = vertices.data();

				return CreateConvexMesh(v, vertexCount, m_PhysicsSystem->GetPhysics(), m_PhysicsSystem->GetCooking());
			}
		}
		return nullptr;
	}

}