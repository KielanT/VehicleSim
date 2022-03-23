#include "ppch.h"
#include "TempSceneTwo.h"
#include "Project/EntitySystem/Components/TransformComponent.h"

namespace Project
{
    TempSceneTwo::TempSceneTwo(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
    {
        m_Renderer = renderer;
        m_SceneIndex = sceneIndex;

        m_AmbientColour = ambientColour;
        m_SpecularPower = specularPower;
        m_backgroundColour = backgroundColour;
        m_VsyncOn = vsyncOn;

        m_sceneManager = sceneManager;
    }

    bool TempSceneTwo::InitGeometry()
    {
        m_EntityManager = new EntityManager(m_Renderer);

        std::string path = "media/";

        m_EntityManager->CreateModelEntity("Cube", path + "Cube.x");
        m_EntityManager->CreateModelEntity("Ground", path + "Hills.x");

        m_EntityManager->CreateModelEntity("Crate", path + "CargoContainer.x");

        m_SceneCamera = new Camera();
        return true;
    }

    bool TempSceneTwo::InitScene()
    {
        if (m_EntityManager->GetEntity("Crate")->GetComponent("Transform"))
        {
            TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Crate")->GetComponent("Transform"));
            comp->SetPosition({ 45, 0, 45 });
            comp->SetRotation({ 0.0f, ToRadians(-50.0f), 0.0f });
            comp->SetScale({ 1.0f, 2.0f, 1.0f });
        }

        m_SceneCamera->SetPosition({ 25, 12,-10 });
        m_SceneCamera->SetRotation({ ToRadians(13.0f), ToRadians(15.0f), 0.0f });

        return true;
    }

    void TempSceneTwo::RenderScene()
    {
        m_EntityManager->RenderAllEntities();
    }

    void TempSceneTwo::UpdateScene(float frameTime)
    {
        m_EntityManager->UpdateAllEntities(frameTime);

        m_SceneCamera->Control(frameTime);

        if (KeyHit(Key_P))
        {
            m_sceneManager->LoadScene(0);
            m_sceneManager->RemoveSceneAtIndex(1);
        }
    }

    void TempSceneTwo::ReleaseResources()
    {
        delete m_SceneCamera;     m_SceneCamera = nullptr;

        m_EntityManager->DestroyAllEntities();
    }
}