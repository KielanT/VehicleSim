#include "ppch.h"
#include "PhysX4.h"

namespace Project
{

    bool PhysX::InitPhysics(float meshWeldTolerance, CVector3 gravity)
    {
         m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
        if (!m_Foundation)
            return false;

        m_Pvd = physx::PxCreatePvd(*m_Foundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5424, 10);
        m_Pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        if (!m_Pvd)
            return false;

        physx::PxTolerancesScale scale = physx::PxTolerancesScale();
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, scale, true, m_Pvd);
        if (!m_Physics)
            return false;

        if (!PxInitExtensions(*m_Physics, m_Pvd))
            return false;



        physx::PxCookingParams params(m_Physics->getTolerancesScale());
        params.meshWeldTolerance = meshWeldTolerance;
        params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
        params.buildGPUData = true; //Enable GRB data being produced in cooking.

        m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, params);
        if (!m_Cooking)
            return false;

        physx::PxSceneDesc desc(scale);
        desc.gravity = physx::PxVec3(gravity.x, gravity.y, gravity.z);

        if (!desc.cpuDispatcher)
        {
            physx::PxU32 mNbThreads = 1;
            m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(mNbThreads);
            if (!m_CpuDispatcher)
                return false;
            desc.cpuDispatcher = m_CpuDispatcher;
        }

        if (!desc.filterShader)
            desc.filterShader = getSampleFilterShader();

        m_Scene = m_Physics->createScene(desc);
        if (!m_Scene)
            return false;

        return true;
    }


    void PhysX::ShutdownPhysics()
    {
        SAFE_RELEASE(m_Scene);
        SAFE_RELEASE(m_CpuDispatcher);
        SAFE_RELEASE(m_Cooking);

        PxCloseExtensions();
        SAFE_RELEASE(m_Physics);
        SAFE_RELEASE(m_Pvd);
        
        
       
        SAFE_RELEASE(m_Foundation);
       
    }

    const EPhysicsType PhysX::GetPhysicsType()
    {
        return EPhysicsType::PhysX4;
    }
}