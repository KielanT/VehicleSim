#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

#include "Utility/Lab/Timer.h"

namespace Project
{
	class TrackSceneOne : public IScene
	{
	public:
		TrackSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		TrackSceneOne(CDirectX11SceneManager* sceneManager, std::shared_ptr<IRenderer> renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		virtual bool InitGeometry() override;

		virtual bool InitScene() override;

		virtual void RenderScene() override;

		virtual void UpdateScene(float frameTime) override;

		virtual void ReleaseResources() override;

		virtual int GetSceneIndex() override { return m_SceneIndex; }

		virtual std::shared_ptr<Camera> GetCamera() override { return m_SceneCamera; }

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
		bool VehicleOverFinishLine();

		void GUI();
		void TimerUI();

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

		std::shared_ptr<EntityManager> m_EntityManager = nullptr;
		std::shared_ptr<EntityManager> m_PhysicsEntityManager = nullptr;
		std::shared_ptr<EntityManager> m_LightEntityManager = nullptr;

		CDirectX11SceneManager* m_sceneManager;

		std::shared_ptr<IPhysics> m_PhysicsSystem = nullptr;

		physx::PxMaterial* m_Material = nullptr;

		std::unique_ptr<Timer> m_Timer;
		int currentMins = 0;
		float currentSeconds = 0;
		float currentTimer = 0;
		float previousTimer = 0;
		float bestLap = 0;

		int test = 0;
	};
}
