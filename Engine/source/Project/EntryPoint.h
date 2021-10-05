#pragma once

#ifdef P_PLATFORM_WINDOWS

extern Project::Application* Project::CreateApplication(); // Allows the main app to implement the project

// Entry point used by the main project
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
   auto app = Project::CreateApplication(); // Creates an the application
   app->Run(); // Runs the application
   delete app; // Deletes the application
}

#endif