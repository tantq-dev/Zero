#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <utility>
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>
namespace Components
{

	/**
	* @brief Transform component that defines an entity's position, scale, and rotation in 2D space.
	*
	* This component is used to represent the spatial properties of an entity:
	* - Position: The entity's location in 2D space
	* - Scale: The entity's size multiplier in both X and Y directions
	* - Rotation: The entity's orientation in radians
	*/
	struct Transform
	{
		Vec2 position = { 0.0f, 0.0f };
		Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		Transform() = default;
		Transform(const Vec2& pos, const Vec2& scl, float rot)
			: position(pos), scale(scl), rotation(rot)
		{
		}
	};

	struct Velocity
	{
		Vec2 velocity = { 0.0f, 0.0f };
		Velocity() = default;
		explicit Velocity(const Vec2& vel)
			: velocity(vel)
		{
		}
	};

	enum class ColliderType
	{
		Circle, Box
	};

	struct BoxCollider
	{
		Vec2 size;
	};

	struct CircleCollider
	{
		float radius;
	};

	struct Collider
	{
		ColliderType type;
		std::variant<BoxCollider, CircleCollider> data;
		bool isColliding = false;
		static Collider MakeBox(const Vec2& size)
		{
			return { ColliderType::Box, BoxCollider{size} };
		}

		static Collider MakeCircle(float radius)
		{
			return { ColliderType::Circle, CircleCollider{radius} };
		}

		[[nodiscard]] const BoxCollider* AsBox() const
		{
			return type == ColliderType::Box ? &std::get<BoxCollider>(data) : nullptr;
		}

		[[nodiscard]] const CircleCollider* AsCircle() const
		{
			return type == ColliderType::Circle ? &std::get<CircleCollider>(data) : nullptr;
		}
	};

	struct Sprite
	{
		SDL_Texture* texture = nullptr;
		bool flipHorizontal = false;
		float rotation = 0.0f; // Rotation in radians
		Sprite() = default;
		explicit Sprite(SDL_Texture* tex, float rot = 0.0f)
			: texture(tex)
		{
		}
	};
	struct Animation
	{
		SDL_Texture* texture = nullptr ;
		int frameCount = 0;
		int currentFrame = 0;
		float frameWidth = 0;
		float frameHeight = 0;
		float currentTime = 0.0f;
		bool loop = true;
		float speed = 1.0f; // Frames per second

		Animation() = default;
		Animation(SDL_Texture * tex ,const float frameW, const float frameH, const float spd, const int frame ) :
			texture(tex), frameCount(frame), currentFrame(0), frameWidth(frameW), frameHeight(frameH), currentTime(0.0f), loop(true), speed(spd)
		{

		}

	};

	struct Animator
	{
		std::string currentAnimation;
		std::unordered_map<std::string, Animation> animations;
		void AddAnimation(const std::string& name, const Animation& anim)
		{
			animations[name] = anim;
		}
		void SetCurrentAnimation(const std::string& name)
		{
			if (animations.contains(name))
			{
				currentAnimation = name;
			}
		}
		Animation* GetCurrentAnimation()
		{
			const auto it = animations.find(currentAnimation);
			return it != animations.end() ? &it->second : nullptr;
		}
	};
	enum class InputEventType
	{
		KeyDown,
		KeyUp,
		MouseButtonDown,
		MouseButtonUp,
		MouseMotion,
		MouseWheel
	};

	struct InputBinding
	{
		enum class Type
    {
        Keyboard,
        MouseButton,
        MouseMotion
    };
    
    Type type;
    union
    {
        SDL_Scancode scancode;      // For keyboard
        Uint8 mouseButton;          // For mouse buttons (SDL_BUTTON_LEFT, etc.)
    };
    
    // Constructors
    explicit InputBinding(SDL_Scancode key) 
        : type(Type::Keyboard), scancode(key) {}
    
    explicit InputBinding(Uint8 button, bool /*mouse_tag*/) 
        : type(Type::MouseButton), mouseButton(button) {}
    
    static InputBinding MouseMotion() 
    { 
        InputBinding binding(SDL_SCANCODE_UNKNOWN);
        binding.type = Type::MouseMotion;
        return binding;
    }
	
	};

	struct InputAction
	{
		std::string name;
		std::vector<InputBinding> bindings;
		bool isPressed = false;
		bool isHeld = false;

		Vec2 mousePosition = { 0.0f, 0.0f };
		Vec2 mouseDelta = { 0.0f, 0.0f };

		bool hasMouseMotion = false;

		InputAction() = default;
		explicit InputAction(std::string  actionName) : name(std::move(actionName)) {}
		void AddBinding(SDL_Scancode scancode)
		{
			bindings.emplace_back(scancode);
		}
		void AddMouseButtonBinding(SDL_Scancode scancode)
		{
			bindings.emplace_back(scancode);
		}

	};



}