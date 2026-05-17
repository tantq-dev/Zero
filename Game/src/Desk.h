#pragma once
#include "Actor.h"
#include "Components.h"
#include "Vec2.h"

struct DeskConfig {
    Components::Texture emptyTexture = {};
    Components::Texture occupiedTexture = {};
    Vec2 position = { 0.0f, 0.0f };
    int layer = 5;
};

class Desk : public Core::Actor {
public:
    Desk(std::shared_ptr<Core::World> world, DeskConfig config) :
        Actor(world), m_config(config)
    {
    }
    // Inherited via Actor
    void OnUpdate(float dt) override;

    void OnFixedUpdate(float dt) override;

    void OnStart() override;

    void OnDestroy() override;
    
    void OnCustomerLeave();
    void OnCustomerEnter();

private:
    size_t m_currentCustomer = 0;
    const DeskConfig m_config;
};
