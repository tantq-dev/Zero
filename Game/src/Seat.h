#pragma once
#include "Actor.h"
#include "RenderComponents.h"
#include "CoreComponents.h"

class Seat : public Core::Actor {
public:
    bool isEmpty = true;
    Seat(std::shared_ptr<Core::World> world);
    
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;
    void OnStart() override;
    void OnDestroy() override;
};
