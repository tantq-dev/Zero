#include "PhysicSystem.h"
#include <algorithm>

namespace System {

    void PhysicSystem::SetLayerCollision(uint32_t layerA, uint32_t layerB, bool canCollide) {
        if (layerA < MAX_LAYERS && layerB < MAX_LAYERS) {
            m_CollisionMatrix[layerA][layerB] = canCollide;
            m_CollisionMatrix[layerB][layerA] = canCollide;
        }
    }
    // Fast integer floor for grid coordinates
    inline int FastFloor(float val) { return (int)val - (val < 0); }

    void PhysicSystem::Update(float deltaTime, entt::registry& registry) {
        m_Collisions.clear();
        CollisionDetect(registry);
    }

    void PhysicSystem::CollisionDetect(entt::registry& registry) {
        auto view = registry.view<Components::Transform2D, Components::Collider>();
        size_t entityCount = view.size_hint();
        
        if (entityCount == 0) return;

        // 1. Prepare Lookup Arrays
        m_SpatialLookUp.resize(entityCount);
        // Use a large prime for the hash table size to reduce collisions
        uint32_t tableSize = (uint32_t)entityCount * 2;
        m_StartIndices.assign(tableSize + 1, UINT32_MAX);

        // 2. Populate Spatial Lookup
        size_t index = 0;
        for (auto e : view) {
            auto& pos = view.get<Components::Transform2D>(e).position;

            int gx = FastFloor(pos.x / m_CellSize);
            int gy = FastFloor(pos.y / m_CellSize);
            uint32_t h = (uint32_t)((gx * 73856093) ^ (gy * 19349663)) % tableSize;

            auto& col = view.get<Components::Collider>(e);
            m_SpatialLookUp[index++] = { e, h, col.layer };
        }

        // 3. Sort by Hash (This groups entities in the same cell together)
        std::sort(m_SpatialLookUp.begin(), m_SpatialLookUp.end(), [](const auto& a, const auto& b) {
            return a.cellHash < b.cellHash;
            });

        // 4. Build Start Indices (Pointer-less Grid)
        for (uint32_t i = 0; i < (uint32_t)entityCount; i++) {
            uint32_t key = m_SpatialLookUp[i].cellHash;
            uint32_t prevKey = (i == 0) ? UINT32_MAX : m_SpatialLookUp[i - 1].cellHash;
            if (key != prevKey) {
                m_StartIndices[key] = i;
            }
        }

        // 5. Querying Phase
        for (size_t i = 0; i < entityCount; i++) {
            entt::entity e1 = m_SpatialLookUp[i].entity;
            auto& t1 = view.get<Components::Transform2D>(e1);
            auto& c1 = view.get<Components::Collider>(e1);

            int gx = FastFloor(t1.position.x / m_CellSize);
            int gy = FastFloor(t1.position.y / m_CellSize);

            // Check 3x3 neighborhood
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    uint32_t h = (uint32_t)(((gx + dx) * 73856093) ^ ((gy + dy) * 19349663)) % tableSize;

                    uint32_t startIdx = m_StartIndices[h];
                    if (startIdx == UINT32_MAX) continue;

                    for (uint32_t j = startIdx; j < entityCount && m_SpatialLookUp[j].cellHash == h; j++) {
                        entt::entity e2 = m_SpatialLookUp[j].entity;

                        // Identity and Symmetry Check
                        if ((uint32_t)e1 >= (uint32_t)e2) continue;

                        // LAYER CHECK FIRST (Fastest) uses cached layer instead of querying ECS view
                        if (!m_CollisionMatrix[c1.layer][m_SpatialLookUp[j].layer]) continue;

                        auto& c2 = view.get<Components::Collider>(e2);
                        auto& t2 = view.get<Components::Transform2D>(e2);

                        // NARROW PHASE
                        bool collision = false;
                        if (c1.type == ColliderType::Box && c2.type == ColliderType::Box)
                            collision = CheckAABBCollision(t1, *c1.AsBox(), t2, *c2.AsBox());
                        else if (c1.type == ColliderType::Circle && c2.type == ColliderType::Circle)
                            collision = CheckCircleCollision(t1, *c1.AsCircle(), t2, *c2.AsCircle());
                        else if (c1.type == ColliderType::Circle && c2.type == ColliderType::Box)
                            collision = CheckCircleAABBCollision(t1, *c1.AsCircle(), t2, *c2.AsBox());
                        else if (c1.type == ColliderType::Box && c2.type == ColliderType::Circle)
                            collision = CheckCircleAABBCollision(t2, *c2.AsCircle(), t1, *c1.AsBox());

                        if (collision) m_Collisions.emplace_back(e1, e2);
                    }
                }
            }
        }
    }

    bool PhysicSystem::CheckAABBCollision(const auto& t1, const auto& b1, const auto& t2, const auto& b2) {
        // Using half-extents is often cleaner, but keeping your AABB style:
        return t1.position.x < t2.position.x + b2.size.x &&
            t1.position.x + b1.size.x > t2.position.x &&
            t1.position.y < t2.position.y + b2.size.y &&
            t1.position.y + b1.size.y > t2.position.y;
    }

    bool PhysicSystem::CheckCircleCollision(const auto& t1, const auto& c1, const auto& t2, const auto& c2) {
        float dx = t1.position.x - t2.position.x;
        float dy = t1.position.y - t2.position.y;
        float radiusSum = c1.radius + c2.radius;
        return (dx * dx + dy * dy) <= (radiusSum * radiusSum);
    }

    bool PhysicSystem::CheckCircleAABBCollision(const auto& tCircle, const auto& circle, const auto& tRect, const auto& rect) {
        // Optimized AABB-Circle: Find closest point on rect to circle center
        float closestX = std::max(tRect.position.x, std::min(tCircle.position.x, tRect.position.x + rect.size.x));
        float closestY = std::max(tRect.position.y, std::min(tCircle.position.y, tRect.position.y + rect.size.y));

        float dx = tCircle.position.x - closestX;
        float dy = tCircle.position.y - closestY;
        return (dx * dx + dy * dy) < (circle.radius * circle.radius);
    }
}