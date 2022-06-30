#include "ppch.h"
#include "TrackSceneOne.h"

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

	}

	bool TrackSceneOne::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera(false);


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);

		std::string path = "media/";
		//m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");
		//m_EntityManager->CreateModelEntity("Track", path + "TrackOneInner.obj");
		//m_EntityManager->CreateModelEntity("Track", path + "TestCube.obj");
		m_EntityManager->CreateModelEntity("Track", path + "TrackOneInner.obj");
		//m_EntityManager->CreateModelEntity("Car", path + "Compact/untitled1Parented.obj", true, path + "Compact/CompactBlue.png");

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
			//m_PhysicsEntityManager->CreatePhysicsEntity("Plane", path + "Ground.x","media/BasicTexOrange.png", PhysicsObjectType::Plane, RigidBodyType::Static);
			//m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png");
			//m_PhysicsEntityManager->CreatePhysicsEntity("Cube", path + "Cube.x", "media/BasicTexOrange.png", PhysicsObjectType::Box, RigidBodyType::Dynamic, SEntityTransform(0.0f, 20.0f, 40.0f), { 5.0f, 5.0f, 5.0f });
			//m_PhysicsEntityManager->CreatePhysicsEntity("Cube", path + "TrackOneInner.obj", "media/Compact/CompactBlue.png", PhysicsObjectType::ConvexMesh, RigidBodyType::Static, SEntityTransform(10.0f, 0.0f, 0.0f));


			m_Track = m_PhysicsSystem->GetPhysics()->createRigidStatic(physx::PxTransform({ 10.0f, 0.0f, 0.0f }));
			physx::PxConvexMeshGeometry geom = MakeTrack(0, m_EntityManager->GetEntity("Track"));
			
			m_TrackShape = physx::PxRigidActorExt::createExclusiveShape(*m_Track, geom, *m_Material);
			m_PhysicsSystem->GetScene()->addActor(*m_Track);
		}

		return true;
	}

	bool TrackSceneOne::InitScene()
	{
		//if (m_PhysicsEntityManager->GetEntity("MainCar")->GetComponent("VehicleComponent"))
		//{
		//	VehicleComponent* comp = static_cast<VehicleComponent*>(m_PhysicsEntityManager->GetEntity("MainCar")->GetComponent("VehicleComponent"));
		//	comp->AttachMainCamera(m_SceneCamera);
		//}
		
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });

		return true;
	}

	void TrackSceneOne::RenderScene()
	{
		if (m_PhysicsEntityManager != nullptr)
		{
			m_PhysicsEntityManager->RenderAllEntities();
		}
		m_EntityManager->RenderAllEntities();
	}

	void TrackSceneOne::UpdateScene(float frameTime)
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

	void TrackSceneOne::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)			  m_EntityManager->DestroyAllEntities();
		if (m_PhysicsEntityManager != nullptr)    m_PhysicsEntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);


		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

	physx::PxConvexMesh* TrackSceneOne::MakeTrack(int index, Entity* entity)
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