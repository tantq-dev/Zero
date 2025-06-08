#include "TileSheetSystem.h"  
namespace System  
{  

	TileSheetSystem::TileSheetSystem(const Components::TileSheet tileSheets)  
	{  
		m_tileSheets = tileSheets;  
	}  
	Components::Tile& TileSheetSystem::GetTile(const Vec2 mousePosition, float camZoom, Vec2 camPosition)
	{  
		Vec2 adjustedPosition = mousePosition - camPosition;
		//adjustedPosition.x /= camZoom;
		//adjustedPosition.y /= camZoom;
		int index = static_cast<int>(adjustedPosition.x) / m_tileSheets.GetTileSize()  + static_cast<int>(adjustedPosition.y) / m_tileSheets.GetTileSize() * m_tileSheets.GetWidth() ;
		LOG_INFO("Mouse index: " + std::to_string(index));
		return m_tileSheets.GetTiles(index);  
	}  
}