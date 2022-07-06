#pragma once
#include "Project/EntitySystem/Components/Vehicle/VehicleSettings.h"

namespace Project
{
	class SaveVehicleData
	{
	public:
		void SaveVehicleDataToFile(VehicleSettings settings);

		bool LoadVehicleData(VehicleSettings& settings);

		void SetPath(std::string path) { m_Path = path; }

	private:
		std::string m_Path = "data/VehicleData.xml";


	};
}

