#include "Character.h"
#include "Seat.h"
#include "Desk.h"

Character::Character(std::shared_ptr<Core::World> world, uint32_t atlasId) 
    : Actor(world), m_atlasId(atlasId) {
}

bool Character::MoveTowards(Vec2 target, float dt) {
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

void Character::UpdateAnimation(const Vec2& dir, bool isMoving) {
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

void Character::OnUpdate(float dt) {
   
}

void Character::OnFixedUpdate(float dt) {}

void Character::OnStart() {
    auto& transform = AddComponent<Components::Transform2D>();
    transform.position = { 0, 120 };

    auto& sprite = AddComponent<Components::Sprite>();
    sprite.layer = 2;
    sprite.sortOffsetY = 1.0f;
    sprite.pivot = { 0.5f, 1.0f }; // Bottom pivot for characters

    auto& anim = AddComponent<Components::Animation>();
    anim.atlasId = m_atlasId;
    anim.currentAnimationName = "IdleDown";
    anim.isPlaying = true;
}

void Character::OnDestroy() {}

void Customer::ChangeState(State state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    OnEnterState(state);
}

void Customer::OnEnterState(State state)
{
    switch (state) {
    case State::Sitting:
    case State::Order:
    case State::Use:
        m_timer = 0.0f;
        break;
    case State::WaitForOder:
        m_doneOrder = true;
        m_timer = 0.0f;
        break;
    case State::Leaving:
        ReleaseSeat();
        break;
    default:
        break;
    }
}

void Customer::ReleaseSeat()
{
    if (auto seat = m_targetSeat.lock()) {
        seat->Release();
        seat->OnLeaveSeat();
    }
}

void Customer::OnUpdate(float dt)
{
    switch (m_state) {
    case State::Waiting: {
        Vec2 queuePos = { 0.0f, (float)(120.0f + (m_queueIndex * 25.0f)) };
        MoveTowards(queuePos, dt);
        break;
    }
    case State::MovingToSeat: {
        if (auto seat = m_targetSeat.lock()) {
            Vec2 seatPos = seat->GetComponent<Components::Transform2D>().position;
            if (MoveTowards(seatPos, dt)) {
                ChangeState(State::Sitting);
                seat->OnTakeSeat();
            }
        } else {
            ChangeState(State::Leaving);
        }
        break;
    }
    case State::Sitting: {
        m_timer += dt;
        if (auto seat = m_targetSeat.lock()) {
            UpdateAnimation(seat->GetFacingVector(), false);
        }
        if (m_timer >= m_patientTime) {
            ChangeState(State::Leaving);
        }
        if (m_isOrder)
        {
            ChangeState(State::Order);
            
        }
        break;
    }

    case State::Order: {
        m_timer += dt;
        //todo: play order anim
        if (m_timer  > m_orderTime)
        {
            ChangeState(State::WaitForOder);
        }
        break;
    }

    case State::WaitForOder: {
        if (m_isHaveFood)
        {
            ChangeState(State::Use);
        }
        break;
    }
    case State::Use: {
        m_timer += dt;
        //todo: play order anim
        if (m_timer > m_useTime)
        {
            ChangeState(State::Leaving);
        }
        break;
    }

    case State::Leaving: {
        Vec2 exitPos = { 0, 140 };
        if (MoveTowards(exitPos, dt)) {
            ChangeState(State::Finished);
        }
        break;
    }
    case State::Finished:
        UpdateAnimation({ 0.0f, 1.0f }, false);
        break;
    }
}

void Staff::SetWorkContext(
    std::vector<std::shared_ptr<Core::Actor>>* customers,
    std::vector<std::shared_ptr<Desk>>* desks,
    Vec2 foodCartPosition)
{
    m_customers = customers;
    m_desks = desks;
    m_foodCartPosition = foodCartPosition;
}

void Staff::ChangeState(State state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    OnEnterState(state);
}

void Staff::OnEnterState(State state)
{
    switch (state) {
    case State::Idle:
        ClearCurrentJob();
        break;
    case State::Ordering:
    case State::Cooking:
    case State::CleanUp:
        m_timer = 0.0f;
        break;
    default:
        break;
    }
}

void Staff::ClearCurrentJob()
{
    m_targetCustomer.reset();
    m_targetDesk.reset();
    m_timer = 0.0f;
}

void Staff::OnUpdate(float dt)
{
    auto& transform = GetComponent<Components::Transform2D>();

    switch (m_state) {
    case State::Idle: {
        UpdateAnimation({ 0.0f, 1.0f }, false);

        if (m_customers) {
            for (auto& actor : *m_customers) {
                auto customer = std::static_pointer_cast<Customer>(actor);
                if (customer->m_state == Customer::State::Sitting && !customer->m_isOrder) {
                    m_targetCustomer = customer;
                    if (auto seat = customer->m_targetSeat.lock()) {
                        m_targetDesk = seat->GetDesk();
                    }
                    ChangeState(State::MovingToCustomer);
                    break;
                }
            }
        }

        if (m_state != State::Idle) {
            break;
        }

        if (m_desks) {
            for (auto& desk : *m_desks) {
                if (desk && desk->NeedsCleanUp()) {
                    m_targetDesk = desk;
                    ChangeState(State::CleanUp);
                    break;
                }
            }
        }
        break;
        
    }
    case State::CleanUp: {
        auto desk = m_targetDesk.lock();
        if (!desk || !desk->NeedsCleanUp()) {
            ChangeState(State::Idle);
            break;
        }

        Vec2 deskPos = desk->GetComponent<Components::Transform2D>().position;
        if (MoveTowards(deskPos, dt)) {
            m_timer += dt;
            if (m_timer >= m_cleanUpTime) {
                desk->OnCleaningUp();
                ChangeState(State::Idle);
            }
        }
        break;

    }
    case State::MovingToCustomer: {
        auto customer = m_targetCustomer.lock();
        if (!customer || customer->m_state != Customer::State::Sitting) {
            ChangeState(State::Idle);
            break;
        }

        if (auto seat = customer->m_targetSeat.lock()) {
            Vec2 seatPos = seat->GetComponent<Components::Transform2D>().position;
            Vec2 offset = transform.position - seatPos;
            if (offset.length() <= 0.01f) {
                offset = seat->GetFacingVector() * -1.0f;
            }
            Vec2 targetPos = seatPos + offset.normalize() * 20;
            if (MoveTowards(targetPos, dt)) {
                customer->m_isOrder = true;
                ChangeState(State::Ordering);
            }
        } else {
            ChangeState(State::Idle);
        }
        break;

    }
    case State::Ordering: {
        auto customer = m_targetCustomer.lock();
        if (!customer || customer->m_state == Customer::State::Leaving || customer->m_state == Customer::State::Finished) {
            ChangeState(State::Idle);
            break;
        }

        if (customer->m_doneOrder) {
            if (auto seat = customer->m_targetSeat.lock()) {
                m_targetDesk = seat->GetDesk();
                ChangeState(State::Cooking);
            } else {
                ChangeState(State::Idle);
            }
        }
        break;
    }
    case State::Cooking: {
        auto customer = m_targetCustomer.lock();
        if (!customer || customer->m_state == Customer::State::Leaving || customer->m_state == Customer::State::Finished) {
            ChangeState(State::Idle);
            break;
        }

        if (MoveTowards(m_foodCartPosition, dt)) {
            m_timer += dt;
            if (m_timer >= m_cookTime) { 
                ChangeState(State::Serving);
            }
        }
        break;
    }
    case State::Serving:{
        auto customer = m_targetCustomer.lock();
        auto desk = m_targetDesk.lock();
        if (!customer || customer->m_state != Customer::State::WaitForOder || !desk) {
            ChangeState(State::Idle);
            break;
        }

        auto seat = customer->m_targetSeat.lock();
        if (!seat) {
            ChangeState(State::Idle);
            break;
        }

        Vec2 deskPos = desk->GetComponent<Components::Transform2D>().position;
        if (MoveTowards(deskPos, dt)) {
            customer->m_isHaveFood = true;
            desk->OnFoodServing();

            ChangeState(State::Idle);
        }
        break;
    }
    
    }
}
