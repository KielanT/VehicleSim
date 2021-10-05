#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"

#include "Project/SceneSystem/CParseLevel.h"

namespace Project
{
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

	private:
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
	};
}
