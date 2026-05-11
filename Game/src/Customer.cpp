#include "Customer.h"
#include "Seat.h"

Customer::Customer(std::shared_ptr<Core::World> world) : Actor(world) {
    OnStart();
}

bool Customer::MoveTowards(Vec2 target, float dt) {
    auto& pos = GetComponent<Components::Transform2D>().position;
    Vec2 dir = target - pos;
    float d = dir.length();
    if (d <= 2.0f) {
        pos = target;
        return true;
    }
    pos += dir.normalize() * (m_speed * dt);
    return false;
}

void Customer::OnUpdate(float dt) {
    auto& transform = GetComponent<Components::Transform2D>();
    auto& shape = GetComponent<Components::Shape>();

    switch (m_state) {
    case State::Waiting: {
        Vec2 queuePos = { 0, (int)(-50.0f - (m_queueIndex * 25.0f)) };
        MoveTowards(queuePos, dt);
        shape.color = { 255, 255, 0, 255 }; // Yellow
        break;
    }
    case State::MovingToSeat: {
        if (auto seat = m_targetSeat.lock()) {
            Vec2 seatPos = seat->GetComponent<Components::Transform2D>().position;
            if (MoveTowards(seatPos, dt)) {
                m_state = State::Sitting;
                m_timer = 0;
            }
        }
        shape.color = { 0, 255, 0, 255 }; // Green
        break;
    }
    case State::Sitting: {
        m_timer += dt;
        if (m_timer >= m_useTime) {
            m_state = State::Leaving;
            if (auto seat = m_targetSeat.lock()) {
                seat->isEmpty = true;
                seat->GetComponent<Components::Shape>().color = { 100, 100, 100, 255 };
            }
        }
        shape.color = { 0, 0, 255, 255 }; // Blue
        break;
    }
    case State::Leaving: {
        Vec2 exitPos = { 0, -100 };
        if (MoveTowards(exitPos, dt)) {
            m_state = State::Finished;
        }
        shape.color = { 255, 0, 255, 255 }; // Purple
        break;
    }
    }
}

void Customer::OnFixedUpdate(float dt) {}

void Customer::OnStart() {
    auto& transform = AddComponent<Components::Transform2D>();
    transform.position = { 0, -70 };

    auto& shape = AddComponent<Components::Shape>();
    shape.type = Components::Shape::Type::Rect;
    shape.size = { 15, 15 };
    shape.color = { 255, 0, 0, 255 };
    shape.layer = 2;
}

void Customer::OnDestroy() {}
