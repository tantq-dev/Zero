#include "system/TileSheetSystem.h"
namespace System
{
	void TileSheetSystem::Init(Components::Tilemap tileSheets)
	{
		m_tileSheets = std::move(tileSheets);
	}
	const Components::Tile& TileSheetSystem::GetTile(const Vec2 mousePosition)
	{
		int index = mousePosition.x / m_tileSheets.GetTileSize() + mousePosition.y / m_tileSheets.GetTileSize() * m_tileSheets.GetMapWidth();

		return m_tileSheets.GetTiles(index);
	}
}