#ifndef INPUT_H_
#define INPUT_H_

#include <SDL.h>

#include <functional>
#include <unordered_map>

class Input
{
public:

    Input(const std::function<void()> functor)
    {
        quit_callback = functor;
    }

    bool IsKeyDown(SDL_KeyCode code) 
    {
        return key_down[code];
    }

    bool IsButtonDown(int button)
    {
        return button_down[button];
    }
  
    //relative to top-left
    const glm::vec2 GetMousePos()
    {
        return { mouse_x, mouse_y };
    }

	void Update()
	{
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_QUIT:
                quit_callback();
                break;

            case SDL_KEYDOWN:
                key_down[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                key_down[event.key.keysym.sym] = false;
                break;

            case SDL_MOUSEMOTION:
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
                break;

            case SDL_MOUSEBUTTONDOWN:
                button_down[event.button.button] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                button_down[event.button.button] = false;
                break;
            }
        }
	}

private:
   
    int mouse_x;
    int mouse_y;

    std::function<void()> quit_callback;

    std::unordered_map<int, bool> button_down;

    std::unordered_map<int, bool> key_down;
};

#endif