#include "ppch.h"
#include "TempSceneSix.h"

namespace Project
{

	TempSceneSix::TempSceneSix(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour,
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

	TempSceneSix::TempSceneSix(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex,
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
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();


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
			
			m_PhysicsEntityManager = new EntityManager(m_Renderer, m_PhysicsSystem);
			m_PhysicsEntityManager->CreatePhysicsEntity("Plane", path + "Ground.x", PhysicsObjectType::Plane, RigidBodyType::Static);
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png");
			m_PhysicsEntityManager->CreatePhysicsEntity("Cube", path + "Cube.x", PhysicsObjectType::Box, RigidBodyType::Dynamic, SEntityTransform(0.0f, 20.0f, 40.0f), { 5.0f, 5.0f, 5.0f });

		}

		return true;
	}

	bool TempSceneSix::InitScene()
	{
		if (m_PhysicsEntityManager->GetEntity("Plane")->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_PhysicsEntityManager->GetEntity("Plane")->GetComponent("Renderer"));
			comp->SetTexture("media/BasicTexOrange.png");
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
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}
	physx::PxRigidStatic* TempSceneSix::CreateDrivablePlane(const physx::PxFilterData& simFilterData, physx::PxMaterial* material, physx::PxPhysics* physics)
	{
		//Add a plane to the scene.
		physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *material);

		//Get the plane shape so we can set query and simulation filter data.
		physx::PxShape* shapes[1];
		groundPlane->getShapes(shapes, 1);

		//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
		physx::PxFilterData qryFilterData;
		DrivableSurface(qryFilterData);
		shapes[0]->setQueryFilterData(qryFilterData);

		//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
		shapes[0]->setSimulationFilterData(simFilterData);

		return groundPlane;
	}
}