#include "ppch.h"
#include "TempSceneBase.h"

/*****************************************************/
/**			  FILE NOT USED FOR                     **/
/**			  CREATING A SCENE						**/
/*****************************************************/

namespace Project
{
	
	TempSceneBase::TempSceneBase(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour,
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
	
	TempSceneBase::TempSceneBase(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex,
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
	
	bool TempSceneBase::InitGeometry()
	{
		m_EntityManager = new EntityManager(m_Renderer);

		m_SceneCamera = std::make_shared<Camera>();


		/*****************************************************/
		/**			   Create the entities                  **/
		/*****************************************************/
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

			

			
			/*****************************************************/
			/**			   Add actors to the scene              **/
			/*****************************************************/

			//m_PhysicsSystem->GetScene()->addActor(*/*ActorName*/);
		}

		return true;
	}
	
	bool TempSceneBase::InitScene()
	{
		m_SceneCamera->SetPosition({ 0, 10, -40 });
		m_SceneCamera->SetRotation({ 0, 0, 0 });
		return true;
	}
	
	void TempSceneBase::RenderScene()
	{
		m_EntityManager->RenderAllEntities();
	}
	
	void TempSceneBase::UpdateScene(float frameTime)
	{
		if (m_EnablePhysics)
		{


			
			m_PhysicsSystem->GetScene()->simulate(frameTime);
			m_PhysicsSystem->GetScene()->fetchResults(true);
		}


		m_EntityManager->UpdateAllEntities(frameTime);
		
	}
	
	void TempSceneBase::ReleaseResources()
	{

		if (m_EntityManager != nullptr)    m_EntityManager->DestroyAllEntities();

		SAFE_RELEASE(m_Material);

		if (m_EnablePhysics && m_PhysicsSystem != nullptr) physx::PxCloseVehicleSDK();

		if (m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();
	}
}