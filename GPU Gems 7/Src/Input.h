#pragma once

#include <SDL.h>

#include <functional>
#include <unordered_map>

class Input
{
public:

    Input(const std::function<void()> functor)
    {
        quitCallback = functor;
    }

    bool IsKeyDown(SDL_KeyCode code) 
    {
        return keyDown[code];
    }

    bool IsButtonDown(int button)
    {
        return buttonDown[button];
    }
  
    //relative to top-left
    glm::vec2 GetMousePos()
    {
        return { mouseX, mouseY };
    }

	void Update()
	{
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_QUIT:
                quitCallback();
                break;

            case SDL_KEYDOWN:
                keyDown[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                keyDown[event.key.keysym.sym] = false;
                break;

            case SDL_MOUSEMOTION:
                SDL_GetGlobalMouseState(&mouseX, &mouseY);
                break;

            case SDL_MOUSEBUTTONDOWN:
                buttonDown[event.button.button] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                buttonDown[event.button.button] = false;
                break;
            }
        }
	}

private:
   
    int mouseX;
    int mouseY;

    std::function<void()> quitCallback;

    std::unordered_map<int, bool> buttonDown;

    std::unordered_map<int, bool> keyDown;
};