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

       
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), true, m_Pvd);
        if(!m_Physics)
            m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxCreatePhysics Failed");
        
        m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
        if(!m_Cooking)
            m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxCreateCooking Failed");

       
       if (!PxInitExtensions(*m_Physics, m_Pvd))
           m_Log.ErrorMessage(renderer->GetWindowsProperties(), "PxInitExtensions Failed");

       physx::PxSceneDesc desc(m_Physics->getTolerancesScale());
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
        m_EntityManager->CreateModelEntity("Ground", path + "Hills.x", path + "GrassDiffuseSpecular.dds");

        m_EntityManager->CreateModelEntity("Crate", path + "CargoContainer.x", path + "CargoA.dds");
        m_LightEntityManager->CreateLightEntity("LightOne");

        m_BoxActor = m_Physics->createRigidDynamic(physx::PxTransform({ 0.0f, 0.0f, 0.0f }));
        m_BoxActor->setGlobalPose({ 0.0f, 30.0f, 0.0f });

        m_Material = m_Physics->createMaterial(.5f, .5f, .1f);
        m_BoxShape = physx::PxRigidActorExt::createExclusiveShape(*m_BoxActor, physx::PxBoxGeometry(2.0f, 2.0f, 2.0f), *m_Material);

        m_Scene->addActor(*m_BoxActor);
        
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
        m_Scene->simulate(frameTime);
        m_Scene->fetchResults(true);

        if (m_EntityManager->GetEntity("Cube")->GetComponent("Transform"))
        {
            TransformComponent* comp = static_cast<TransformComponent*>(m_EntityManager->GetEntity("Cube")->GetComponent("Transform"));

            CVector3 vect;
            vect.x = m_BoxActor->getGlobalPose().p.x;
            vect.y = m_BoxActor->getGlobalPose().p.y;
            vect.z = m_BoxActor->getGlobalPose().p.z;
            comp->SetPosition(vect);
        }

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


        m_EntityManager->DestroyAllEntities();
        m_LightEntityManager->DestroyAllEntities();
        m_TestManager->DestroyAllEntities();

        m_Physics->release();       delete m_Physics;       m_Physics = nullptr;
        m_Foundation->release();    m_Foundation = nullptr;
        m_Cooking->release();       m_Cooking = nullptr;
        m_Pvd->release();           m_Pvd = nullptr;
        m_Scene->release();         m_Scene = nullptr;

        m_BoxActor->release();
        m_BoxShape->release();
        m_Material->release();

        delete m_SceneCamera;       m_SceneCamera = nullptr;
        delete m_CpuDispatcher;     m_CpuDispatcher = nullptr;
    }
    
}
