#include <Project.h>



class Main : public Project::Application
{
public:
	Main()
	{

	}

	~Main()
	{

	}
};

Project::Application* Project::CreateApplication()
{
	
	return new Main();
}