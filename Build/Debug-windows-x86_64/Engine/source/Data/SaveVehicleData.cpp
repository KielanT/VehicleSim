#include "ppch.h"
#include "SaveVehicleData.h"
#include "Misc/tinyxml2.h"

namespace Project
{
	void SaveVehicleData::SaveVehicleDataToFile(VehicleSettings settings)
	{
		// Creates a new document to write to (will Overwrite existing one)
		tinyxml2::XMLDocument doc;
		
		// Sets the version for the xml doc
		tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
		doc.InsertFirstChild(decl);

		// Creates a new root and adds it to the file
		tinyxml2::XMLElement* root = doc.NewElement("VehicleSettings");
		doc.InsertEndChild(root);

		// Creates new child object with attributes and adds it to the file
		tinyxml2::XMLElement* child = doc.NewElement("Basic");
		child->SetAttribute("Auto", settings.GetAuto());
		child->SetAttribute("ChassisMass", settings.GetChassisMass());
		child->SetAttribute("WheelMass", settings.GetWheelMass());
		child->SetAttribute("MaxSteer", settings.GetMaxSteer());
		root->InsertEndChild(child);

		child = doc.NewElement("Tires");
		child->SetAttribute("Type", settings.GetTireType());
		root->InsertEndChild(child);

		child = doc.NewElement("ToeAngle");
		child->SetAttribute("FL", settings.GetToeAngle().frontLeftAngle);
		child->SetAttribute("FR", settings.GetToeAngle().frontRightAngle);
		child->SetAttribute("RL", settings.GetToeAngle().rearLeftAngle);
		child->SetAttribute("RR", settings.GetToeAngle().rearRightAngle);
		root->InsertEndChild(child);

		child = doc.NewElement("Differential");
		child->SetAttribute("Type", settings.GetDiff().mType);
		child->SetAttribute("FrontBias", settings.GetDiff().mFrontBias);
		child->SetAttribute("RearBias", settings.GetDiff().mRearBias);
		child->SetAttribute("CentreBias", settings.GetDiff().mCentreBias);
		child->SetAttribute("FrontRearSplit", settings.GetDiff().mFrontRearSplit);
		child->SetAttribute("FrontLeftRightSplit", settings.GetDiff().mFrontLeftRightSplit);
		child->SetAttribute("RearLeftRightSplit", settings.GetDiff().mRearLeftRightSplit);
		root->InsertEndChild(child);

		child = doc.NewElement("Engine");
		child->SetAttribute("PeakTorque", settings.GetEngine().mPeakTorque);
		child->SetAttribute("MaxRpm", settings.GetEngine().mMaxOmega);
		root->InsertEndChild(child);

		child = doc.NewElement("Gears");
		child->SetAttribute("SwitchTime", settings.GetGears().mSwitchTime);
		child->SetAttribute("NumGears", settings.GetGears().mNbRatios);
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
		childFL->SetAttribute("CamberAtRest", settings.GetSuspension(0).mCamberAtRest);
		childFL->SetAttribute("CamberAtMaxDroop", settings.GetSuspension(0).mCamberAtMaxDroop);
		childFL->SetAttribute("CamberAtMaxCompression", settings.GetSuspension(0).mCamberAtMaxCompression);
		child->InsertEndChild(childFL);

		tinyxml2::XMLElement* childFR = doc.NewElement("FrontRight");
		childFR->SetAttribute("MaxCompression", settings.GetSuspension(1).mMaxCompression);
		childFR->SetAttribute("MaxDroop", settings.GetSuspension(1).mMaxDroop);
		childFR->SetAttribute("SpringStength", settings.GetSuspension(1).mSpringStrength);
		childFR->SetAttribute("SpringDamperRate", settings.GetSuspension(1).mSpringDamperRate);
		childFR->SetAttribute("CamberAtRest", -settings.GetSuspension(1).mCamberAtRest);
		childFR->SetAttribute("CamberAtMaxDroop", -settings.GetSuspension(1).mCamberAtMaxDroop);
		childFR->SetAttribute("CamberAtMaxCompression", -settings.GetSuspension(1).mCamberAtMaxCompression);
		child->InsertEndChild(childFR);

		tinyxml2::XMLElement* childRL = doc.NewElement("RearLeft");
		childRL->SetAttribute("MaxCompression", settings.GetSuspension(2).mMaxCompression);
		childRL->SetAttribute("MaxDroop", settings.GetSuspension(2).mMaxDroop);
		childRL->SetAttribute("SpringStength", settings.GetSuspension(2).mSpringStrength);
		childRL->SetAttribute("SpringDamperRate", settings.GetSuspension(2).mSpringDamperRate);
		childRL->SetAttribute("CamberAtRest", settings.GetSuspension(2).mCamberAtRest);
		childRL->SetAttribute("CamberAtMaxDroop", settings.GetSuspension(2).mCamberAtMaxDroop);
		childRL->SetAttribute("CamberAtMaxCompression", settings.GetSuspension(2).mCamberAtMaxCompression);
		child->InsertEndChild(childRL);

		tinyxml2::XMLElement* childRR = doc.NewElement("RearRight");
		childRR->SetAttribute("MaxCompression", settings.GetSuspension(3).mMaxCompression);
		childRR->SetAttribute("MaxDroop", settings.GetSuspension(3).mMaxDroop);
		childRR->SetAttribute("SpringStength", settings.GetSuspension(3).mSpringStrength);
		childRR->SetAttribute("SpringDamperRate", settings.GetSuspension(3).mSpringDamperRate);
		childRR->SetAttribute("CamberAtRest", -settings.GetSuspension(3).mCamberAtRest);
		childRR->SetAttribute("CamberAtMaxDroop", -settings.GetSuspension(3).mCamberAtMaxDroop);
		childRR->SetAttribute("CamberAtMaxCompression", -settings.GetSuspension(3).mCamberAtMaxCompression);
		child->InsertEndChild(childRR);
		root->InsertEndChild(child);

		child = doc.NewElement("AntiRollbar");
		child->SetAttribute("FrontStiffness", settings.GetFrontAntiRollBarStiffness());
		child->SetAttribute("RearStiffness", settings.GetRearAntiRollBarStiffness());
		root->InsertEndChild(child);



		doc.SaveFile(m_Path.c_str()); // Saves the file in the correct place

	}
	bool SaveVehicleData::LoadVehicleData(VehicleSettings& settings)
	{
		// Creates a document
		tinyxml2::XMLDocument doc;

		// Loads and checks if the file exists
		tinyxml2::XMLError error = doc.LoadFile(m_Path.c_str());
		if (error != tinyxml2::XML_SUCCESS) return false;

		// Gets the first element to start reading from
		tinyxml2::XMLElement* element = doc.FirstChildElement();
		if (element == nullptr) return false;

		while (element != nullptr) // Loops until now more elements in the file
		{
			std::string elementName = element->Name();
			if (elementName == "VehicleSettings") // Checks its reading in from the correct element
			{
				// Get the basic details first and reads in the settings and adds it to the settings
				tinyxml2::XMLElement* childElement = element->FirstChildElement("Basic"); 
				const tinyxml2::XMLAttribute* attr = childElement->FindAttribute("Auto");
				if (attr != nullptr) settings.SetAutomatic(attr->BoolValue());
				 attr = childElement->FindAttribute("ChassisMass");
				if (attr != nullptr) settings.SetChassisMass(attr->FloatValue());
				attr = childElement->FindAttribute("WheelMass");
				if (attr != nullptr) settings.SetWheelMass(attr->FloatValue());
				attr = childElement->FindAttribute("MaxSteer");
				if (attr != nullptr) settings.SetMaxSteer(attr->FloatValue());

				childElement = element->FirstChildElement("Tires");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr) settings.SetTires(attr->IntValue());

				ToeAngles angles;
				childElement = element->FirstChildElement("ToeAngle");
				attr = childElement->FindAttribute("FL");
				if (attr != nullptr) angles.frontLeftAngle = attr->FloatValue();
				attr = childElement->FindAttribute("FR");
				if (attr != nullptr) angles.frontRightAngle = attr->FloatValue();
				attr = childElement->FindAttribute("RL");
				if (attr != nullptr) angles.rearLeftAngle = attr->FloatValue();
				attr = childElement->FindAttribute("RR");
				if (attr != nullptr) angles.rearRightAngle = attr->FloatValue();
				settings.SetToeAngle(angles);


				physx::PxVehicleDifferential4WData data;
				childElement = element->FirstChildElement("Differential");
				attr = childElement->FindAttribute("Type");
				if (attr != nullptr) data.mType = static_cast<physx::PxVehicleDifferential4WData::Enum>(attr->IntValue());
				attr = childElement->FindAttribute("FrontBias");
				if (attr != nullptr) data.mFrontBias = attr->FloatValue();
				attr = childElement->FindAttribute("RearBias");
				if (attr != nullptr) data.mRearBias = attr->FloatValue();
				attr = childElement->FindAttribute("CentreBias");
				if (attr != nullptr) data.mCentreBias = attr->FloatValue();
				attr = childElement->FindAttribute("FrontRearSplit");
				if (attr != nullptr) data.mFrontRearSplit = attr->FloatValue();
				attr = childElement->FindAttribute("FrontLeftRightSplit");
				if (attr != nullptr) data.mFrontLeftRightSplit = attr->FloatValue();
				attr = childElement->FindAttribute("RearLeftRightSplit");
				if (attr != nullptr) data.mRearLeftRightSplit = attr->FloatValue();
				settings.SetDifferential(data);


				childElement = element->FirstChildElement("Engine"); physx::PxVehicleEngineData eData;
				attr = childElement->FindAttribute("PeakTorque");
				if (attr != nullptr) eData.mPeakTorque = attr->FloatValue();
				attr = childElement->FindAttribute("MaxRpm");
				if (attr != nullptr) eData.mMaxOmega = attr->FloatValue();
				settings.SetEngine(eData);

				childElement = element->FirstChildElement("Gears"); physx::PxVehicleGearsData gData;
				attr = childElement->FindAttribute("SwitchTime");
				if (attr != nullptr) gData.mSwitchTime = attr->FloatValue();
				attr = childElement->FindAttribute("NumGears");
				if (attr != nullptr) gData.mNbRatios = attr->FloatValue();
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
				attr = suspElement->FindAttribute("CamberAtRest");
				if (attr != nullptr) sData[0].mCamberAtRest = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxDroop");
				if (attr != nullptr) sData[0].mCamberAtMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxCompression");
				if (attr != nullptr) sData[0].mCamberAtMaxCompression = attr->FloatValue();

				suspElement = childElement->FirstChildElement("FrontRight");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[1].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[1].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[1].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[1].mSpringDamperRate = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtRest");
				if (attr != nullptr) sData[1].mCamberAtRest = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxDroop");
				if (attr != nullptr) sData[1].mCamberAtMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxCompression");
				if (attr != nullptr) sData[1].mCamberAtMaxCompression = attr->FloatValue();

				suspElement = childElement->FirstChildElement("RearLeft");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[2].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[2].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[2].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[2].mSpringDamperRate = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtRest");
				if (attr != nullptr) sData[2].mCamberAtRest = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxDroop");
				if (attr != nullptr) sData[2].mCamberAtMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxCompression");
				if (attr != nullptr) sData[2].mCamberAtMaxCompression = attr->FloatValue();

				suspElement = childElement->FirstChildElement("RearRight");
				attr = suspElement->FindAttribute("MaxCompression");
				if (attr != nullptr) sData[3].mMaxCompression = attr->FloatValue();
				attr = suspElement->FindAttribute("MaxDroop");
				if (attr != nullptr) sData[3].mMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringStength");
				if (attr != nullptr) sData[3].mSpringStrength = attr->FloatValue();
				attr = suspElement->FindAttribute("SpringDamperRate");
				if (attr != nullptr) sData[3].mSpringDamperRate = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtRest");
				if (attr != nullptr) sData[3].mCamberAtRest = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxDroop");
				if (attr != nullptr) sData[3].mCamberAtMaxDroop = attr->FloatValue();
				attr = suspElement->FindAttribute("CamberAtMaxCompression");
				if (attr != nullptr) sData[3].mCamberAtMaxCompression = attr->FloatValue();

				settings.SetSuspension(sData, 4);

				childElement = element->FirstChildElement("AntiRollbar");
				attr = childElement->FindAttribute("FrontStiffness");
				if (attr != nullptr) settings.SetFrontAntiRollBarStiffness(attr->FloatValue());
				attr = childElement->FindAttribute("RearStiffness");
				if (attr != nullptr) settings.SetRearAntiRollBarStiffness(attr->FloatValue());

			}
			element = element->NextSiblingElement();
		}

		return true;
	}
}