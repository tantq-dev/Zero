#pragma once
#include "Vec2.h"
#include "SDL3/SDL.h"
#include <vector>
#include <string>
#include <cmath>

namespace Components
{
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
		size_t frameIndexStart = 0;
		size_t numberOfFrames = 0;
		bool isLoop = true;
		float frameTime = 0;
	};

	struct Animation {
		AnimationClip currentClip = {};
		size_t currentFrame = 0;
		float currentFrameTime = 0;
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
