#pragma once
union SDL_Event;
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

		// Call once at the end of each frame to clear one-frame states
		void PostUpdate();

		void HandleInput(SDL_Event& event);
		void RegisterAction(Components::InputAction action);

		// Returns true only on the exact frame the action was pressed
		bool IsActionJustPressed(const std::string& actionName) const;
		// Returns true on the frame the button/key went down (and stays true while held)
		bool IsActionPressed(const std::string& actionName) const;
		// Returns true as long as the button/key is held down
		bool IsActionHeld(const std::string& actionName) const;

		// Returns false instead of throwing if the action is not registered
		bool HasAction(const std::string& actionName) const;

		float GetMouseWheelDelta(const std::string& actionName) const;

		bool IsWindowCloseRequested() const
		{
			return m_isWindowCloseRequested;
		}

		const Vec2& GetMousePosition(const std::string& actionName) const;

		void ResetMouseWheelDelta(const std::string& actionName);
		void ResetMousePress(const std::string& actionName);

	private:
		void HandleKeyDown(SDL_Event& event);
		void HandleKeyUp(SDL_Event& event);
		void HandleMouseButtonDown(SDL_Event& event);
		void HandleMouseButtonUp(SDL_Event& event);
		void HandleMouseMotion(SDL_Event& event);
		void HandleMouseWheel(SDL_Event& event);
		void HandleWindowCloseRequest(SDL_Event& event);

		// Safe helper — returns nullptr if not found
		const Components::InputAction* FindAction(const std::string& actionName) const;
		Components::InputAction* FindAction(const std::string& actionName);

		std::unordered_map<std::string, Components::InputAction> m_registeredActions;
		bool m_isWindowCloseRequested = false;

		static const Vec2 s_zeroVec;
	};
}
