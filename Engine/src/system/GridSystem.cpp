#include "GridSystem.h"  
namespace System
{

	GridSystem::GridSystem(const Components::Grid gr)
	{
		grid = gr;
	}
	Components::Cell* GridSystem::GetCell(const Vec2 mousePosition)
	{
		int index = static_cast<int>(mousePosition.x) / grid.GetCellSize() + static_cast<int>(mousePosition.y) / grid.GetCellSize() * grid.GetWidth();
		LOG_INFO("Mouse index: " + std::to_string(index));

		Components::Cell* pCell = grid.GetCell(index);
		return pCell;

	}

	void GridSystem::ResetGridHightlight() {
		for (size_t i = 0; i < grid.GetHeight() * grid.GetWidth(); i++)
		{
			grid.GetCell(i)->isColor = false;
		}
	}

}