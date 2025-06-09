#include "GridSystem.h"  
namespace System
{

	GridSystem::GridSystem(const Components::Grid tileSheets)
	{
		m_tileSheets = tileSheets;
	}
	Components::Cell& GridSystem::GetCell(const Vec2 mousePosition, float camZoom, Vec2 camPosition)
	{
		Vec2 adjustedPosition = mousePosition - camPosition;
		//adjustedPosition.x /= camZoom;
		//adjustedPosition.y /= camZoom;
		int index = static_cast<int>(adjustedPosition.x) / m_tileSheets.GetCellSize() + static_cast<int>(adjustedPosition.y) / m_tileSheets.GetCellSize() * m_tileSheets.GetWidth();
		LOG_INFO("Mouse index: " + std::to_string(index));
		return m_tileSheets.GetCell(index);
	}
}