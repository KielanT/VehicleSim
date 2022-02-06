#include "ppch.h"
#include "TempSceneOne.h"
#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"

//#include "vehicle/PxVehicleSDK.h"
#include "PxPhysicsAPI.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

namespace Project
{

    TempSceneOne::TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
    {
        m_Renderer = renderer;
        m_SceneIndex = sceneIndex;

        m_AmbientColour = ambientColour;
        m_SpecularPower = specularPower;
        m_backgroundColour = backgroundColour;
        m_VsyncOn = vsyncOn;

        m_sceneManager = sceneManager;
    }

    bool TempSceneOne::InitGeometry()
    {
        m_EntityManager = new EntityManager(m_Renderer);
        m_LightEntityManager = new EntityManager(m_Renderer);
        m_TestManager = new EntityManager(m_Renderer);

        CParseLevel LevelParser(m_TestManager);

        LevelParser.ParseFile("test.xml");

        std::string path = "media/";

        m_EntityManager->CreateModelEntity("Cube", path + "Cube.x");
        m_EntityManager->CreateModelEntity("Ground", path + "Hills.x", path + "GrassDiffuseSpecular.dds");
        
        m_EntityManager->CreateModelEntity("Crate", path + "CargoContainer.x", path + "CargoA.dds");
        m_LightEntityManager->CreateLightEntity("LightOne");
        
        m_SceneCamera = new Camera();
        return true;
    }

    bool TempSceneOne::InitScene()
    {

        if (m_EntityManager->GetEntity("Cube")->GetComponent("Transform"))
        {
            TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube")->GetComponent("Transform"));
            comp->SetPosition({10, 30.0f, 0 });
        }

        if (m_EntityManager->GetEntity("Crate")->GetComponent("Transform"))
        {
             TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Crate")->GetComponent("Transform"));
             comp->SetPosition({ 45, 0, 45 });
             comp->SetRotation({ 0.0f, ToRadians(-50.0f), 0.0f });
             comp->SetScale({ 6.0f, 6.0f, 6.0f });
        }
        
        if (m_LightEntityManager->GetEntity("LightOne")->GetComponent("Transform"))
        {
            TransformComponent* comp = static_cast<TransformComponent*>(m_LightEntityManager->GetEntity("LightOne")->GetComponent("Transform"));
            comp->SetPosition({ 0, 20, 0 });
        
            if (m_LightEntityManager->GetEntity("LightOne")->GetComponent("Light Renderer"))
            {
                LightRendererComponent* compRenderer = 
                static_cast<LightRendererComponent*>(m_LightEntityManager->GetEntity("LightOne")->GetComponent("Light Renderer"));
        
                
                comp->SetScale(CVector3(pow(compRenderer->GetStrength(), 0.7f), pow(compRenderer->GetStrength(), 0.7f), pow(compRenderer->GetStrength(), 0.7f)));
            }
        }

        m_SceneCamera->SetPosition({ 25, 12,-10 });
        m_SceneCamera->SetRotation({ ToRadians(13.0f), ToRadians(15.0f), 0.0f });

        return true;
    }

    void TempSceneOne::RenderScene()
    {

        
        ImGui::ShowDemoWindow();
        
       

        m_TestManager->RenderAllEntities();
        m_EntityManager->RenderAllEntities();
        m_LightEntityManager->RenderAllEntities();

        
       
    }

    void TempSceneOne::UpdateScene(float frameTime)
    {
        m_EntityManager->UpdateAllEntities(frameTime);
        m_LightEntityManager->UpdateAllEntities(frameTime);
        m_TestManager->UpdateAllEntities(frameTime);

        m_SceneCamera->Control(frameTime);

        if (KeyHit(Key_P))
        {
            m_sceneManager->LoadScene(1);
            m_sceneManager->RemoveSceneAtIndex(0);
        }

    }

    void TempSceneOne::ReleaseResources()
    {
        delete m_SceneCamera;     m_SceneCamera = nullptr;

        m_EntityManager->DestroyAllEntities();
        m_LightEntityManager->DestroyAllEntities();
        m_TestManager->DestroyAllEntities();
    }
}
