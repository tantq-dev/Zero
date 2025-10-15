#pragma once
#include "Components.h"
#include <vector>
#include "entt.hpp"

class Grid
{
public:
    Grid() = default;
    ~Grid() = default;

    void Initialize(int width, int height, int cellSize);
    void CreateTileEntities(entt::registry& registry, 
                           const std::vector<std::vector<int>>& mapData,
                           uint32_t spriteSheetId);
    void Clear(entt::registry& registry);
    
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetCellSize() const { return m_cellSize; }
    int GetTileCount() const { return static_cast<int>(m_tileEntities.size()); }

private:
    int m_width = 0;
    int m_height = 0;
    int m_cellSize = 0;
    std::vector<entt::entity> m_tileEntities;
};