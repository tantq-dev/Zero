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
		const Components::Grid& GetTileSheets() const { return m_tileSheets; }
		Components::Cell& GetCell(const Vec2 mousePosition);
	private:
		Components::Grid m_tileSheets = Components::Grid(0, 0, 0, 0);;
	};
} // namespace System
