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
	private:
		Components::Grid grid = Components::Grid(0, 0, 0, 0);;
	};
} // namespace System
