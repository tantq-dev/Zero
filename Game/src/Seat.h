#pragma once
#include "Actor.h"
#include "RenderComponents.h"
#include "CoreComponents.h"
class Desk;

class Seat : public Core::Actor {
public:
    bool isEmpty = true;
    Seat(std::shared_ptr<Core::World> world, Components::Texture texture);
    
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;
    void OnStart() override;
    void OnDestroy() override;
    void OnTakeSeat();
    void OnLeaveSeat();
    void SetDesk(std::shared_ptr<Desk> desk);
private:
    Components::Texture m_texture;
    std::weak_ptr<Desk> m_desk; // can change later if seat use not for desk 
};
