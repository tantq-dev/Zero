#include "Seat.h"
#include "Desk.h"

Seat::Seat(std::shared_ptr<Core::World> world, SeatConfig config) 
    : Actor(world), m_config(config) {
}

void Seat::OnUpdate(float dt) {}

void Seat::OnFixedUpdate(float dt) {}

void Seat::OnStart() {
    auto& transform = AddComponent<Components::Transform2D>();
    transform.position = m_config.position;

    auto& sprite = AddComponent<Components::Sprite>();
    sprite.texture = m_config.texture;
    sprite.source = { 0, 0, 32, 32 };
    sprite.layer = m_config.layer;
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

void Seat::Reserve()
{
    m_isEmpty = false;
}

void Seat::Release()
{
    m_isEmpty = true;
}
