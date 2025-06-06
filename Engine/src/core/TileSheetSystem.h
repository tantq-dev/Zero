#pragma once
#include "Components.h"
namespace Core
{
	class TileSheetSystem
	{
	public:
		TileSheetSystem();
		~TileSheetSystem();

	private:
		void RenderTileSheet(Components::Tilemap& tileMap);
	};
}


