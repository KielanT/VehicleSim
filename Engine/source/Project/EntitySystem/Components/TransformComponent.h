#pragma once

#include "Project/Core.h"
#include "Project/EntitySystem/Entity.h"
#include "Project/EntitySystem/EntityComponent.h"
#include "Project/EntitySystem/Components/RendererComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"
#include "Math/CVector3.h"
#include "Math/CMatrix4x4.h"

namespace Project
{
	class P_API TransformComponent : public EntityComponent
	{
	public:
		TransformComponent(Entity* entity, TEntityUID UID, CVector3 position = CVector3(0.0f, 0.0f, 0.0f),
			CVector3 rotation = CVector3(0.0f, 0.0f, 0.0f), CVector3 scale = CVector3(1.0f, 1.0f, 1.0f)) : EntityComponent("Transform", UID, entity)
		{
			m_Entity = entity;
			m_Position = position;
			m_Rotation = rotation;
			m_Scale = scale;

			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
			{
				RendererComponent* comp = static_cast<RendererComponent*>(m_Entity->GetComponent("Renderer"));
				comp->GetModel()->SetPosition(m_Position);
				comp->GetModel()->SetRotation(m_Rotation);
				comp->GetModel()->SetScale(m_Scale);
			}
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
			{
				LightRendererComponent* comp = static_cast<LightRendererComponent*>(m_Entity->GetComponent("Light Renderer"));
				comp->GetModel()->SetPosition(m_Position);
				comp->GetModel()->SetRotation(m_Rotation);
				comp->GetModel()->SetScale(m_Scale);
			}

		}

		~TransformComponent();

		void SetPosition(CVector3 pos) { m_Position = pos; }
		void SetRotation(CVector3 rot) { m_Rotation = rot; }
		void SetRotationFromQuat(CVector3 quat, float w) 
		{ 
			CMatrix4x4 m = CMatrix4x4(quat, w);
			m_Rotation = m.GetEulerAngles();
		}
		void SetScale(CVector3 scale) { m_Scale = scale; }

		const CVector3 GetPosition() { return m_Position; }
		const CVector3 GetRotation() { return m_Rotation; }
		const CVector3 GetScale() { return m_Scale; }

		virtual bool Update(float frameTime) override;

	private:
		Entity* m_Entity;
		CVector3 m_Position;
		CVector3 m_Rotation;
		CVector3 m_Scale;

		float m_W;
	};
}