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

		m_SceneCamera = new Camera(true);


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
		std::string path = "media/";
		//m_EntityManager->CreateModelEntity("/*EntityName*/", /*EntityMesh*/);

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
			m_PhysicsEntityManager->CreatePhysicsStaticEntity("TrackFinishLine", PhysicsStaticObjectType::TriangleMesh, path + "TrackOneFinishLine.obj"/*, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, false, path + "brick1.jpg" */);
			m_PhysicsEntityManager->CreateVehicleEntity("MainCar", path + "Compact/untitled1Parented.obj", path + "Compact/untitled4.obj", VehicleSettings(), path + "Compact/CompactBlue.png");
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

	physx::PxTriangleMesh* TrackSceneOne::MakeTrack(int index, Entity* entity)
	{
		physx::PxU32 vertexCount;
		std::vector<physx::PxVec3> vertices;

		
		if (entity != nullptr)
		{

			if (entity->GetComponent("Renderer"))
			{
				RendererComponent* comp = static_cast<RendererComponent*>(entity->GetComponent("Renderer"));
				vertexCount = comp->GetNumberOfVertices(index);
				int triCount = comp->GetNumberTriangles();

				std::vector<CVector3> trackVertices = comp->GetVertices();
				for (int i = 0; i < vertexCount; i++)
				{
					vertices.push_back(physx::PxVec3(trackVertices[i].x, trackVertices[i].y, trackVertices[i].z));
				}
				physx::PxVec3* v = vertices.data();

				physx::PxU32* i = comp->GetIndices().data();

				return CreateTriangleMesh(v, vertexCount, triCount, comp->GetIndices(), *m_PhysicsSystem->GetPhysics(), *m_PhysicsSystem->GetCooking());
				
			}
		}
		return nullptr;
	}

}