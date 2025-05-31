#include "InputSystem.h"
namespace System
{
	void InputSystem::HandleInput(SDL_Event& event)
	{
		switch (event.type)
		{
		case SDL_EVENT_KEY_DOWN:
			HandleKeyDown(event);
			break;
		case SDL_EVENT_KEY_UP:
			HandleKeyUp(event);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			HandleMouseButtonDown(event);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			HandleMouseButtonUp(event);
			break;
		case SDL_EVENT_MOUSE_MOTION:
			HandleMouseMotion(event);
			break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			HandleWindowCloseRequest(event);
			break;
		default:
			break;
		}
	
	}

	void InputSystem::RegisterAction(const std::string& actionName)
	{
		m_registeredActions.insert_or_assign(actionName, Components::InputAction(actionName));
	}

	void InputSystem::BindingToAction(const std::string& actionName, Components::InputBinding binding)
	{
		m_registeredActions[actionName].bindings.push_back(binding);
	}

	bool InputSystem::IsActionPressed(const std::string& actionName) const
	{
		return m_registeredActions.at(actionName).isPressed;
	}

	void InputSystem::HandleKeyDown(SDL_Event& event)
	{
		for (auto &action: m_registeredActions)
		{
			for (auto &binding: action.second.bindings)
			{
				if (event.key.scancode == binding.scancode)
				{
					action.second.isPressed = true;
				}
			}
		}
	}

	void InputSystem::HandleKeyUp(SDL_Event& event)
	{
		for (auto& action : m_registeredActions)
		{
			for (auto& binding : action.second.bindings)
			{
				if (event.key.scancode == binding.scancode)
				{
					action.second.isPressed = false;
				}
			}
		}
	}

	void InputSystem::HandleMouseButtonDown(SDL_Event& event)
	{

	}

	void InputSystem::HandleMouseButtonUp(SDL_Event& event)
	{

	}

	void InputSystem::HandleMouseMotion(SDL_Event& event)
	{

	}

	void InputSystem::HandleWindowCloseRequest(SDL_Event& event)
	{
		m_isWindowCloseRequested = true;
	}

}