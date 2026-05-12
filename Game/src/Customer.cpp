#include "Customer.h"
#include "Seat.h"

Customer::Customer(std::shared_ptr<Core::World> world, uint32_t atlasId) 
    : Actor(world), m_atlasId(atlasId) {
    OnStart();
}

bool Customer::MoveTowards(Vec2 target, float dt) {
    auto& pos = GetComponent<Components::Transform2D>().position;
    Vec2 dir = target - pos;
    float d = dir.length();
    
    if (d <= 2.0f) {
        pos = target;
        UpdateAnimation(dir, false);
        return true;
    }
    
    Vec2 velocity = dir.normalize();
    pos += velocity * (m_speed * dt);
    UpdateAnimation(velocity, true);
    return false;
}

void Customer::UpdateAnimation(const Vec2& dir, bool isMoving) {
    auto& anim = GetComponent<Components::Animation>();
    
    std::string direction = m_lastDir;
    if (std::abs(dir.x) > std::abs(dir.y)) {
        direction = (dir.x > 0) ? "Right" : "Left";
    } else if (std::abs(dir.y) > 0.1f) {
        direction = (dir.y > 0) ? "Down" : "Up";
    }
    
    m_lastDir = direction;
    std::string animName = (isMoving ? "Move" : "Idle") + direction;
    
    if (anim.currentAnimationName != animName) {
        anim.currentAnimationName = animName;
        anim.currentFrameIndex = 0;
        anim.currentFrameTime = 0;
    }
}

void Customer::OnUpdate(float dt) {
    auto& transform = GetComponent<Components::Transform2D>();

    switch (m_state) {
    case State::Waiting: {
        Vec2 queuePos = { 0, (int)(-50.0f - (m_queueIndex * 25.0f)) };
        MoveTowards(queuePos, dt);
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
        break;
    }
    case State::Sitting: {
        m_timer += dt;
        UpdateAnimation(Vec2(0, 1), false); // IdleDown when sitting
        if (m_timer >= m_useTime) {
            m_state = State::Leaving;
            if (auto seat = m_targetSeat.lock()) {
                seat->isEmpty = true;
                seat->GetComponent<Components::Shape>().color = { 100, 100, 100, 255 };
            }
        }
        break;
    }
    case State::Leaving: {
        Vec2 exitPos = { 0, -100 };
        if (MoveTowards(exitPos, dt)) {
            m_state = State::Finished;
        }
        break;
    }
    }
}

void Customer::OnFixedUpdate(float dt) {}

void Customer::OnStart() {
    auto& transform = AddComponent<Components::Transform2D>();
    transform.position = { 0, -70 };

    auto& sprite = AddComponent<Components::Sprite>();
    sprite.layer = 2;
    sprite.pivot = { 0.5f, 1.0f }; // Bottom pivot for characters

    auto& anim = AddComponent<Components::Animation>();
    anim.atlasId = m_atlasId;
    anim.currentAnimationName = "IdleDown";
    anim.isPlaying = true;
}

void Customer::OnDestroy() {}
