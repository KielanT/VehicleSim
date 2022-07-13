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
		OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		OpenWorldSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		virtual bool InitGeometry() override;

		virtual bool InitScene() override;

		virtual void RenderScene() override;

		virtual void UpdateScene(float frameTime) override;

		virtual void ReleaseResources() override;

		virtual int GetSceneIndex() override { return m_SceneIndex; }

		virtual  std::shared_ptr<Camera> GetCamera() override { return m_SceneCamera; }

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
		void CreateDynamicSpheres(int amount);
		void CreateStaticSpheres(int amount);

		void GUI();
		void PauseMenu();

		bool m_IsPaused = false;

	private:
		ErrorLogger m_Log;

		std::shared_ptr<IRenderer> m_Renderer;
		int m_SceneIndex;
		bool m_EnablePhysics;

		CVector3 m_AmbientColour;
		float m_SpecularPower;
		ColourRGBA m_backgroundColour;
		bool m_VsyncOn;

		std::shared_ptr<Camera> m_SceneCamera = nullptr;

		std::unique_ptr<EntityManager> m_EntityManager = nullptr;
		std::unique_ptr<EntityManager> m_PhysicsEntityManager = nullptr;
		std::unique_ptr<EntityManager> m_LightEntityManager = nullptr;

		CDirectX11SceneManager* m_sceneManager;

		std::shared_ptr<IPhysics> m_PhysicsSystem = nullptr;

		physx::PxMaterial* m_Material = nullptr;

		struct DefaultObjectSettings
		{
			CVector3 position;
			CVector3 rotation;
			CVector3 scale;
			float mass;
		};
		
		const static int CUBE_DYNAMIC_AMOUNT = 10;
		DefaultObjectSettings m_CubedDynamicSettings[CUBE_DYNAMIC_AMOUNT]
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

		const static int CUBE_STATIC_AMOUNT = 10;
		DefaultObjectSettings m_CubedStaticSettings[CUBE_STATIC_AMOUNT]
		{
			//Position						  Rotation					      Scale						  Mass
			{ CVector3(100.0f, 5.0f, 100.0f), CVector3(0.0f, 90.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f },
			{ CVector3(100.0f, 0.0f,   0.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(1.0f, 1.0f, 1.0f), 10000.0f },
			{ CVector3(-40.0f, 0.0f,  200.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10.0f },
			{ CVector3(-60.0f, 20.0f, -200.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 100000.0f },
			{ CVector3(80.0f, 0.0f, -390.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f },
			{ CVector3(-190.0f, 25.0f, -50.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 50000.0f },
			{ CVector3(300.0f, 10.0f, -40.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(3.5f, 3.5f, 3.5f), 10000.0f },
			{ CVector3(0.0f, 5.0f,  0.0f), CVector3(5.0f, 0.0f,  60.0f), CVector3(0.5f, 0.5f, 0.5f), 400.0f },
			{ CVector3(-150.0f, 2.0f,  -100.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 1000000.0f },
			{ CVector3(-155.0f, 7.0f,  180.0f), CVector3(0.0f, 0.0f, -40.0f), CVector3(0.5f, 0.5f, 0.5f), 70000.0f },
		};

		const static int SPHERE_DYNAMIC_AMOUNT = 10;
		DefaultObjectSettings m_SphereDynamicSettings[SPHERE_DYNAMIC_AMOUNT]
		{
			//Position						  Rotation					      Scale						  Mass
			{ CVector3(200.0f, 10.0f, 100.0f), CVector3(0.0f, 90.0f,   0.0f), CVector3(0.5f, 0.5f, 0.5f), 10000.0f },
			{ CVector3(200.0f, 15.0f,   -110.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(1.0f, 1.0f, 1.0f), 10000.0f },
			{ CVector3(-250.0f, 0.0f,  200.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(2.0f, 2.0f, 2.0f), 10.0f },
			{ CVector3(-250.0f, 15.0f, -200.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 100000.0f },
			{ CVector3(400.0f, 20.0f, -390.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(10.0f, 10.0f, 10.0f), 10000.0f },
			{ CVector3(-400.0f, 5.0f, -400.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 50000.0f },
			{ CVector3(400.0f, 10.0f, 400.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.2f, 0.2f, 0.2f), 10000.0f },
			{ CVector3(400.0f, 5.0f,  250.0f), CVector3(5.0f, 0.0f,  60.0f), CVector3(0.5f, 0.5f, 0.5f), 400.0f },
			{ CVector3(-300.0f, 2.0f,  100.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.7f, 0.7f, 0.7f), 1000000.0f },
			{ CVector3(-500.0f, 30.0f,  500.0f), CVector3(0.0f, 0.0f, -40.0f), CVector3(15.0f, 15.0f, 15.0f), 70000.0f },
		};

		const static int SPHERE_STATIC_AMOUNT = 10;
		DefaultObjectSettings m_SphereStaticSettings[SPHERE_STATIC_AMOUNT]
		{
			//Position						  Rotation					      Scale						  Mass
			{ CVector3(500.0f, 10.0f, 300.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(5.0f, 5.0f, 5.0f), 10000.0f },
			{ CVector3(500.0f, 35.0f,   -600.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(5.0f, 5.0f, 5.0f), 10000.0f },
			{ CVector3(-600.0f, 0.0f,  600.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(2.0f, 2.0f, 2.0f), 10.0f },
			{ CVector3(-600.0f, 15.0f, -600.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 100000.0f },
			{ CVector3(700.0f, 20.0f, 0.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(10.0f, 10.0f, 10.0f), 10000.0f },
			{ CVector3(-700.0f, 5.0f, 0.0f), CVector3(0.0f, 0.0f,  0.0f), CVector3(5.0f, 5.0f, 5.0f), 50000.0f },
			{ CVector3(800.0f, 10.0f, 700.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(0.2f, 0.2f, 0.2f), 10000.0f },
			{ CVector3(0.0f, 1.0f,  800.0f), CVector3(5.0f, 0.0f,  0.0f), CVector3(20.0f, 20.0f, 20.0f), 400.0f },
			{ CVector3(0.0f, -20.0f,  -900.0f), CVector3(0.0f, 0.0f,   0.0f), CVector3(10.0f, 10.0f, 10.0f), 1000000.0f },
			{ CVector3(-900.0f, 250.0f,  900.0f), CVector3(0.0f, 0.0f, 0.0f), CVector3(15.0f, 15.0f, 15.0f), 70000.0f },
		};

		
		// TEMP
		physx::PxShape* m_TrackShape = nullptr;
		physx::PxRigidStatic* m_Track = nullptr;
		physx::PxConvexMesh* MakeTrack(int index, Entity* entity);

		physx::PxRigidDynamic* m_SphereActor = nullptr;
		physx::PxShape* m_SphereShape = nullptr;

	};
}
