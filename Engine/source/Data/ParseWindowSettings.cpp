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
	void ParseWindowSettings::SaveWindowSettings(WindowProperties settings)
	{
		std::filesystem::path mainPath = std::filesystem::current_path();
		std::filesystem::path Path = std::filesystem::current_path().parent_path().append("KielansVehicleSim\\data");
		std::filesystem::current_path(Path);

		tinyxml2::XMLDocument doc;

		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.InsertFirstChild(decl);

		tinyxml2::XMLElement* root = doc.NewElement("Window");
		doc.InsertEndChild(root);

		tinyxml2::XMLElement* child = doc.NewElement("Title");
		child->SetAttribute("Name", settings.Title.c_str());
		root->InsertEndChild(child);

		child = doc.NewElement("Size");
		child->SetAttribute("Width", settings.Width);
		child->SetAttribute("Height", settings.Height);
		root->InsertEndChild(child);

		child = doc.NewElement("Renderer");
		std::string renderType = "";
		if (settings.RendererType == ERendererType::DirectX11)
		{
			renderType = "DirectX11";
		}
		child->SetAttribute("Type", renderType.c_str());
		root->InsertEndChild(child);

		child = doc.NewElement("Physics");
		std::string physicsType = "";
		if (settings.PhysicsType == EPhysicsType::PhysX4)
		{
			physicsType = "PhysX4";
		}
		child->SetAttribute("Type", physicsType.c_str());
		root->InsertEndChild(child);

		doc.SaveFile("WinSettings.xml");
		std::filesystem::current_path(mainPath);
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
