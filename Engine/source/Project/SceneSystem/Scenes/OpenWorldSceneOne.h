#pragma once
#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

namespace Project
{
	class OpenWorldSceneOne : public IScene
	{
	public:
		OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
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
		virtual void SetBackgroundColour(ColourRGBA backgroundColour) override { m_backgroundColour = backgroundColour; }
		virtual void SetVSync(bool VSync) override { m_VsyncOn = VSync; }

		virtual CVector3 GetAmbientColour() override { return m_AmbientColour; }
		virtual float GetSpecularPower() override { return m_SpecularPower; }
		virtual ColourRGBA GetBackgroundColour() override { return m_backgroundColour; }
		virtual bool GetVSync() override { return m_VsyncOn; }
		
	private:
		void CreateDynamicCubes(int amount);
		void CreateStaticCubes(int amount);

	private:
		ErrorLogger m_Log;

		IRenderer* m_Renderer;
		int m_SceneIndex;
		bool m_EnablePhysics;

		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		Camera* m_SceneCamera = nullptr;

		EntityManager* m_EntityManager = nullptr;
		EntityManager* m_PhysicsEntityManager = nullptr;
		EntityManager* m_LightEntityManager = nullptr;

		CDirectX11SceneManager* m_sceneManager;

		IPhysics* m_PhysicsSystem = nullptr;

		physx::PxMaterial* m_Material = nullptr;

		struct DefaultObjectSettings
		{
			CVector3 position;
			CVector3 rotation;
			CVector3 scale;
			float mass;
		};
		
		const int CUBE_DYNAMIC_AMOUNT = 10;
		DefaultObjectSettings m_CubedDynamicSettings[10]
		{
			//Position						  Rotation					      Scale						  Mass
			{ CVector3(  20.0f, 5.0f,   0.0f), CVector3( 0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f },
			{ CVector3( -200.0f, 5.0f,   0.0f), CVector3( 0.0f, 0.0f,   0.0f), CVector3(1.0f, 1.0f, 1.0f), 10000.0f },
			{ CVector3( -40.0f, 5.0f,  20.0f), CVector3( 0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10.0f },
			{ CVector3(  60.0f, 10.0f, -100.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(2.0f, 2.0f, 2.0f), 100000.0f },
			{ CVector3(  30.0f, 5.0f, -90.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f }, 
			{ CVector3(  90.0f, 25.0f, -200.0f), CVector3( 0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 50000.0f },
			{ CVector3( 100.0f, 5.0f, -40.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f }, 
			{ CVector3( -50.0f, 5.0f,  70.0f), CVector3( 5.0f, 0.0f,  60.0f), CVector3(0.1f, 0.1f, 0.1f), 400.0f }, 
			{ CVector3(-100.0f, 5.0f,  15.0f), CVector3( 0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 1000000.0f }, 
			{ CVector3( -55.0f, 5.0f,  80.0f), CVector3( 0.0f, 0.0f, -40.0f), CVector3(0.5f, 0.5f, 0.5f), 70000.0f }, 
		};
		
		// TEMP
		physx::PxShape* m_TrackShape = nullptr;
		physx::PxRigidStatic* m_Track = nullptr;
		physx::PxConvexMesh* MakeTrack(int index, Entity* entity);

		physx::PxRigidDynamic* m_SphereActor = nullptr;
		physx::PxShape* m_SphereShape = nullptr;

	};
}
