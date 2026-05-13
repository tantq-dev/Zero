#include "Seat.h"
#include "Desk.h"

Seat::Seat(std::shared_ptr<Core::World> world, Components::Texture texture) 
    : Actor(world), m_texture(texture) {
    OnStart();
}

void Seat::OnUpdate(float dt) {}

void Seat::OnFixedUpdate(float dt) {}

void Seat::OnStart() {
    AddComponent<Components::Transform2D>();
    auto& sprite = AddComponent<Components::Sprite>();
    sprite.texture = m_texture;
    sprite.source = { 0, 0, 32, 32 };
    sprite.layer = 2; // Above background
}

void Seat::OnDestroy() {}

void Seat::OnTakeSeat()
{
    auto desk = m_desk.lock();
    if (desk)
    {
        desk->OnCustomerLeave();
    }
}

void Seat::OnLeaveSeat()
{
    auto desk = m_desk.lock();
    if (desk)
    {
        desk->OnCustomerEnter();
    }
}

void Seat::SetDesk(std::shared_ptr<Desk> desk)
{
    m_desk = desk;
}
