#pragma once
#include "SDL3/SDL.h"
namespace System
{
	class InputSystem
	{
	public:
		InputSystem() = default;
		~InputSystem() = default;
		void HandleInput(SDL_Event& event);
	};
}


