#pragma once
#include "utilities/Vec2.h"
#include "SDL3/SDL.h"
#include <utility>
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>
#include "utilities/Logger.h"
#include <stdexcept>
#define MATRIX_2D_INT std::vector<std::vector<int>>
namespace Components
{

	//TODO: add detail file structure to Components.h
	// [Transform]
	// [Velocity]
	// [Collider]
	// [Sprite]
	// [Animation]
	// [Animator]
	// [InputBinding]
	// [InputAction]
	// [Tile]
	// [Grid]
	// [Camera]

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
		SDL_Texture* texture = nullptr;
		int frameCount = 0;
		int currentFrame = 0;
		float frameWidth = 0;
		float frameHeight = 0;
		float currentTime = 0.0f;
		bool loop = true;
		float speed = 1.0f; // Frames per second

		Animation() = default;
		Animation(SDL_Texture* tex, const float frameW, const float frameH, const float spd, const int frame) :
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

		// 	Without union (separate variables):
		// ┌─────────────┬──────────────┐
		// │ SDL_Scancode│ Uint8        │
		// │ (4 bytes)   │ (1 byte)     │
		// └─────────────┴──────────────┘
		// Total: 5 bytes

		// With union:
		// ┌─────────────┐
		// │ Same memory │ ← Can hold either SDL_Scancode OR Uint8
		// │ (4 bytes)   │
		// └─────────────┘
		// Total: 4 bytes

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


	//Grid
	struct Cell
	{
		Vec2 position;
		Vec2 size;
		bool isColor = false; // If true, the tile is colored
		Cell() = default;
		Cell(const Vec2& pos, const Vec2& sz)
			: position(pos), size(sz) {
		}
		[[nodiscard]] Vec2 GetPosition() const { return position; }
		[[nodiscard]] Vec2 GetSize() const { return size; }
		[[nodiscard]] Vec2 GetCenter() const { return position + size * 0.5f; } // Center of the tile
	};

	struct Grid
	{
		std::vector<Cell> cells; // 2D grid of tile indices
		int cellSize = 0;
		int numberCellCol = 0;
		int numberCellRow = 0;
		Grid(int ts, int th, int nc, int nr)
			: cellSize(ts), numberCellCol(nc), numberCellRow(nr)
		{
			for (int i = 0; i < numberCellCol * numberCellRow; i++)
			{
				cells.push_back(
					{
					Vec2{(i % numberCellCol) * cellSize, (i / numberCellCol) * cellSize}, // Calculate position based on index
					Vec2{static_cast<float>(cellSize), static_cast<float>(cellSize)} // Set size
					}
				);
			}
		}

		Cell& GetCell(int index)
		{
			if (index < 0 || index >= cells.size())
			{
				throw std::out_of_range("Tile index out of range: " + std::to_string(index));
			}
			return cells[index];
		}

		[[nodiscard]] int GetCellSize() const
		{
			return cellSize;
		}

		[[nodiscard]] int GetWidth() const
		{
			return numberCellCol;
		}
		[[nodiscard]] int GetHeight() const

		{
			return numberCellRow;
		}

	};

	// Camera 
	struct Camera
	{
		Vec2 position = { 0,0 };
		float zoom = 1.0f;

		[[nodiscard]] Vec2 GetPosition() const {
			return position;
		}

		void SetPosition(const Vec2 p) {
			position = p;
		}

		void Adjust(const Vec2 offset) {
			position += offset;
		}

		[[nodiscard]] float GetZoom() const {
			return zoom;
		}

		void SetZoom(const int z) {
			zoom = z;
		}

		void AdjustZoom(const float z) {
			if (zoom + z <= 0)
			{
				return;
			}

			zoom += z;
		}


	};


}