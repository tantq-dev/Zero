#include "InputSystem.h"
#include "Logger.h"
#include <string>

namespace System
{
	// -----------------------------------------------------------------------
	// Static fallback
	// -----------------------------------------------------------------------
	const Vec2 InputSystem::s_zeroVec = { 0.0f, 0.0f };

	// -----------------------------------------------------------------------
	// Safe internal helpers
	// -----------------------------------------------------------------------
	const Components::InputAction* InputSystem::FindAction(const std::string& actionName) const
	{
		auto it = m_registeredActions.find(actionName);
		if (it != m_registeredActions.end())
			return &it->second;
		LOG_INFO("InputSystem: action not registered: " + actionName);
		return nullptr;
	}

	Components::InputAction* InputSystem::FindAction(const std::string& actionName)
	{
		auto it = m_registeredActions.find(actionName);
		if (it != m_registeredActions.end())
			return &it->second;
		LOG_INFO("InputSystem: action not registered: " + actionName);
		return nullptr;
	}

	// -----------------------------------------------------------------------
	// Registration
	// -----------------------------------------------------------------------
	void InputSystem::RegisterAction(Components::InputAction action)
	{
		m_registeredActions.insert_or_assign(action.GetName(), std::move(action));
	}

	bool InputSystem::HasAction(const std::string& actionName) const
	{
		return m_registeredActions.count(actionName) > 0;
	}

	// -----------------------------------------------------------------------
	// Public query API (all crash-safe)
	// -----------------------------------------------------------------------
	bool InputSystem::IsActionJustPressed(const std::string& actionName) const
	{
		const auto* action = FindAction(actionName);
		return action ? action->isJustPressed : false;
	}

	bool InputSystem::IsActionPressed(const std::string& actionName) const
	{
		const auto* action = FindAction(actionName);
		return action ? action->isPressed : false;
	}

	bool InputSystem::IsActionHeld(const std::string& actionName) const
	{
		const auto* action = FindAction(actionName);
		return action ? action->isHeld : false;
	}

	float InputSystem::GetMouseWheelDelta(const std::string& actionName) const
	{
		const auto* action = FindAction(actionName);
		return action ? action->mouseWheelDelta : 0.0f;
	}

	const Vec2& InputSystem::GetMousePosition(const std::string& actionName) const
	{
		const auto* action = FindAction(actionName);
		return action ? action->mousePosition : s_zeroVec;
	}

	// -----------------------------------------------------------------------
	// Frame lifecycle
	// -----------------------------------------------------------------------

	// Call this at the END of each frame to clear one-frame states.
	void InputSystem::PostUpdate()
	{
		for (auto& [name, action] : m_registeredActions)
		{
			action.isJustPressed = false;
			action.isPressed     = false;
			action.mouseWheelDelta = 0.0f;
		}
	}

	// -----------------------------------------------------------------------
	// Main event dispatcher
	// -----------------------------------------------------------------------
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

	// -----------------------------------------------------------------------
	// Private handlers
	// -----------------------------------------------------------------------
	void InputSystem::HandleKeyDown(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			for (auto& binding : action.bindings)
			{
				if (binding.type == Components::InputBinding::Type::Keyboard &&
					event.key.scancode == binding.scancode)
				{
					// isJustPressed is true only on the first frame (not while held)
					action.isJustPressed = !action.isHeld;
					action.isPressed     = true;
					action.isHeld        = true;
				}
			}
		}
	}

	void InputSystem::HandleKeyUp(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			for (auto& binding : action.bindings)
			{
				if (binding.type == Components::InputBinding::Type::Keyboard &&
					event.key.scancode == binding.scancode)
				{
					action.isHeld        = false;
					action.isPressed     = false;
					action.isJustPressed = false;
				}
			}
		}
	}

	void InputSystem::HandleMouseButtonDown(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			for (auto& binding : action.bindings)
			{
				if (binding.type == Components::InputBinding::Type::MouseButton &&
					event.button.button == binding.mouseButton)
				{
					action.isJustPressed = !action.isHeld;
					action.isPressed     = true;
					action.isHeld        = true;
				}
			}
		}
	}

	void InputSystem::HandleMouseButtonUp(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			for (auto& binding : action.bindings)
			{
				if (binding.type == Components::InputBinding::Type::MouseButton &&
					event.button.button == binding.mouseButton)
				{
					// FIX: also clear isPressed on release (was missing before)
					action.isHeld        = false;
					action.isPressed     = false;
					action.isJustPressed = false;
				}
			}
		}
	}

	void InputSystem::HandleMouseMotion(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			if (action.hasMouseMotion)
			{
				action.mousePosition = {
					static_cast<float>(event.motion.x),
					static_cast<float>(event.motion.y)
				};
			}
		}
	}

	void InputSystem::HandleMouseWheel(SDL_Event& event)
	{
		for (auto& [name, action] : m_registeredActions)
		{
			for (auto& binding : action.bindings)
			{
				if (binding.type == Components::InputBinding::Type::MouseWheel)
				{
					action.mouseWheelDelta = event.wheel.y; // float in SDL3
				}
			}
		}
	}

	void InputSystem::HandleWindowCloseRequest(SDL_Event& event)
	{
		m_isWindowCloseRequested = true;
	}

	// -----------------------------------------------------------------------
	// Manual reset helpers (kept for backward compatibility)
	// -----------------------------------------------------------------------
	void InputSystem::ResetMouseWheelDelta(const std::string& actionName)
	{
		auto* action = FindAction(actionName);
		if (action)
			action->mouseWheelDelta = 0.0f;
	}

	void InputSystem::ResetMousePress(const std::string& actionName)
	{
		auto* action = FindAction(actionName);
		if (action)
			action->isPressed = false;
	}

} // namespace System