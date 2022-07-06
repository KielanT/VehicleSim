#include "ppch.h"
#include "ParseWindowSettings.h"



namespace Project
{
	WindowProperties ParseWindowSettings::LoadWindowSettings(std::string fileName)
	{
		// The tinyXML object XMLDocument will hold the parsed structure and data from the XML file
		tinyxml2::XMLDocument xmlDoc;

		// Open and parse document into tinyxml2 object xmlDoc
		tinyxml2::XMLError error = xmlDoc.LoadFile(fileName.c_str());
		if (error != tinyxml2::XML_SUCCESS) return WindowProperties();

		tinyxml2::XMLElement* element = xmlDoc.FirstChildElement();
		if (element == nullptr) return WindowProperties();

		while (element != nullptr)
		{

			std::string elementName = element->Name();
			if (elementName == "Window")
			{
				tinyxml2::XMLElement* childElement = element->FirstChildElement("Title");
				const tinyxml2::XMLAttribute* attr = childElement->FindAttribute("Name");
				if (attr != nullptr) m_Props.Title = attr->Value();
				

				childElement = element->FirstChildElement("Size");
				attr = childElement->FindAttribute("Width");
				if (attr != nullptr) m_Props.Width = attr->FloatValue();
				attr = childElement->FindAttribute("Height");
				if (attr != nullptr) m_Props.Height = attr->FloatValue();

				childElement = element->FirstChildElement("Renderer");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr) m_Props.RendererType = GetRenderType(attr->Value());

				childElement = element->FirstChildElement("Physics");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr) m_Props.PhysicsType = GetPhysicsType(attr->Value());
			}

			element = element->NextSiblingElement();
		}

		return m_Props;
	}
	ERendererType ParseWindowSettings::GetRenderType(std::string value)
	{
		if (value == "DirectX11")
		{
			return ERendererType::DirectX11;
		}
		else
		{
			return ERendererType::None;
		}

		
	}

	EPhysicsType ParseWindowSettings::GetPhysicsType(std::string value)
	{
		if (value == "PhysX4")
		{
			return EPhysicsType::PhysX4;
		}
		else
		{
			return EPhysicsType::None;
		}


	}
}
