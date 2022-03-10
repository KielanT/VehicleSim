#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

#include "Project/SceneSystem/CParseLevel.h"

#include <PxPhysicsAPI.h>
#include "extensions/PxDefaultSimulationFilterShader.h"




namespace Project
{
	// Test
	class UserErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file,
			int line)
		{
			// error processing implementation
		}
	};

	PX_FORCE_INLINE physx::PxSimulationFilterShader getSampleFilterShader()
	{
		return physx::PxDefaultSimulationFilterShader;
	}

	// Test globals 
	static UserErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	class TempSceneOne : public IScene
	{
	public:
		TempSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour = CVector3(0.2f, 0.2f, 0.3f),
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

	protected:
		virtual			void									getDefaultSceneDesc(physx::PxSceneDesc&) {}
		virtual			void									customizeSceneDesc(physx::PxSceneDesc&) {}
	

	private:
		void Gui();

		// Gui Test variables
		CVector3 renderedBox;

		IRenderer* m_Renderer;
		int m_SceneIndex;

		Camera* m_SceneCamera;

		EntityManager* m_EntityManager;
		EntityManager* m_LightEntityManager;

		EntityManager* m_TestManager;
		
		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		CDirectX11SceneManager* m_sceneManager;

		ErrorLogger m_Log;

		// Physx setup temp
		physx::PxFoundation* m_Foundation;
		physx::PxPvd* m_Pvd;
		physx::PxPhysics* m_Physics;
		physx::PxCooking* m_Cooking;

		// Physx Scene test
		physx::PxScene* m_Scene;
		physx::PxCpuDispatcher* m_CpuDispatcher;
		physx::PxCudaContextManager* m_CudaContextManager;
		

		// Physx Geometry test
		physx::PxRigidDynamic* m_BoxActor;
		physx::PxRigidActor* m_BoxActor2;
		physx::PxShape* m_BoxShape;
		physx::PxShape* m_BoxShape2;
		physx::PxMaterial* m_Material;

		// Test
		bool advance(physx::PxReal dt);
		float mAccumulator = 0.0f;
		float mStepSize = 1.0f / 60.0f;
	};
}


