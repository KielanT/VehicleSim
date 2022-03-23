#include "ppch.h"
#include "TempSceneThree.h"

#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"

namespace Project
{
	/*TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;

		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;

		m_EnablePhysics = false;
	}*/

	TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
	{
		m_Renderer = renderer;
		m_SceneIndex = sceneIndex;
		m_EnablePhysics = enablePhysics;
		if (m_EnablePhysics)
			m_PhysicsSystem = NewPhysics(sceneManager->GetWindowsProperties().PhysicsType);


		m_AmbientColour = ambientColour;
		m_SpecularPower = specularPower;
		m_backgroundColour = backgroundColour;
		m_VsyncOn = vsyncOn;

		m_sceneManager = sceneManager;
		
	}

	bool TempSceneThree::InitGeometry()
	{
		if (m_EnablePhysics)
			if(!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = new Camera();

		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");

		m_EntityManager->CreateModelEntity("Cube", path + "Cube.x");
		m_EntityManager->CreateModelEntity("Cube2", path + "Cube.x", path + "brick1.jpg");

		m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0, 0, 0);
		m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 0.0f }));
		m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5, 5, 5), *m_Material);

		m_BoxActor2 = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 10.0f, 0.0f });
		m_BoxShape2 = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor2, physx::PxBoxGeometry(5, 5, 5), *m_Material);


		m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
		m_PhysicsSystem->GetScene()->addActor(*m_BoxActor2);

		return true;
	}

	bool TempSceneThree::InitScene()
	{
		if (m_EntityManager->GetEntity("Cube")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube")->GetComponent("Transform"));

			CVector3 vect;
			vect.x = m_BoxActor->getGlobalPose().p.x;
			vect.y = m_BoxActor->getGlobalPose().p.y;
			vect.z = m_BoxActor->getGlobalPose().p.z;
			comp->SetPosition(vect);
		}

		if (m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"));
			comp->SetPosition({ 0.0f, 10.0f, 0.0f });
		}


		m_SceneCamera->SetPosition({ 0, 10, -50 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });
		return true;
	}

	void TempSceneThree::RenderScene()
	{
		m_EntityManager->RenderAllEntities();
	}

	void TempSceneThree::UpdateScene(float frameTime)
	{
		m_PhysicsSystem->GetScene()->simulate(frameTime);

		if (m_EntityManager->GetEntity("Cube")->GetComponent("Transform"))
		{

			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube")->GetComponent("Transform"));

			CVector3 vect;
			vect.x = m_BoxActor->getGlobalPose().p.x;
			vect.y = m_BoxActor->getGlobalPose().p.y;
			vect.z = m_BoxActor->getGlobalPose().p.z;
			comp->SetPosition(vect);
		}

		if (m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"))
		{
			TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"));

			CVector3 vect;
			vect.x = m_BoxActor2->getGlobalPose().p.x;
			vect.y = m_BoxActor2->getGlobalPose().p.y;
			vect.z = m_BoxActor2->getGlobalPose().p.z;
			comp->SetPosition(vect);
		}

		m_EntityManager->UpdateAllEntities(frameTime);

		m_SceneCamera->Control(frameTime);

		m_PhysicsSystem->GetScene()->fetchResults(true);
	}

	void TempSceneThree::ReleaseResources()
	{
		delete m_SceneCamera;     m_SceneCamera = nullptr;

		m_EntityManager->DestroyAllEntities();
		m_PhysicsSystem->ShutdownPhysics();
	}
}