#include "ppch.h"
#include "TempSceneFive.h"
#include "Project/EntitySystem/Components/TransformComponent.h"

namespace Project
{

	TempSceneFive::TempSceneFive(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
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

	TempSceneFive::TempSceneFive(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour /*= CVector3(1.0f, 1.0f, 1.0f)*/, float specularPower /*= 256.0f*/, ColourRGBA backgroundColour /*= ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f)*/, bool vsyncOn /*= true*/)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;
		m_EnablePhysics = enablePhysics;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;
	}

	bool TempSceneFive::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();

		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");
		m_EntityManager->CreateModelEntity("Test Cube", path + "Cube.x", true, path + "brick1.jpg");

		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");
				
			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0, 0, 0);

			// Set Actors and shapes here
			m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 0.0f }));
			m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5.0f, 5.0f, 5.0f), *m_Material);

			m_FloorActor = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 0.0f, 0.0f });
			m_FloorBoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_FloorActor, physx::PxBoxGeometry(1000.0f, 0.01f, 1000.0f), *m_Material);

			m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
			m_PhysicsSystem->GetScene()->addActor(*m_FloorActor);
		}

		return true;
	}

	bool TempSceneFive::InitScene()
	{
		if (m_EnablePhysics && m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));

			CVector3 vectPos;
			vectPos.x = m_BoxActor->getGlobalPose().p.x;
			vectPos.y = m_BoxActor->getGlobalPose().p.y;
			vectPos.z = m_BoxActor->getGlobalPose().p.z;
			comp->SetPosition(vectPos);

			CVector3 vectRot;
			vectRot.x = m_BoxActor->getGlobalPose().q.x;
			vectRot.y = m_BoxActor->getGlobalPose().q.y;
			vectRot.z = m_BoxActor->getGlobalPose().q.z;
			comp->SetRotation(vectRot);
		}


		// Pos 1
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });

		// Pos 2
		//m_SceneCamera->SetPosition({ -40, 10, 0 });
		//m_SceneCamera->SetRotation({ 0, ToRadians(90), 0 });

		return true;
	}

	void TempSceneFive::RenderScene()
	{
		m_EntityManager->RenderAllEntities();
	}

	void TempSceneFive::UpdateScene(float frameTime)
	{
		if (m_EnablePhysics)
		{
			m_PhysicsSystem->GetScene()->simulate(frameTime);
			m_PhysicsSystem->GetScene()->fetchResults(true);

			if (m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"))
			{

				TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Test Cube")->GetComponent("Transform"));
				CVector3 vect;
				vect.x = m_BoxActor->getGlobalPose().p.x;
				vect.y = m_BoxActor->getGlobalPose().p.y;
				vect.z = m_BoxActor->getGlobalPose().p.z;
				comp->SetPosition(vect);
				CVector3 vectRot;
				vectRot.x = m_BoxActor->getGlobalPose().q.x;
				vectRot.y = m_BoxActor->getGlobalPose().q.y;
				vectRot.z = m_BoxActor->getGlobalPose().q.z;
				comp->SetRotation(vectRot);

			}
		}

		m_EntityManager->UpdateAllEntities(frameTime);

		m_SceneCamera->Control(frameTime);
	}

	void TempSceneFive::ReleaseResources()
	{
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();

		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}

}
