#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <utility>
#include <variant>
#include <string>
#include <unordered_map>
#include <vector>
#include "Logger.h"
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
	// [Drawable]

	struct Transform2D
	{
		Vec2 position = { 0.0f, 0.0f };
		Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;

		Transform2D() = default;
		Transform2D(const Vec2& pos, const Vec2& scl, float rot)
			: position(pos), scale(scl), rotation(rot)
		{
		}
		Transform2D(const Vec2& pos, const Vec2& s) :position(pos), scale(s) {

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

	struct Size
	{
		float width = 0.0f;
		float height = 0.0f;
		Size() = default;
		Size(float w, float h)
			: width(w), height(h)
		{
		}
	};

	struct Rect {
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
		Rect() = default;
		Rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h)
		{
		}
	};

	struct Texture
	{
		uint32_t id = 0;
		Size size = { 0.0f, 0.0f };
		Texture() = default;
		Texture(uint32_t texID, const Size& sz)
			: id(texID), size(sz)
		{
		}
	};

	struct Sprite
	{
		Texture texture = {};
		bool flipHorizontal = false;
		bool visible = true;
		int layer = 0;
		float opacity = 1;
		Vec2 pivot = { 0.5f, 0.5f }; // Normalized (0 to 1)
		Rect source = { 0, 0, 0, 0 }; //
		SDL_Color tint = { 255, 255, 255, 255 }; // Default white (no tint)
		
		// Spritesheet support
		uint32_t spriteSheetId = 0;
		size_t frameIndex = 0;

		Sprite() = default;
		explicit Sprite(Texture texture)
			: texture(texture)
		{
		}
	};
	

	// Optional: asset describing a sprite sheet (kept in a DB, not a component)
	struct SpriteSheet {
		Texture texture = {};
		std::vector<Rect> frames;      // rectangles per frame
	};

	struct AnimationClip {
		uint32_t spriteSheetId = 0;
		size_t numberOfFrames = 0;
		bool isLoop = true;
		float frameTime = 0;
	};

	struct Animation {
		AnimationClip currentClip = {};
		size_t currentFrame = 0;
		float currentFrameTime = 0;
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


	//Grid
	struct Cell {
		Vec2 position;
		Vec2 size;
		Vec2 gridIndex = { 0,0 };
		bool isColor = false; // If true, the tile is colored
		SDL_Texture* texture = nullptr;


		// Default constructor
		Cell() = default;

		// Parameterized constructor
		Cell(const Vec2& pos, const Vec2& sz, const Vec2& grIndex) : position(pos), size(sz), gridIndex(grIndex) {

		}

		// Constructor with color flag
		Cell(const Vec2& pos, const Vec2& sz, bool colored, const Vec2& grIndex)
			: position(pos), size(sz), isColor(colored), gridIndex(grIndex) {
		}

		// Getters
		[[nodiscard]] Vec2 GetPosition() const { return position; }
		[[nodiscard]] Vec2 GetSize() const { return size; }
		[[nodiscard]] Vec2 GetCenter() const { return position + size * 0.5f; }
		[[nodiscard]] bool IsColored() const { return isColor; }
		[[nodiscard]] Vec2 GetGridIndex() const { return gridIndex; }


		// Setters
		void SetPosition(const Vec2& pos) { position = pos; }
		void SetSize(const Vec2& sz) { size = sz; }
		void SetBorderColored(bool colored) { isColor = colored; }
		void ToggleColor() { isColor = !isColor; }

		// Utility methods
		[[nodiscard]] bool Contains(const Vec2& point) const {
			return point.x >= position.x && point.x <= position.x + size.x &&
				point.y >= position.y && point.y <= position.y + size.y;
		}

		[[nodiscard]] SDL_FRect ToSDLRect() const {
			return SDL_FRect{
				position.x,
				position.y,
				size.x,
				size.y
			};
		}

		// Get bounds
		[[nodiscard]] Vec2 GetTopLeft() const { return position; }
		[[nodiscard]] Vec2 GetTopRight() const { return Vec2{ position.x + size.x, position.y }; }
		[[nodiscard]] Vec2 GetBottomLeft() const { return Vec2{ position.x, position.y + size.y }; }
		[[nodiscard]] Vec2 GetBottomRight() const { return position + size; }

		// Check if this cell overlaps with another
		[[nodiscard]] bool Overlaps(const Cell& other) const {
			return !(position.x + size.x < other.position.x ||
				other.position.x + other.size.x < position.x ||
				position.y + size.y < other.position.y ||
				other.position.y + other.size.y < position.y);
		}

		// Distance from center to another cell's center
		[[nodiscard]] float DistanceTo(const Cell& other) const {
			Vec2 thisCenter = GetCenter();
			Vec2 otherCenter = other.GetCenter();
			Vec2 diff = otherCenter - thisCenter;
			return sqrt(diff.x * diff.x + diff.y * diff.y);
		}

		// Equality operators
		bool operator==(const Cell& other) const {
			return position == other.position && size == other.size && isColor == other.isColor;
		}

		bool operator!=(const Cell& other) const {
			return !(*this == other);
		}
	};

	struct Grid
	{
		std::vector<Cell> cells; // 2D grid of tile indices
		int cellSize = 0;
		int numberCellCol = 0;
		int numberCellRow = 0;
		Grid(int ts, int nc, int nr)
			: cellSize(ts), numberCellCol(nc), numberCellRow(nr)
		{
			for (int i = 0; i < numberCellCol * numberCellRow; i++)
			{
				cells.push_back(
					{
					Vec2{(i % numberCellCol) * cellSize, (i / numberCellCol) * cellSize}, // Calculate position based on index
					Vec2{static_cast<float>(cellSize), static_cast<float>(cellSize)},// Set size
					Vec2{static_cast<float>(ceil(i % numberCellCol)),static_cast<float>(ceil(i / static_cast<int>(numberCellCol)))}
					}
				);
			}
		}

		Cell* GetCell(int index)
		{
			if (index < 0 || index >= cells.size())
			{
				return nullptr;
			}
			return &cells[index];
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
		int width = 0; // Default width
		int height = 0; // Default height

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

		void AdjustZoom(const float z) {
			if (zoom + z <= 0)
			{
				return;
			}
			zoom += z;
		}
	};

}