#include "ppch.h"
#include "TempSceneSix.h"

namespace Project
{

	TempSceneSix::TempSceneSix(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour,
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

	TempSceneSix::TempSceneSix(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex,
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

	bool TempSceneSix::InitGeometry()
	{
		m_EntityManager = std::make_unique<EntityManager>(m_Renderer);

		m_SceneCamera = std::make_shared<Camera>();


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);
		
		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");

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
			//m_PhysicsEntityManager->CreatePhysicsEntity("Plane", path + "Ground.x", "media/BasicTexOrange.png", PhysicsObjectType::Plane, RigidBodyType::Static);
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png");
			//m_PhysicsEntityManager->CreatePhysicsEntity("Cube", path + "Cube.x", "media/BasicTexWhite.png", PhysicsObjectType::Box, RigidBodyType::Dynamic, SEntityTransform(0.0f, 20.0f, 40.0f), { 5.0f, 5.0f, 5.0f });

		}

		return true;
	}

	bool TempSceneSix::InitScene()
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

	void TempSceneSix::RenderScene()
	{
		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void TempSceneSix::UpdateScene(float frameTime)
	{

		if (m_EnablePhysics && m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->UpdateAllEntities(frameTime);
			m_PhysicsSystem->GetScene()->simulate(frameTime);
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}


			



		m_EntityManager->UpdateAllEntities(frameTime);

	}

	void TempSceneSix::ReleaseResources()
	{

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

}