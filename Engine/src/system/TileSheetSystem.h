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
		void Init(Components::Tilemap tileSheets);
		[[nodiscard]]const Components::Tilemap& GetTileSheets() const { return m_tileSheets; }
		[[nodiscard]]const Components::Tile& GetTile(const Vec2 mousePosition);
	private:
		Components::Tilemap m_tileSheets;
	};
} // namespace System
