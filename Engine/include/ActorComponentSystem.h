#pragma once
#include "Actor.h"
#include "entt.hpp"
#include "ActorComponent.h"
namespace System {
	class ActorComponentSystem {

	public:
		ActorComponentSystem() = default;
		~ActorComponentSystem() = default;
		void Update(float deltaTime, entt::registry& registry) {
			auto actorView = registry.view<Components::ActorComponent>();
			actorView.each([&](auto entity, Components::ActorComponent& actor) {
				if (actor.actor)
				{
					actor.actor->OnUpdate(deltaTime);
				}
				});
		}
		void FixedUpdate(float deltaTime, entt::registry& registry) {
			auto actorView = registry.view<Components::ActorComponent>();
			actorView.each([&](auto entity, Components::ActorComponent& actor) {
				if (actor.actor)
				{
					actor.actor->OnFixedUpdate(deltaTime);
				}
				});
		}

	};
}