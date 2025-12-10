#pragma once
#include "SDL3/SDL.h"
#include "Vec2.h"
#include <string>
#include <vector>

namespace Components
{
	struct InputBinding
	{
		enum class Type
		{
			Keyboard,
			MouseButton,
			MouseMotion,
			MouseWheel
		};

		Type type;
		union
		{
			SDL_Scancode scancode; // For keyboard
			Uint8 mouseButton;	   // For mouse buttons (SDL_BUTTON_LEFT, etc.)
		};

		// Private constructor
	private:
		InputBinding(Type t) : type(t) {} // named constructor to enforce type

	public:
		// Named static factory methods
		static InputBinding Keyboard(SDL_Scancode key)
		{
			InputBinding binding(Type::Keyboard);
			binding.scancode = key;
			return binding;
		}

		static InputBinding MouseButton(Uint8 button)
		{
			InputBinding binding(Type::MouseButton);
			binding.mouseButton = button;
			return binding;
		}

		static InputBinding MouseMotion()
		{
			return InputBinding(Type::MouseMotion);
		}

		static InputBinding MouseWheel()
		{
			return InputBinding(Type::MouseWheel);
		}
	};

	struct InputAction
	{
		std::string name;
		std::vector<InputBinding> bindings;
		bool isPressed = false;
		bool isHeld = false;
		bool isJustPressed = false; // True only on the frame the action is pressed

		Vec2 mousePosition = { 0.0f, 0.0f };
		float mouseWheelDelta = 0.0f; // For mouse wheel input
		bool hasMouseMotion = false;

		InputAction() = default;
		explicit InputAction(std::string  actionName) : name(std::move(actionName)) {}
		void AddBinding(SDL_Scancode scancode)
		{
			bindings.push_back(InputBinding::Keyboard(scancode));
		}

		void AddMouseButtonBinding(Uint8 mouseButton)
		{
			bindings.push_back(InputBinding::MouseButton(mouseButton));
		}

		void AddMouseMotionBinding()
		{
			hasMouseMotion = true;
			bindings.push_back(InputBinding::MouseMotion());
		}

		void AddMouseWheelBinding()
		{
			bindings.push_back(InputBinding::MouseWheel());
		}
		std::string GetName() const
		{
			return name;
		}
	};
}
