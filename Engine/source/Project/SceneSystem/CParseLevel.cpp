#include "ppch.h"
#include "CParseLevel.h"

#include "Project/EntitySystem/Components/TransformComponent.h"
#include "Project/EntitySystem/Components/MeshComponent.h"
#include "Project/EntitySystem/Components/RendererComponent.h"
#include "Project/EntitySystem/Components/LightRendererComponent.h"

//TODO: learn serilization and deserilization

namespace Project
{
	// Parse the entire level file and create all the templates and entities inside
	bool CParseLevel::ParseFile(const std::string& fileName)
	{

		// The tinyXML object XMLDocument will hold the parsed structure and data from the XML file
		tinyxml2::XMLDocument xmlDoc;

		std::filesystem::path MainPath = std::filesystem::current_path(); // Gets the current working directory

		// Gets the scene path
		std::filesystem::path scenePath = std::filesystem::current_path().parent_path().append("Engine\\scenes\\");

		std::filesystem::current_path(scenePath); // Sets the current path to the shader path

		// Open and parse document into tinyxml2 object xmlDoc
		tinyxml2::XMLError error = xmlDoc.LoadFile(fileName.c_str());
		std::filesystem::current_path(MainPath); // Resets the path
		if (error != tinyxml2::XML_SUCCESS) return false; 


		// No XML element in the level file means malformed XML or not an XML document at all
		tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
		if (element == nullptr) return false;

		while (element != nullptr)
		{
			// Found a "Scene" tag at the root level, parse it
			std::string elementName = element->Name();
			if (elementName == "Scene")
			{
				ParseLevelElement(element);
			}

			element = element->NextSiblingElement();
		}

		return true;
	}

	// Parse a "Scene" tag within the level XML file
	bool CParseLevel::ParseLevelElement(tinyxml2::XMLElement* rootElement)
	{
		tinyxml2::XMLElement* element = rootElement->FirstChildElement();
		while (element != nullptr)
		{
			// Things expected in a "Scene" tag
			std::string elementName = element->Name();
			if (elementName == "Entities") ParseEntitiesElement(element);

			element = element->NextSiblingElement();
		}

		return true;
	}

	bool CParseLevel::ParseEntitiesElement(tinyxml2::XMLElement* rootElement)
	{
		tinyxml2::XMLElement* element = rootElement->FirstChildElement("Entity");

		while (element != nullptr) 
		{
			const tinyxml2::XMLAttribute* attr = element->FindAttribute("Name");
			if (attr == nullptr) return false;
			std::string name = attr->Value();

			attr = element->FindAttribute("EntityType");
			if (attr == nullptr) return false;
			std::string type = attr->Value();

			if (type == "Default")
			{
				m_EntityManager->CreateEntity(name);
				Entity* newEntity = m_EntityManager->GetEntity(name);

				ParseComponentElement(element, newEntity);
			}
			else if (type == "Model")
			{
				tinyxml2::XMLElement* child = element->FirstChildElement("MeshPath");
				std::string meshPath;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Path");
					if (attr != nullptr) meshPath = attr->Value();
				}
				
				child = element->FirstChildElement("TexturePath");
				std::string texturePath = "media/BasicTex.png"; // Set default
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Path");
					if (attr != nullptr) texturePath = attr->Value();
				}

				
				child = element->FirstChildElement("Transform");
				SEntityTransform transform = SEntityTransform();
				if (child != nullptr)
				{
					tinyxml2::XMLElement* nextChild = child->FirstChildElement("Position");
					if (nextChild != nullptr) transform.Position = GetVector3FromElement(nextChild);
				
					nextChild = child->FirstChildElement("Rotation");
					if (nextChild != nullptr) transform.Rotation = GetVector3FromElement(nextChild);
				
					nextChild = child->FirstChildElement("Scale");
					if (nextChild != nullptr) transform.Scale = GetVector3FromElement(nextChild);
				
				}

				child = element->FirstChildElement("PixelShader");
				EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Shader");
					if (attr != nullptr) pixelShader = GetPixelShader(attr->Value());
				}

				child = element->FirstChildElement("VertexShader");
				EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Shader");
					if (attr != nullptr) vertexShader = GetVertexShader(attr->Value());
				}

				child = element->FirstChildElement("BlendState");
				EBlendState blendState = EBlendState::NoBlendingState;
				if (child != nullptr) 
				{ 
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Blend");
					if (attr != nullptr) blendState = GetBlendState(child->Value());
				}

				child = element->FirstChildElement("DepthStencilState");
				EDepthStencilState depthStencil = EDepthStencilState::UseDepthBufferState;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("DepthStencil");
					if (attr != nullptr) depthStencil = GetDepthStencilState(child->Value());
				}

				child = element->FirstChildElement("CullState");
				ERasterizerState rasterizer = ERasterizerState::CullNoneState;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Cull");
					if (attr != nullptr) rasterizer = GetRasterizerState(child->Value());
				}

				child = element->FirstChildElement("SamplerState");
				ESamplerState sampler = ESamplerState::Anisotropic4xSampler;
				if (child != nullptr)
				{
					const tinyxml2::XMLAttribute* attr = child->FindAttribute("Sampler");
					if (attr != nullptr) sampler = GetSamplerState(child->Value());
				}

				m_EntityManager->CreateModelEntity(name, meshPath, texturePath, transform, pixelShader, vertexShader,
					blendState, depthStencil, rasterizer, sampler);
				Entity* newEntity = m_EntityManager->GetEntity(name);

				ParseComponentElement(element, newEntity);
			}
			else if (type == "Light")
			{
				m_EntityManager->CreateLightEntity(name);
				Entity* newEntity = m_EntityManager->GetEntity(name);

				ParseComponentElement(element, newEntity);
			}
			else
			{
				return false;
			}

			element = element->NextSiblingElement("Entity");
		}

		return true;
	}

	bool CParseLevel::ParseComponentElement(tinyxml2::XMLElement* rootElement, Entity* entity)
	{
		tinyxml2::XMLElement* element = rootElement->FirstChildElement("Component");
		while (element != nullptr)
		{
			const tinyxml2::XMLAttribute* attr = element->FindAttribute("Type");
			if (attr == nullptr) return false;
			std::string type = attr->Value();

			EntityComponent* component = nullptr;

			if (type == "Transform")
			{
				// Default Values if not set
				CVector3 pos{ 0, 0, 0 };
				CVector3 rot{ 0, 0, 0 };
				CVector3 scale{ 1, 1, 1 };

				tinyxml2::XMLElement* child = element->FirstChildElement("Position");
				if (child != nullptr) pos = GetVector3FromElement(child);

				child = element->FirstChildElement("Rotation");
				if (child != nullptr) rot = GetVector3FromElement(child);

				child = element->FirstChildElement("Scale");
				if (child != nullptr) scale = GetVector3FromElement(child);

				component = new TransformComponent(entity, m_EntityManager->GetNewUID(), pos, rot, scale);
				entity->AddComponent(component);
			}
			else if (type == "Mesh")
			{
				tinyxml2::XMLElement* child = element->FirstChildElement("MeshPath");
				std::string path;
				if (child != nullptr) path = child->Value();

				component = new MeshComponent(path, entity, m_EntityManager->GetNewUID());
				entity->AddComponent(component);
			}
			// TODO: set the EntityManager to get the IRenderer
			/*else if (type == "Renderer") // The renderers require to take an IRenderer object 
			{
				
			}
			else if (type == "LightRenderer")
			{
				
			}*/
			else
			{
				return false;
			}

			element = element->NextSiblingElement("Component");
		}

		return true;
	}

	CVector3 CParseLevel::GetVector3FromElement(tinyxml2::XMLElement* rootElement)
	{
		CVector3 vector{ 0, 0, 0 };

		const tinyxml2::XMLAttribute* attr = rootElement->FindAttribute("X");
		if (attr != nullptr) vector.x = attr->FloatValue();

		attr = rootElement->FindAttribute("Y");
		if (attr != nullptr) vector.y = attr->FloatValue();

		attr = rootElement->FindAttribute("Z");
		if (attr != nullptr) vector.z = attr->FloatValue();

		return vector;
	}

	EPixelShader CParseLevel::GetPixelShader(std::string value)
	{
		if (value == "PixelLighting")
		{
			return EPixelShader::PixelLightingPixelShader;
		}
		else if (value == "LightModel")
		{
			return EPixelShader::LightModelPixelShader;
		}
	}
	EVertexShader CParseLevel::GetVertexShader(std::string value)
	{
		if (value == "BasicTransform")
		{
			return EVertexShader::BasicTransformVertexShader;
		}
		else if (value == "PixelLighting")
		{
			return EVertexShader::PixelLightingVertexShader;
		}
		else if (value == "Skinning")
		{
			return EVertexShader::SkinningVertexShader;
		}
	}

	EBlendState CParseLevel::GetBlendState(std::string value)
	{
		if (value == "Additive")
		{
			return EBlendState::AdditiveBlendingState;
		}
		else if (value == "None")
		{
			return EBlendState::NoBlendingState;
		}
	}

	EDepthStencilState CParseLevel::GetDepthStencilState(std::string value)
	{
		if (value == "DepthReadOnly")
		{
			return EDepthStencilState::DepthReadOnlyState;
		}
		else if (value == "None")
		{
			EDepthStencilState::NoDepthBufferState;
		}
		else if (value == "UseDepthBuffer")
		{
			EDepthStencilState::UseDepthBufferState;
		}
	}

	ERasterizerState CParseLevel::GetRasterizerState(std::string value)
	{
		if (value == "Back")
		{
			return ERasterizerState::CullBackState;
		}
		else if (value == "Front")
		{
			ERasterizerState::CullFrontState;
		}
		else if (value == "None")
		{
			ERasterizerState::CullNoneState;
		}
	}

	ESamplerState CParseLevel::GetSamplerState(std::string value)
	{
		if (value == "Anisotropic4x")
		{
			return ESamplerState::Anisotropic4xSampler;
		}
		else if (value == "Point")
		{
			ESamplerState::PointSampler;
		}
		else if (value == "Trilinear")
		{
			ESamplerState::TrilinearSampler;
		}
	}
}

