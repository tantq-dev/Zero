#include "Seat.h"
#include "Desk.h"

Seat::Seat(std::shared_ptr<Core::World> world, SeatConfig config) 
    : Actor(world), m_config(config) {
}

void Seat::OnUpdate(float dt) {}

void Seat::OnFixedUpdate(float dt) {}

void Seat::OnStart() {
    auto& transform = AddComponent<Components::Transform2D>();
    constexpr float SeatSpriteHeight = 32.0f;
    transform.position = { m_config.position.x, m_config.position.y };

    auto& sprite = AddComponent<Components::Sprite>();
    sprite.texture = m_config.texture;
    sprite.source = { 0, 0, 32, 32 };
    sprite.layer = m_config.layer;
    sprite.pivot = { 0.5f, 0.5f };
}

void Seat::OnDestroy() {}

void Seat::OnTakeSeat()
{
    auto desk = m_desk.lock();

    if (desk)
    {
        desk->OnCustomerEnter();
    }
}

void Seat::OnLeaveSeat()
{
    auto desk = m_desk.lock();
    if (desk)
    {
        desk->OnCustomerLeave();
    }
}

void Seat::SetDesk(std::shared_ptr<Desk> desk)
{
    m_desk = desk;
}

bool Seat::IsEmpty() const
{
    return m_isEmpty;
}

bool Seat::IsAvailable() const
{
    auto desk = m_desk.lock();
    return m_isEmpty && (!desk || desk->IsClean());
}

std::shared_ptr<Desk> Seat::GetDesk() const
{
    return m_desk.lock();
}

void Seat::Reserve()
{
    m_isEmpty = false;
}

void Seat::Release()
{
    m_isEmpty = true;
}

SeatDirection Seat::GetDirection() const
{
    return m_config.direction;
}

Vec2 Seat::GetFacingVector() const
{
    switch (m_config.direction)
    {
    case SeatDirection::Up:
        return { 0.0f, -1.0f };
    case SeatDirection::Down:
        return { 0.0f, 1.0f };
    case SeatDirection::Left:
        return { -1.0f, 0.0f };
    case SeatDirection::Right:
        return { 1.0f, 0.0f };
    default:
        return { 0.0f, 1.0f };
    }
}
