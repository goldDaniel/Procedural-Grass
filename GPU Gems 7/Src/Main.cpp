#include <Core/SampleApplication.h>

#include <thread>

int main(int argc, char** argv)
{
	std::unique_ptr<Application> app = std::make_unique<SampleApplication>();
	
	app->Run();

	return 0;
}
