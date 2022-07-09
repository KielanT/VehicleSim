#pragma once

#include "Utility/Lab/Input.h"

namespace Project
{
	struct PlayerControls
	{
		KeyCode accelerate;
		KeyCode steerRight;
		KeyCode steerLeft;
		KeyCode brake;
		KeyCode gearUp;
		KeyCode gearDown;
		KeyCode handBrake;
		KeyCode reset;

		PlayerControls()
		{
			accelerate = static_cast<KeyCode>(87);
			steerRight = static_cast<KeyCode>(68);
			steerLeft = static_cast<KeyCode>(65);
			brake = static_cast<KeyCode>(83);
			gearUp = static_cast<KeyCode>(81);
			gearDown = static_cast<KeyCode>(69);
			handBrake = static_cast<KeyCode>(32);
			reset = static_cast<KeyCode>(82);
		}
	};


	class PlayerSettings
	{
	public:
		void SavePlayerControlsToFile(PlayerControls settings);
		bool LoadPlayerControls(PlayerControls& settings);
	};

}
