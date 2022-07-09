#include "ppch.h"
#include "PlayerSettings.h"
#include "Misc/tinyxml2.h"

namespace Project
{
	void PlayerSettings::SavePlayerControlsToFile(PlayerControls settings)
	{
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.InsertFirstChild(decl);

		tinyxml2::XMLElement* root = doc.NewElement("PlayerControls");
		doc.InsertEndChild(root);

		tinyxml2::XMLElement* child = doc.NewElement("Accelerate");
		child->SetAttribute("Key", settings.accelerate);
		root->InsertEndChild(child);

		child = doc.NewElement("SteerRight");
		child->SetAttribute("Key", settings.steerRight);
		root->InsertEndChild(child);

		child = doc.NewElement("SteerLeft");
		child->SetAttribute("Key", settings.steerLeft);
		root->InsertEndChild(child);

		child = doc.NewElement("Brake");
		child->SetAttribute("Key", settings.brake);
		root->InsertEndChild(child);

		child = doc.NewElement("GearUp");
		child->SetAttribute("Key", settings.gearUp);
		root->InsertEndChild(child);

		child = doc.NewElement("GearDown");
		child->SetAttribute("Key", settings.gearDown);
		root->InsertEndChild(child);

		child = doc.NewElement("HandBrake");
		child->SetAttribute("Key", settings.handBrake);
		root->InsertEndChild(child);

		child = doc.NewElement("Reset");
		child->SetAttribute("Key", settings.reset);
		root->InsertEndChild(child);

		doc.SaveFile("data/ControlSettings.xml");
	}

	bool PlayerSettings::LoadPlayerControls(PlayerControls& settings)
	{
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLError error = doc.LoadFile("data/ControlSettings.xml");
		if (error != tinyxml2::XML_SUCCESS) return false;

		tinyxml2::XMLElement* element = doc.FirstChildElement();
		if (element == nullptr) return false;

		while (element != nullptr)
		{
			std::string elementName = element->Name();
			if (elementName == "PlayerControls")
			{
				tinyxml2::XMLElement* childElement = element->FirstChildElement("Accelerate");
				const tinyxml2::XMLAttribute* attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.accelerate = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("SteerRight");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.steerRight = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("SteerLeft");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.steerLeft = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("Brake");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.brake = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("GearUp");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.gearUp = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("GearDown");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.gearDown = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("HandBrake");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.handBrake = static_cast<KeyCode>(attr->IntValue());

				childElement = element->FirstChildElement("Reset");
				attr = childElement->FindAttribute("Key");
				if (attr != nullptr) settings.reset = static_cast<KeyCode>(attr->IntValue());
			}
			element = element->NextSiblingElement();
		}
		return true;
	}
}