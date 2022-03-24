#include "ppch.h"
#include "TempSceneThree.h"

#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"

namespace Project
{
	TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
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

	TempSceneThree::TempSceneThree(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
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

	bool TempSceneThree::InitGeometry()
	{
		
		m_EntityManager = new EntityManager(m_Renderer);
		
		m_SceneCamera = new Camera();
		
		std::string path = "media/";
		m_EntityManager->CreateModelEntity("Floor", path + "Ground.x");

		
		if (m_EnablePhysics)
		{
			m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

			if (!m_PhysicsSystem->InitPhysics())
				m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

			m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0, 0, 0);
			
		}

		return true;
	}

	bool TempSceneThree::InitScene()
	{
		
		


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
		if (m_EnablePhysics)
		{
			m_PhysicsSystem->GetScene()->simulate(frameTime);

			
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}

		m_EntityManager->UpdateAllEntities(frameTime);

		m_SceneCamera->Control(frameTime);
		if (KeyHit(Key_L))
		{
			
			m_sceneManager->RemoveSceneAtIndex(0);
			
			m_sceneManager->LoadScene(1);
			
		}
		
	}

	void TempSceneThree::ReleaseResources()
	{
		
		if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		
		
		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
		
		
	}
}