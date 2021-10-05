#include "ppch.h"
#include "TransformComponent.h"

namespace Project
{
	TransformComponent::~TransformComponent()
	{
		delete m_Entity;
	}
	bool TransformComponent::Update(float frameTime)
	{
		if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
		{
			RendererComponent* comp = static_cast<RendererComponent*>(m_Entity->GetComponent("Renderer"));
			comp->GetModel()->SetPosition(m_Position);
			comp->GetModel()->SetRotation(m_Rotation);
			comp->GetModel()->SetScale(m_Scale); 
		}
		else if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
		{
			LightRendererComponent* comp = static_cast<LightRendererComponent*>(m_Entity->GetComponent("Light Renderer"));
			comp->GetModel()->SetPosition(m_Position);
			comp->GetModel()->SetRotation(m_Rotation);
			comp->GetModel()->SetScale(m_Scale);
		}
		return true;
	}
}