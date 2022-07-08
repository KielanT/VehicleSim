#include "ppch.h"
#include "SaveVehicleData.h"
#include "Misc/tinyxml2.h"

namespace Project
{
	void SaveVehicleData::SaveVehicleDataToFile(VehicleSettings settings)
	{
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.InsertFirstChild(decl);

		tinyxml2::XMLElement* root = doc.NewElement("VehicleSettings");
		doc.InsertEndChild(root);

		tinyxml2::XMLElement* child = doc.NewElement("Basic");
		child->SetAttribute("ChassisMass", settings.GetChassisMass());
		child->SetAttribute("WheelMass", settings.GetWheelMass());
		child->SetAttribute("MaxSteer", settings.GetMaxSteer());
		root->InsertEndChild(child);

		child = doc.NewElement("Differential");
		child->SetAttribute("Type", settings.GetDiff().mType);
		root->InsertEndChild(child);

		child = doc.NewElement("Engine");
		child->SetAttribute("PeakTorque", settings.GetEngine().mPeakTorque);
		child->SetAttribute("MaxRpm", settings.GetEngine().mMaxOmega);
		root->InsertEndChild(child);

		child = doc.NewElement("Gears");
		child->SetAttribute("SwitchTime", settings.GetGears().mSwitchTime);
		root->InsertEndChild(child);

		child = doc.NewElement("Clutch");
		child->SetAttribute("Strength", settings.GetClutch().mStrength);
		root->InsertEndChild(child);

		child = doc.NewElement("Suspension");
		tinyxml2::XMLElement* childFL = doc.NewElement("FrontLeft");
		childFL->SetAttribute("MaxCompression", settings.GetSuspension(0).mMaxCompression);
		childFL->SetAttribute("MaxDroop", settings.GetSuspension(0).mMaxDroop);
		childFL->SetAttribute("SpringStength", settings.GetSuspension(0).mSpringStrength);
		childFL->SetAttribute("SpringDamperRate", settings.GetSuspension(0).mSpringDamperRate);
		child->InsertEndChild(childFL);

		tinyxml2::XMLElement* childFR = doc.NewElement("FrontRight");
		childFR->SetAttribute("MaxCompression", settings.GetSuspension(1).mMaxCompression);
		childFR->SetAttribute("MaxDroop", settings.GetSuspension(1).mMaxDroop);
		childFR->SetAttribute("SpringStength", settings.GetSuspension(1).mSpringStrength);
		childFR->SetAttribute("SpringDamperRate", settings.GetSuspension(1).mSpringDamperRate);
		child->InsertEndChild(childFR);

		tinyxml2::XMLElement* childRL = doc.NewElement("RearLeft");
		childRL->SetAttribute("MaxCompression", settings.GetSuspension(2).mMaxCompression);
		childRL->SetAttribute("MaxDroop", settings.GetSuspension(2).mMaxDroop);
		childRL->SetAttribute("SpringStength", settings.GetSuspension(2).mSpringStrength);
		childRL->SetAttribute("SpringDamperRate", settings.GetSuspension(2).mSpringDamperRate);
		child->InsertEndChild(childRL);

		tinyxml2::XMLElement* childRR = doc.NewElement("RearRight");
		childRR->SetAttribute("MaxCompression", settings.GetSuspension(3).mMaxCompression);
		childRR->SetAttribute("MaxDroop", settings.GetSuspension(3).mMaxDroop);
		childRR->SetAttribute("SpringStength", settings.GetSuspension(3).mSpringStrength);
		childRR->SetAttribute("SpringDamperRate", settings.GetSuspension(3).mSpringDamperRate);
		child->InsertEndChild(childRR);
		root->InsertEndChild(child);


		doc.SaveFile(m_Path.c_str());

	}
	bool SaveVehicleData::LoadVehicleData(VehicleSettings& settings)
	{
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLError error = doc.LoadFile(m_Path.c_str());
		if (error != tinyxml2::XML_SUCCESS) return false;

		tinyxml2::XMLElement* element = doc.FirstChildElement();
		if (element == nullptr) return false;

		while (element != nullptr)
		{
			std::string elementName = element->Name();
			if (elementName == "VehicleSettings")
			{
				// Get the basic details first
				tinyxml2::XMLElement* childElement = element->FirstChildElement("Basic");
				const tinyxml2::XMLAttribute* attr = childElement->FindAttribute("ChassisMass");
				if (attr != nullptr) settings.SetChassisMass(attr->FloatValue());
				attr = childElement->FindAttribute("WheelMass");
				if (attr != nullptr) settings.SetWheelMass(attr->FloatValue());
				attr = childElement->FindAttribute("MaxSteer");
				if (attr != nullptr) settings.SetMaxSteer(attr->FloatValue());

				childElement = element->FirstChildElement("Differential");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr)
				{
					physx::PxVehicleDifferential4WData data;
					data.mType = static_cast<physx::PxVehicleDifferential4WData::Enum>(attr->IntValue());
					settings.SetDifferential(data);
				}

				childElement = element->FirstChildElement("Engine"); physx::PxVehicleEngineData eData;
				attr = childElement->FindAttribute("PeakTorque");
				if (attr != nullptr) eData.mPeakTorque = attr->FloatValue();
				attr = childElement->FindAttribute("MaxRpm");
				if (attr != nullptr) eData.mMaxOmega = attr->FloatValue();
				settings.SetEngine(eData);

				childElement = element->FirstChildElement("Gears"); physx::PxVehicleGearsData gData;
				attr = childElement->FindAttribute("SwitchTime");
				if (attr != nullptr) gData.mSwitchTime = attr->FloatValue();
				settings.SetGears(gData);

				childElement = element->FirstChildElement("Clutch"); physx::PxVehicleClutchData cData;
				attr = childElement->FindAttribute("Strength");
				if (attr != nullptr) cData.mStrength = attr->FloatValue();
				settings.SetClutch(cData);

				childElement = element->FirstChildElement("Suspension"); physx::PxVehicleSuspensionData sData[4];
				tinyxml2::XMLElement* suspElement = childElement->FirstChildElement("FrontLeft");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[0].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[0].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[0].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[0].mSpringDamperRate = attr->FloatValue();

				suspElement = childElement->FirstChildElement("FrontRight");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[1].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[1].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[1].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[1].mSpringDamperRate = attr->FloatValue();

				suspElement = childElement->FirstChildElement("RearLeft");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[2].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[2].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[2].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[2].mSpringDamperRate = attr->FloatValue();

				suspElement = childElement->FirstChildElement("RearRight");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[3].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[3].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[3].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[3].mSpringDamperRate = attr->FloatValue();

				settings.SetSuspension(sData, 4);

			}
			element = element->NextSiblingElement();
		}

		return true;
	}
}