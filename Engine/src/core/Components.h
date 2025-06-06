#pragma once
#include "utilities/Vec2.h"
#include "SDL3/SDL.h"
#include <utility>
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>
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
	// [Tilemap]
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
			MouseMotion
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
		std::string GetName() const
		{
			return name;
		}
	};


	//TileMap
	struct Tile
	{
		Vec2 position;
		Vec2 size;
	};

	struct Tilemap
	{
		MATRIX_2D_INT tiles; // 2D grid of tile indices
		int tileWidth = 0;
		int tileHeight = 0;
		int mapWidth = 0;
		int mapHeight = 0;
		Tilemap(int tw, int th, int mw, int mh)
			: tileWidth(tw), tileHeight(th), mapWidth(mw), mapHeight(mh)
		{
			tiles.resize(mapHeight, std::vector<int>(mapWidth, -1)); // Initialize with -1 (no tile)
		}
		void SetTile(int x, int y, int tileIndex)
		{
			if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
			{
				tiles[y][x] = tileIndex;
			}
		}
		int GetTile(int x, int y) const
		{
			if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
			{
				return tiles[y][x];
			}
			return -1; // Invalid tile index
		}

		void Clear()
		{
			for (auto& row : tiles)
			{
				std::fill(row.begin(), row.end(), -1); // Reset all tiles to -1
			}
		}

		[[nodiscard]] Vec2 GetTilePosition(int x, int y) const
		{
			return { static_cast<float>(x * tileWidth), static_cast<float>(y * tileHeight) };
		}
		[[nodiscard]] MATRIX_2D_INT GetTiles() const
		{
			return tiles;
		}
		[[nodiscard]] int GetTileWidth() const
		{
			return tileWidth;
		}
		[[nodiscard]] int GetTileHeight() const
		{
			return tileHeight;
		}
		[[nodiscard]] int GetMapWidth() const
		{
			return mapWidth;
		}
		[[nodiscard]] int GetMapHeight() const
		{
			return mapHeight;
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

		void SetZoom(const float z) {
			zoom = z;
		}

		void ZoomIn(const float z) {
			zoom += z;
		}

		void ZoomOut(const float z) {
			zoom -= z;
		}


	};


}