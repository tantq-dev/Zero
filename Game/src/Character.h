#pragma once
#include "Actor.h"
#include "RenderComponents.h"
#include "CoreComponents.h"
#include "Vec2.h"
#include <memory>
#include <vector>

class Seat;
class Desk;

class Character : public Core::Actor {
public:
    
    float m_speed = 80.0f;
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
    enum class State { Waiting, MovingToSeat, Sitting, Order,WaitForOder, Use,  Leaving, Finished };
    State m_state = State::Waiting;
    float m_useTime = 5.0f;
    float m_patientTime = 10.0f;
    float m_orderTime = 2.0f;
    int m_queueIndex = -1;
    float m_timer = 0;
    bool m_isOrder = false;
    bool m_isHaveFood = false;
    bool m_doneOrder = false;
    std::weak_ptr<Seat> m_targetSeat;

    void OnUpdate(float dt) override;
    

};

class Staff : public Character {
public:
    Staff(std::shared_ptr<Core::World> world, uint32_t atlasId)
        : Character(world, atlasId)
    {
    }
    enum class State {Idle, MovingToCustomer, Ordering, Cooking, Serving, CleanUp};
    State m_state = State::Idle;
    float m_cookTime = 2.0f;
    float m_cleanUpTime = 1.0f;
    float m_timer = 0.0f;
    std::weak_ptr<Desk> m_targetDesk;
    std::weak_ptr<Customer> m_targetCustomer;
    std::vector<std::shared_ptr<Core::Actor>>* m_customers = nullptr;
    std::vector<std::shared_ptr<Desk>>* m_desks = nullptr;
    Vec2 m_foodCartPosition = { 0.0f, -50.0f };

    void SetWorkContext(
        std::vector<std::shared_ptr<Core::Actor>>* customers,
        std::vector<std::shared_ptr<Desk>>* desks,
        Vec2 foodCartPosition);
    void OnUpdate(float dt) override;

};
