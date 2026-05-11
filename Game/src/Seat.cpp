#include "Seat.h"

Seat::Seat(std::shared_ptr<Core::World> world) : Actor(world) {
    OnStart();
}

void Seat::OnUpdate(float dt) {}

void Seat::OnFixedUpdate(float dt) {}

void Seat::OnStart() {
    AddComponent<Components::Transform2D>();
    auto& shape = AddComponent<Components::Shape>();
    shape.type = Components::Shape::Type::Rect;
    shape.size = { 20, 20 };
    shape.color = { 100, 100, 100, 255 }; // Gray for empty
    shape.layer = 1;
}

void Seat::OnDestroy() {}
