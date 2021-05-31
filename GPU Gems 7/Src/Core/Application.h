#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <Core/Core.h>

class Application
{
private:
	SDL_Window* window;
	SDL_GLContext gl_context;

	bool running = true;

protected:

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Input> input;

	int screen_width;
	int screen_height;

	virtual void Render() = 0;
	virtual void Update(float delta) = 0;

public:

	Application();
	~Application();

	void Run();
};

#endif