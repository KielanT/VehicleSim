#pragma once


// Used for exporting and importing the dll so that the two projects work together
#ifdef P_PLATFORM_WINDOWS
	#ifdef  P_BUILD_DLL
		#define P_API __declspec(dllexport)
	#else
		#define P_API __declspec(dllimport)
	#endif 
#endif 