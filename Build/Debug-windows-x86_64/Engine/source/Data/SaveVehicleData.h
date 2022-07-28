#pragma once
#include "Project/EntitySystem/Components/Vehicle/VehicleSettings.h"

namespace Project
{
	class SaveVehicleData
	{
	public:
		// Saves vehicle data to file
		void SaveVehicleDataToFile(VehicleSettings settings);

		// Loads the vehicle date
		bool LoadVehicleData(VehicleSettings& settings);

		// Allows the file path to be changed
		void SetPath(std::string path) { m_Path = path; } 

	private:
		std::string m_Path = "data/VehicleData.xml";


	};
}

