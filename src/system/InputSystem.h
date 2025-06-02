#pragma once
#include "SDL3/SDL.h"
#include <unordered_map>
#include <string>
#include "Components.h"
namespace System
{
	class InputSystem
	{
	public:
		InputSystem() = default;
		~InputSystem() = default;
		void HandleInput(SDL_Event& event);
		void RegisterAction(const std::string& actionName);
		void BindingToAction(const std::string& actionName, Components::InputBinding binding);
		bool IsActionPressed(const std::string& actionName) const;
		const bool IsWindowCloseRequested() const
		{
			return m_isWindowCloseRequested;
		}
	private:
		void HandleKeyDown(SDL_Event& event);
		void HandleKeyUp(SDL_Event& event);
		void HandleMouseButtonDown(SDL_Event& event);
		void HandleMouseButtonUp(SDL_Event& event);
		void HandleMouseMotion(SDL_Event& event);
		void HandleWindowCloseRequest(SDL_Event& event);
		std::unordered_map<std::string, Components::InputAction> m_registeredActions;
		bool m_isWindowCloseRequested = false;
	};
}
