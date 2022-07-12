#pragma once

#include "Utility/Lab/Input.h"

namespace Project
{
	// Used to store and set the player controls
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
			accelerate = static_cast<KeyCode>(87);// W
			steerRight = static_cast<KeyCode>(68);// D
			steerLeft = static_cast<KeyCode>(65); // A
			brake = static_cast<KeyCode>(83);	  // S
			gearUp = static_cast<KeyCode>(81);	  // Q
			gearDown = static_cast<KeyCode>(69);  // E
			handBrake = static_cast<KeyCode>(32); // Space
			reset = static_cast<KeyCode>(82);	  // Reset
		}
	};


	class PlayerSettings
	{
	public:
		void SavePlayerControlsToFile(PlayerControls settings);
		bool LoadPlayerControls(PlayerControls& settings);
	};

}
