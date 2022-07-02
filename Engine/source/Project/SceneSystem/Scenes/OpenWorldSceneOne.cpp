#include "ppch.h"
#include "OpenWorldSceneOne.h"


namespace Project
{

	OpenWorldSceneOne::OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour,
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

	OpenWorldSceneOne::OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex,
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

	bool OpenWorldSceneOne::InitGeometry()
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
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Plane", PhysicsStaticObjectType::Plane, path + "Ground.x");
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png");
			
			SEntityTransform transform = SEntityTransform();
			transform.Position = {0, 5, 10};
			//m_PhysicsEntityManager->CreatePhysicsDynamicEntity("test", PhysicsDynmaicObjectType::ConvexMesh, path + "TestCube.obj", transform);
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("test", PhysicsStaticObjectType::ConvexMesh, path + "TestCube.obj", transform);
			CreateDynamicCubes(CUBE_DYNAMIC_AMOUNT);
			CreateStaticCubes(1);
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
		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void OpenWorldSceneOne::UpdateScene(float frameTime)
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

	void OpenWorldSceneOne::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

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
			m_PhysicsEntityManager->CreatePhysicsDynamicEntity("Cube" + i, PhysicsDynmaicObjectType::Box, path + "Cube.x", transform, { transform.Scale.x * 5, transform.Scale.y * 5, transform.Scale.z * 5 });
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
			transform.Position = { 100, 5, 100 };// m_CubeSettings[i].position;
			//transform.Rotation = m_CubeSettings[i].rotation;
			//transform.Scale = m_CubeSettings[i].scale;
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("Cube" + i, PhysicsStaticObjectType::Box, path + "Cube.x", transform, { transform.Scale.x * 5, transform.Scale.y * 5, transform.Scale.z * 5 });

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