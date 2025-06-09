#include "GridSystem.h"  
namespace System
{

	GridSystem::GridSystem(const Components::Grid tileSheets)
	{
		m_tileSheets = tileSheets;
	}
	Components::Cell& GridSystem::GetCell(const Vec2 mousePosition)
	{
		int index = static_cast<int>(mousePosition.x) / m_tileSheets.GetCellSize() + static_cast<int>(mousePosition.y) / m_tileSheets.GetCellSize() * m_tileSheets.GetWidth();
		LOG_INFO("Mouse index: " + std::to_string(index));
		return m_tileSheets.GetCell(index);
	}
}