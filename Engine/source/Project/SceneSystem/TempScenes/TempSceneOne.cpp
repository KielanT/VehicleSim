#include "ppch.h"
#include "TempSceneOne.h"
#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"



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


        /////////////////////////////////
        // Physx set up temp (Physx 4.1 documentation startup and shutdown)
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
        if (!m_Foundation)
           m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxCreateFoundation Failed");

        m_Pvd = physx::PxCreatePvd(*m_Foundation); 
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5424, 10);
        m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

        physx::PxTolerancesScale scale = physx::PxTolerancesScale();
        scale.length = 0.1f;
        //scale.speed = 0.1f;
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, scale, true, m_Pvd);
        if(!m_Physics)
            m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxCreatePhysics Failed");
        
      
       if (!PxInitExtensions(*m_Physics, m_Pvd))
           m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxInitExtensions Failed");

       
       
       physx::PxCookingParams params(m_Physics->getTolerancesScale());
       params.meshWeldTolerance = 0.001f; // Physx sample default
       params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
       params.buildGPUData = true; //Enable GRB data being produced in cooking.

       m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, params);
       if (!m_Cooking)
           m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxCreateCooking Failed");


       physx::PxSceneDesc desc(scale);
       desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
     

       if (!desc.cpuDispatcher)
       {
           physx::PxU32 mNbThreads = 1;
           m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(mNbThreads); 
           if (!m_CpuDispatcher)
               m_Log.ErrorMessage(renderer->GetWindowsProperties(), "CpuDispatcher Failed");
           desc.cpuDispatcher = m_CpuDispatcher;
       }
       
       if (!desc.filterShader)
           desc.filterShader = getSampleFilterShader();

       customizeSceneDesc(desc);
       m_Scene = m_Physics->createScene(desc);
       if (!m_Scene)
           m_Log.ErrorMessage(renderer->GetWindowsProperties(), "Scene Failed");
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
        m_EntityManager->CreateModelEntity("Cube2", path + "Cube.x", path + "brick1.jpg");
        m_EntityManager->CreateModelEntity("Ground", path + "Ground.x", path + "GrassDiffuseSpecular.dds");

        m_EntityManager->CreateModelEntity("Crate", path + "CargoContainer.x", path + "CargoA.dds");
        m_LightEntityManager->CreateLightEntity("LightOne");

        //m_Material = m_Physics->createMaterial(.5f, .5f, .1f);
        m_Material = m_Physics->createMaterial(0, 0, 0);

        m_BoxActor = m_Physics->createRigidDynamic(physx::PxTransform({ 0.0f, 40.0f, 0.0f }));
        //m_BoxActor->setActorFlags(physx::PxActorFlag::eDISABLE_SIMULATION);
        

        m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(5, 5, 5), *m_Material);
        
        m_BoxActor2 = m_Physics->createRigidStatic({ 0.0f, 10.0f, 0.0f });
        m_BoxShape2 = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor2, physx::PxBoxGeometry(5, 5, 5), *m_Material);

        
        m_Scene->addActor(*m_BoxActor);
        m_Scene->addActor(*m_BoxActor2);
        
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
        m_Scene->simulate(frameTime);
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

        /*if (m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"))
        {
            TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube2")->GetComponent("Transform"));

            CVector3 vect;
            vect.x = m_BoxActor2->getGlobalPose().p.x;
            vect.y = m_BoxActor2->getGlobalPose().p.y;
            vect.z = m_BoxActor2->getGlobalPose().p.z;
            comp->SetPosition(vect);
        }*/

        m_EntityManager->UpdateAllEntities(frameTime);
        m_LightEntityManager->UpdateAllEntities(frameTime);
        m_TestManager->UpdateAllEntities(frameTime);

        m_SceneCamera->Control(frameTime);

        if (KeyHit(Key_L))
        {
            m_sceneManager->LoadScene(1);
            m_sceneManager->RemoveSceneAtIndex(0);
        }

        m_Scene->fetchResults(true);
    }

    void TempSceneOne::ReleaseResources()
    {

        m_EntityManager->DestroyAllEntities();
        m_LightEntityManager->DestroyAllEntities();
        m_TestManager->DestroyAllEntities();

        m_Physics->release();       delete m_Physics;       m_Physics = nullptr;
        m_Foundation->release();    m_Foundation = nullptr;
        m_Cooking->release();       m_Cooking = nullptr;
        m_Pvd->release();           m_Pvd = nullptr;
        m_Scene->release();         m_Scene = nullptr;

        m_BoxActor->release();
        m_BoxActor2->release();
        m_BoxShape->release();
        m_Material->release();

        delete m_SceneCamera;       m_SceneCamera = nullptr;
        delete m_CpuDispatcher;     m_CpuDispatcher = nullptr;
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

        ImGuiWindowFlags window_flags = 0;

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

        ImGui::End();
    }

    bool TempSceneOne::advance(physx::PxReal dt)
    {
        mAccumulator += dt;
        if (mAccumulator < mStepSize)
            return false;

        mAccumulator -= mStepSize;

        m_Scene->simulate(mStepSize);
        return true;
    }
}
