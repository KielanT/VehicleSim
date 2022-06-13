#include "ppch.h"
#include "TempSceneOne.h"
#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"



#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


namespace Project
{
    ErrorLogger log;
    TempSceneOne::TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
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

    TempSceneOne::TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour, float specularPower, ColourRGBA backgroundColour, bool vsyncOn)
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

    bool TempSceneOne::InitGeometry()
    {
        
        m_EntityManager = new EntityManager(m_Renderer);
        
        m_LightEntityManager = new EntityManager(m_Renderer);
        
        m_TestManager = new EntityManager(m_Renderer);
        
        CParseLevel LevelParser(m_TestManager);
        
        LevelParser.ParseFile("test.xml");
        
        std::string path = "media/";
        
        m_EntityManager->CreateModelEntity("Cube",  path + "Cube.x");
        m_EntityManager->CreateModelEntity("Cube2", path + "Cube.x", true, path + "brick1.jpg");
        m_EntityManager->CreateModelEntity("Ground", path + "Ground.x", true, path + "GrassDiffuseSpecular.dds");
        
        m_EntityManager->CreateModelEntity("Crate", path + "CargoContainer.x", true, path + "CargoA.dds");
        m_LightEntityManager->CreateLightEntity("LightOne");


        if (m_EnablePhysics)
        {
             m_PhysicsSystem = NewPhysics(m_sceneManager->GetWindowsProperties().PhysicsType);

            if (!m_PhysicsSystem->InitPhysics())
                m_Log.ErrorMessage(m_Renderer->GetWindowsProperties(), "Failed to Initialise Physics");

            //m_Material = m_Physics->createMaterial(.5f, .5f, .1f);
            m_Material = m_PhysicsSystem->GetPhysics()->createMaterial(0, 0, 0);

            m_BoxActor = m_PhysicsSystem->GetPhysics()->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 0.0f }));
            //m_BoxActor->setActorFlags(physx::PxActorFlag::eDISABLE_SIMULATION);


            m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5, 5, 5), *m_Material);

            m_BoxActor2 = m_PhysicsSystem->GetPhysics()->createRigidStatic({ 0.0f, 10.0f, 0.0f });
            m_BoxShape2 = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor2, physx::PxBoxGeometry(5, 5, 5), *m_Material);


            m_PhysicsSystem->GetScene()->addActor(*m_BoxActor);
            m_PhysicsSystem->GetScene()->addActor(*m_BoxActor2);
        }
        
        m_SceneCamera = new Camera();
        return true;
    }

    bool TempSceneOne::InitScene()
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

        m_SceneCamera->SetPosition({ -3.2f, 63.5f, -157.0f });
        m_SceneCamera->SetRotation({ 0.3f, -0.06f, 0.0f });

        return true;
    }

    void TempSceneOne::RenderScene()
    {
        Gui();

        m_TestManager->RenderAllEntities();
        m_EntityManager->RenderAllEntities();
        m_LightEntityManager->RenderAllEntities();
    }

    
    void TempSceneOne::UpdateScene(float frameTime)
    {
        if (m_EnablePhysics)
        {
            m_PhysicsSystem->GetScene()->simulate(frameTime);
            //advance(frameTime);
            
            if (m_EntityManager->GetEntity("Cube")->GetComponent("Transform"))
            {
                
                TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube")->GetComponent("Transform"));

                CVector3 vect;
                vect.x = m_BoxActor->getGlobalPose().p.x;
                vect.y = m_BoxActor->getGlobalPose().p.y;
                vect.z = m_BoxActor->getGlobalPose().p.z;
                comp->SetPosition(vect);

                renderedBox = comp->GetPosition();
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
            m_PhysicsSystem->GetScene()->fetchResults(true);
        }

        m_EntityManager->UpdateAllEntities(frameTime);
        m_LightEntityManager->UpdateAllEntities(frameTime);
        m_TestManager->UpdateAllEntities(frameTime);

        m_SceneCamera->Control(frameTime);

        if (KeyHit(Key_L))
        {
            m_sceneManager->RemoveSceneAtIndex(1);
            m_sceneManager->LoadScene(0);
            
        }

      
    }

    void TempSceneOne::ReleaseResources()
    {
        if (m_EntityManager         != nullptr)    m_EntityManager->DestroyAllEntities(); 
        if (m_LightEntityManager    != nullptr)    m_LightEntityManager->DestroyAllEntities();
        if (m_TestManager           != nullptr)    m_TestManager->DestroyAllEntities();
        
        SAFE_RELEASE(m_Material);
        SAFE_RELEASE(m_BoxActor);
        SAFE_RELEASE(m_BoxActor2);

        if(m_PhysicsSystem != nullptr) m_PhysicsSystem->ShutdownPhysics();


        if (m_SceneCamera != nullptr) { delete m_SceneCamera;  m_SceneCamera = nullptr; }
    }

    void TempSceneOne::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
    {
        physx::PxTransform a, b;
        a = pairs->shapes[0]->getActor()->getGlobalPose();
        b = pairs->shapes[1]->getActor()->getGlobalPose();

        pairs = nullptr;
    }

    void TempSceneOne::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
    {
    }

    void TempSceneOne::Gui()
    {
        //ImGui::ShowDemoWindow();

       /* ImGuiWindowFlags window_flags = 0;

        ImGui::Begin("Debug", 0, window_flags);

        if (ImGui::TreeNodeEx("PhysX tests cube 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.18f);
            ImGui::Text(" Velocity [X: %g] [Y: %g] [Z: %g]", m_BoxActor->getLinearVelocity().x, 
                m_BoxActor->getLinearVelocity().y, m_BoxActor->getLinearVelocity().z);
            ImGui::SameLine();

            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNodeEx("MainCubeRender", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.18f);
            ImGui::Text(" [X: %g] [Y: %g] [Z: %g] ", renderedBox.x, renderedBox.y, renderedBox.z);
            ImGui::SameLine();

            ImGui::TreePop();
            ImGui::Separator();
        }

        if (ImGui::TreeNodeEx("MainCube physx", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.18f);
            ImGui::Text(" [X: %g] [Y: %g] [Z: %g] ", m_BoxActor->getGlobalPose().p.x, m_BoxActor->getGlobalPose().p.y, 
                m_BoxActor->getGlobalPose().p.z);
            ImGui::SameLine();

            ImGui::TreePop();
            ImGui::Separator();
        }

        ImGui::End();*/
    }

  bool TempSceneOne::advance(physx::PxReal dt)
    {
       /* mAccumulator += dt;
        if (mAccumulator < mStepSize)
            return false;

        mAccumulator -= mStepSize;

        m_Scene->simulate(mStepSize);*/
        return true;
    }
}
