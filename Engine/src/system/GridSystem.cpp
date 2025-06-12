#include "GridSystem.h"  
namespace System
{

	GridSystem::GridSystem(const Components::Grid gr)
	{
		grid = gr;
		InitializeGrid();
	}
	Components::Cell* GridSystem::GetCell(const Vec2 mousePosition)
	{
		const float cellSize = grid.GetCellSize();
		const int gridWidth = grid.GetWidth();
		const int gridHeight = grid.GetHeight();

		// Convert mouse position to grid-relative coordinates
		// Account for the fact that grid is centered at origin
		const float gridWorldWidth = gridWidth * cellSize;
		const float gridWorldHeight = gridHeight * cellSize;

		// Offset mouse position to grid-relative coordinates
		const float relativeX = mousePosition.x + (gridWorldWidth / 2.0f);
		const float relativeY = mousePosition.y + (gridWorldHeight / 2.0f);

		// Convert to grid indices
		const int gridX = static_cast<int>(relativeX / cellSize);
		const int gridY = static_cast<int>(relativeY / cellSize);

		// Bounds checking
		if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
			LOG_INFO("Mouse position out of grid bounds: (" +
				std::to_string(mousePosition.x) + ", " +
				std::to_string(mousePosition.y) + ")");
			LOG_INFO("Grid indices: (" + std::to_string(gridX) + ", " + std::to_string(gridY) + ")");
			return nullptr;
		}

		// Calculate linear index (row-major order)
		const int index = gridY * gridWidth + gridX;

		LOG_INFO("Mouse position: (" + std::to_string(mousePosition.x) + ", " + std::to_string(mousePosition.y) + ")");
		LOG_INFO("Grid coordinates: (" + std::to_string(gridX) + ", " + std::to_string(gridY) + ")");
		LOG_INFO("Cell index: " + std::to_string(index));

		Components::Cell* pCell = grid.GetCell(index);
		return pCell;

	}

	void GridSystem::ResetGridHightlight() {
		for (size_t i = 0; i < grid.GetHeight() * grid.GetWidth(); i++)
		{
			grid.GetCell(i)->isColor = false;
		}
	}

	// Add this method to properly initialize cell positions
	void GridSystem::InitializeGrid()
	{
		const float cellSize = grid.GetCellSize();
		const int gridWidth = grid.GetWidth();
		const int gridHeight = grid.GetHeight();

		// Calculate base position to center the grid
		const float baseX = -(gridWidth * cellSize) / 2.0f;
		const float baseY = -(gridHeight * cellSize) / 2.0f;

		// Initialize each cell with correct world position
		for (int r = 0; r < gridHeight; r++) {
			for (int c = 0; c < gridWidth; c++) {
				int index = r * gridWidth + c;

				// Calculate world position for this cell
				Vec2 cellWorldPos = {
					baseX + (c * cellSize),
					baseY + (r * cellSize)
				};

				Vec2 cellSize2D = { cellSize, cellSize };

				// Get the cell and set its position
				Components::Cell* cell = grid.GetCell(index);
				if (cell) {
					cell->SetPosition(cellWorldPos);
					cell->SetSize(cellSize2D);
				}
			}
		}
	}
	const float& GridSystem::GetGridHeight() {
		return grid.GetHeight() * grid.cellSize;
	}
	const float& GridSystem::GetGridWidth() {
		return grid.GetWidth() * grid.cellSize;
	}

}