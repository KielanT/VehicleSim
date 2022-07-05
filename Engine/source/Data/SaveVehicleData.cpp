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
		tinyxml2::XMLElement* childSuspension = doc.NewElement("Suspension");

		tinyxml2::XMLElement* childFL = doc.NewElement("FrontLeft");
		childFL->SetAttribute("MaxCompression", settings.GetSuspension(0).mMaxCompression);
		childFL->SetAttribute("MaxDroop", settings.GetSuspension(0).mMaxDroop);
		childFL->SetAttribute("SpringStength", settings.GetSuspension(0).mSpringStrength);
		childFL->SetAttribute("SpringDamperRate", settings.GetSuspension(0).mSpringDamperRate);
		childSuspension->InsertEndChild(childFL);

		tinyxml2::XMLElement* childFR = doc.NewElement("FrontRight");
		childFR->SetAttribute("MaxCompression", settings.GetSuspension(1).mMaxCompression);
		childFR->SetAttribute("MaxDroop", settings.GetSuspension(1).mMaxDroop);
		childFR->SetAttribute("SpringStength", settings.GetSuspension(1).mSpringStrength);
		childFR->SetAttribute("SpringDamperRate", settings.GetSuspension(1).mSpringDamperRate);
		childSuspension->InsertEndChild(childFR);

		tinyxml2::XMLElement* childRL = doc.NewElement("RearLeft");
		childRL->SetAttribute("MaxCompression", settings.GetSuspension(2).mMaxCompression);
		childRL->SetAttribute("MaxDroop", settings.GetSuspension(2).mMaxDroop);
		childRL->SetAttribute("SpringStength", settings.GetSuspension(2).mSpringStrength);
		childRL->SetAttribute("SpringDamperRate", settings.GetSuspension(2).mSpringDamperRate);
		childSuspension->InsertEndChild(childRL);

		tinyxml2::XMLElement* childRR = doc.NewElement("RearRight");
		childRR->SetAttribute("MaxCompression", settings.GetSuspension(3).mMaxCompression);
		childRR->SetAttribute("MaxDroop", settings.GetSuspension(3).mMaxDroop);
		childRR->SetAttribute("SpringStength", settings.GetSuspension(3).mSpringStrength);
		childRR->SetAttribute("SpringDamperRate", settings.GetSuspension(3).mSpringDamperRate);
		childSuspension->InsertEndChild(childRR);

		child->InsertEndChild(childSuspension);
		root->InsertEndChild(child);


		doc.SaveFile("data/VehicleData.xml");
	}
}