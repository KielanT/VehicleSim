#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

#include "Project/SceneSystem/CParseLevel.h"

#include <PxPhysicsAPI.h>
#include "extensions/PxDefaultSimulationFilterShader.h"

#include "Physics/PhysX4.1/PhysxHelpers.h"



namespace Project
{
	// Test

	//inline physx::PxFilterFlags SampleSubmarineFilterShader(
	//	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	//	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	//	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	//{
	//	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

	//	return physx::PxFilterFlag::eDEFAULT;
	//}



	//PX_FORCE_INLINE physx::PxSimulationFilterShader getSampleFilterShader()
	//{
	//	return physx::PxDefaultSimulationFilterShader;
	//}

	//// Test globals 
	//UserErrorCallback gDefaultErrorCallback;
	//physx::PxDefaultAllocator gDefaultAllocatorCallback;

	class TempSceneOne : public IScene, public physx::PxSimulationEventCallback
	{
	public:
		TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour = CVector3(0.2f, 0.2f, 0.3f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		virtual bool InitGeometry() override;

		virtual bool InitScene() override;

		virtual void RenderScene() override;

		virtual void UpdateScene(float frameTime) override;

		virtual void ReleaseResources() override;

		virtual int GetSceneIndex() override { return m_SceneIndex; }

		virtual Camera* GetCamera() override { return m_SceneCamera; } 

		// Scene Settings
		virtual void SetAmbientColour(CVector3 ambientColour) override { m_AmbientColour = ambientColour; }
		virtual void SetSpecularPower(float specularPower) override { m_SpecularPower = specularPower; }
		virtual void SetBackgroundColour(ColourRGBA backgroundColour) override{ m_backgroundColour = backgroundColour; }
		virtual void SetVSync(bool VSync) override{ m_VsyncOn = VSync; }

		virtual CVector3 GetAmbientColour() override { return m_AmbientColour; }
		virtual float GetSpecularPower() override{ return m_SpecularPower; }
		virtual ColourRGBA GetBackgroundColour() override{ return m_backgroundColour; }
		virtual bool GetVSync() override{ return m_VsyncOn; }

		virtual void							onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		virtual void							onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		virtual void							onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
		virtual void							onWake(physx::PxActor**, physx::PxU32) {}
		virtual void							onSleep(physx::PxActor**, physx::PxU32) {}
		virtual void							onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}

	protected:

		

		virtual			void									getDefaultSceneDesc(physx::PxSceneDesc&) {}
		/*virtual			void									customizeSceneDesc(physx::PxSceneDesc& sceneDesc)
		{
			sceneDesc.filterShader = SampleSubmarineFilterShader;
			sceneDesc.simulationEventCallback = this;
			sceneDesc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;
		}*/
	private:
		void Gui();

		// Gui Test variables
		CVector3 renderedBox;

		IRenderer* m_Renderer;
		int m_SceneIndex;
		bool m_EnablePhysics;

		Camera* m_SceneCamera = nullptr;

		EntityManager* m_EntityManager = nullptr;
		EntityManager* m_LightEntityManager = nullptr;

		EntityManager* m_TestManager = nullptr;
		
		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		CDirectX11SceneManager* m_sceneManager;

		ErrorLogger m_Log;

		IPhysics* m_PhysicsSystem = nullptr;
		

		// Physx Geometry test
		physx::PxRigidDynamic* m_BoxActor = nullptr;
		physx::PxRigidActor* m_BoxActor2 = nullptr;
		physx::PxShape* m_BoxShape = nullptr;
		physx::PxShape* m_BoxShape2 = nullptr;
		physx::PxMaterial* m_Material = nullptr;

		// Test
		bool advance(physx::PxReal dt);
		float mAccumulator = 0.0f;
		float mStepSize = 1.0f / 60.0f;
	};
}


