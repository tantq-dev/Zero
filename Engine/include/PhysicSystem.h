#pragma once
#include <vector>
#include <entt.hpp>
#include "Components.h"
#include <bitset>
#undef min
#undef max
namespace System {
    struct Cell {
        int x, y;
        bool operator==(const Cell& other) const { return x == other.x && y == other.y; }
    };

    // A simple hash for the cell coordinates
    struct CellHash {
        size_t operator()(const Cell& c) const {
            return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 1);
        }
    };

    class PhysicSystem {
    public:
        void Update(float deltaTime, entt::registry& registry);
        void SetLayerCollision(uint32_t layerA, uint32_t layerB, bool canCollide);
        std::vector<Components::CollisionPair>& GetCollisions() {
            return m_Collisions;
        }
    private:
        void CollisionDetect(entt::registry& registry);

        // Helper to check and store collision
        void NarrowPhase(entt::entity e1, entt::entity e2, entt::registry& registry);

        // Grid Settings
        float m_CellSize = 100.0f;

        // Data structures for Spatial Hashing
        struct SpatialEntry {
            entt::entity entity;
            uint32_t cellHash;
            uint32_t layer;
        };
        std::vector<SpatialEntry> m_SpatialLookUp;
        std::vector<uint32_t> m_StartIndices;

        // Collision result storage
        std::vector<Components::CollisionPair> m_Collisions;
        std::array<std::bitset<MAX_LAYERS>, MAX_LAYERS> m_CollisionMatrix;
        

        // Math Helpers
        bool CheckAABBCollision(const auto& t1, const auto& b1, const auto& t2, const auto& b2);
        bool CheckCircleCollision(const auto& t1, const auto& c1, const auto& t2, const auto& c2);
        bool CheckCircleAABBCollision(const auto& tCircle, const auto& c, const auto& tRect, const auto& b);
    };
}