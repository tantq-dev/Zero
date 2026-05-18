#pragma once
#include "Actor.h"
#include "RenderComponents.h"
#include "CoreComponents.h"
#include "Vec2.h"
#include <memory>

class Seat;


class Character : public Core::Actor {
public:
    
    float m_useTime = 5.0f;
    float m_speed = 80.0f;
    int m_queueIndex = -1;
    uint32_t m_atlasId = 0;
    std::string m_lastDir = "Down";

    Character(std::shared_ptr<Core::World> world, uint32_t atlasId);

    bool MoveTowards(Vec2 target, float dt);
    void UpdateAnimation(const Vec2& dir, bool isMoving);
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;
    void OnStart() override;
    void OnDestroy() override;
};

class Customer : public Character {
public:
    Customer(std::shared_ptr<Core::World> world, uint32_t atlasId)
        : Character(world, atlasId)
    {
    }
    enum class State { Waiting, MovingToSeat, Sitting, Leaving, Finished };
    State m_state = State::Waiting;
    float m_timer = 0;
    std::weak_ptr<Seat> m_targetSeat;

    void OnUpdate(float dt) override;

};
