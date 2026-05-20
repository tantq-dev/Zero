#pragma once
#include "Actor.h"
#include "RenderComponents.h"
#include "CoreComponents.h"
#include "Vec2.h"
class Desk;

enum class SeatDirection {
    Up,
    Down,
    Left,
    Right
};

struct SeatConfig {
    Components::Texture texture = {};
    Vec2 position = { 0.0f, 0.0f };
    int layer = 4;
    SeatDirection direction = SeatDirection::Down;
};

class Seat : public Core::Actor {
public:
    Seat(std::shared_ptr<Core::World> world, SeatConfig config);
    
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float dt) override;
    void OnStart() override;
    void OnDestroy() override;
    void OnTakeSeat();
    void OnLeaveSeat();
    void SetDesk(std::shared_ptr<Desk> desk);
    bool IsEmpty() const;
    bool IsAvailable() const;
    std::shared_ptr<Desk> GetDesk() const;
    void Reserve();
    void Release();
    SeatDirection GetDirection() const;
    Vec2 GetFacingVector() const;


private:
    const SeatConfig m_config;
    bool m_isEmpty = true;
    std::weak_ptr<Desk> m_desk; // can change later if seat use not for desk 
};
