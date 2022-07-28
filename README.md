Github link: https://github.com/KielanT/VehicleSim.git

Executable in Build/Debug-windows-x86_64/KielansVehicleSim 

**** IF PULLED FROM GIT **** 
1) Generate project with the GenerateProject.bat
2) Load VehicleSim.sln
3) Right Click KielansVehicleSim Project and Set as Startup Project (was set in premake didn't work)	
4) Build Solution to create bin folders (project won't work yet)
5) Copy DLLs "VehicleSim\Engine\external\PhysX\lib\debug" to "VehicleSim\bin\Debug-windows-x86_64\KielansVehicleSim"
	DLLs Needed:
	- PhysX_64.dll
	- PhysXCommon_64.dll
	- PhysXCooking_64.dll
	- PhysXFoundation_64.dll
	- PhysXGpu_64.dll
6) Build and Run solution

Note: 
	Build Configuartions for checked and profile have not been set up yet. Only debug and release work


