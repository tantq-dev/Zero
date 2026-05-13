#pragma once
#include "Actor.h"
class Desk : public Core::Actor {
public:
    Desk(std::shared_ptr < Core::World> world) :
        Actor(world) {
        OnStart();
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
 uint32_t m_deskSlotTextureIDs = -1;
 uint32_t m_deskWithFoodTextureIDs =-1;

};