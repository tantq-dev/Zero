#include "system/InputSystem.h"
#include "utilities/Logger.h"
#include <string>
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
		case SDL_EVENT_MOUSE_WHEEL:
			HandleMouseWheel(event);
			break;

		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			HandleWindowCloseRequest(event);
			break;

		default:
			break;
		}

	}

	void InputSystem::RegisterAction(Components::InputAction action)
	{
		m_registeredActions.insert_or_assign(action.GetName(), std::move(action));
	}

	bool InputSystem::IsActionPressed(const std::string& actionName) const
	{
		return m_registeredActions.at(actionName).isPressed;
	}

	int InputSystem::GetMouseWheelDelta(const std::string& actionName) const
	{
		auto it = m_registeredActions.find(actionName);
		if (it != m_registeredActions.end())
		{
			return it->second.mouseWheelDelta;
		}
		return 0;
	}

	void InputSystem::HandleKeyDown(SDL_Event& event)
	{
		for (auto& action : m_registeredActions)
		{
			for (auto& binding : action.second.bindings)
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
		for (auto& action : m_registeredActions)
		{
			for (auto& binding : action.second.bindings)
			{
				if (binding.type == Components::InputBinding::Type::MouseButton)
				{
					action.second.isPressed = true;
				}
			}
		}
	}

	void InputSystem::HandleMouseButtonUp(SDL_Event& event)
	{
		for (auto& action : m_registeredActions)
		{
			for (auto& binding : action.second.bindings)
			{
				if (binding.type == Components::InputBinding::Type::MouseButton)
				{
					action.second.isPressed = false;
				}
			}
		}
	}

	void InputSystem::HandleMouseMotion(SDL_Event& event)
	{
		for (auto& action : m_registeredActions)
		{
			if (action.second.hasMouseMotion)
			{
				action.second.mousePosition = { static_cast<float>(event.motion.x), static_cast<float>(event.motion.y) };
			}
		}
	}

	void InputSystem::HandleMouseWheel(SDL_Event& event)
	{
		for (auto& action : m_registeredActions)
		{
			for (auto& binding : action.second.bindings )
			{
				if (binding.type == Components::InputBinding::Type::MouseWheel) {
					action.second.mouseWheelDelta = event.wheel.integer_y;
				}
			}
		}
	}

	void InputSystem::ResetMouseWheelDelta(const std::string& actionName)
	{
		auto it = m_registeredActions.find(actionName);
		if (it != m_registeredActions.end())
		{
			it->second.mouseWheelDelta = 0;
		}
	}

	void InputSystem::HandleWindowCloseRequest(SDL_Event& event)
	{
		m_isWindowCloseRequested = true;
	}

}