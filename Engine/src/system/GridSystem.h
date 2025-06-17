#pragma once
#include "core/Components.h"
#include "utilities/Vec2.h"
namespace System
{
	class GridSystem
	{
	public:
		GridSystem() = default;
		~GridSystem() = default;
		GridSystem(Components::Grid tilesheet);
		const Components::Grid& GetGrid() const { return grid; }
		Components::Cell* GetCell(const Vec2 mousePosition);
		void ResetGridHightlight();
		const float& GetGridHeight();
		const float& GetGridWidth();
		Vec2 CellIndexInGridToWorldPosition(Vec2 gridPosition);
	private:
		Components::Grid grid = Components::Grid(0, 0, 0, 0);;
		void InitializeGrid();
	};
} // namespace System
