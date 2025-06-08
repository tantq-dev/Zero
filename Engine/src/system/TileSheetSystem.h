#pragma once
#include "core/Components.h"
#include "utilities/Vec2.h"
namespace System
{
	class TileSheetSystem
	{
	public:
		TileSheetSystem() = default;
		~TileSheetSystem() = default;
		TileSheetSystem(Components::TileSheet tilesheet);
		const Components::TileSheet& GetTileSheets() const { return m_tileSheets; }
		Components::Tile& GetTile(const Vec2 mousePosition, float camZoom, Vec2 camPosition);
	private:
		Components::TileSheet m_tileSheets = Components::TileSheet(0, 0, 0, 0);;
	};
} // namespace System
