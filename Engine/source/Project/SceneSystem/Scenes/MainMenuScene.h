#pragma once

#include "Project/SceneSystem/IScene.h"
#include "Project/Interfaces/IRenderer.h"
#include "Project/Interfaces/IPhysics.h"
#include "Project/EntitySystem/EntityManager.h"
#include "Project/SceneSystem/CDirectX11SceneManager.h"
#include "Graphics/DirectX11/DirectX11Renderer.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

namespace Project
{
#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))

	class MainMenuScene : public IScene
	{
	public:
		MainMenuScene(CDirectX11SceneManager* sceneManager, IRenderer* renderer, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
			float specularPower = 256.0f, ColourRGBA backgroundColour = ColourRGBA(0.2f, 0.2f, 0.3f, 1.0f),
			bool vsyncOn = true);

		MainMenuScene(CDirectX11SceneManager* sceneManager, IRenderer* renderer, bool enablePhysics, int sceneIndex, CVector3 ambientColour = CVector3(1.0f, 1.0f, 1.0f),
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
		// GUI Functions
		void GUI();
		void MainMenu();
		void GameMode();
		void LoadMaps();
		void VehicleSetup();

		bool m_ShowGameModeSelect = false;
		bool m_IsHotLapSelected = false;
		bool m_IsOpenWorldSelected = false;

		int m_Image_Width = 0;
		int m_Image_Height = 0;
		ID3D11ShaderResourceView* m_Image = nullptr;
		
		int m_CarImage_Width = 0;
		int m_CarImage_Height = 0;
		ID3D11ShaderResourceView* m_CarImage = nullptr;
		
		bool m_Selected = false;
		
		bool m_IsMapSelected = false;
		int m_MapIndex = 0;

		void SelectableColor(ImU32 color)
		{
			ImVec2 p_min = ImGui::GetItemRectMin();
			ImVec2 p_max = ImGui::GetItemRectMax();
			ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
		}

		VehicleSettings m_VehicleSettings;
		int m_ChassisMass;
		int m_WheelMass;
		int m_MaxSteer;
		int m_PeakTorque;
		int m_RPM;
		int m_ClutchStrength;
		
		float m_GearSwitchTime;
		

		physx::PxVehicleDifferential4WData m_Diff;
		physx::PxVehicleEngineData m_Engine;
		physx::PxVehicleGearsData m_Gears;
		physx::PxVehicleClutchData m_Clutch;

		physx::PxVehicleSuspensionData m_Suspension[4];

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

		

	};
}
