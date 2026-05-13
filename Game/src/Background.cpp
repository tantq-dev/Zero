#include "Background.h"
#include "Components.h"

Background::Background(std::shared_ptr<Core::World> world) : Actor(world) {
  OnStart();
}
void Background::OnUpdate(float dt) {}

void Background::OnFixedUpdate(float dt) {}

void Background::OnStart() {
  AddComponent<Components::Transform2D>();
  auto &shape = AddComponent<Components::Shape>();
  shape.size = {1280, 720};
  shape.color = {0, 0, 0, 255};
  shape.type = Components::Shape::Type::Rect;
  shape.layer = 1; // background
}

void Background::OnDestroy() {}
