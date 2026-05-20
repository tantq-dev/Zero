#include "Actor.h"
#include "World.h"
#include <ActorComponent.h>
#include <stdexcept>

namespace Core
{
    Actor::Actor(std::shared_ptr<World> world)
    {
        m_world = world;

        entity = world->Registry.create();
        world->Registry.emplace<Components::ActorComponent>(
            entity,
            this
        );
    }
    Actor::~Actor()
    {
        DestroyEntity();
    }

    void Actor::DestroyEntity()
    {
        auto world = m_world.lock();

        if (world &&
            world->Registry.valid(entity))
        {
            world->Registry.destroy(entity);
            entity = entt::null;
        }
    }

    entt::registry& Actor::Registry()
    {
        auto world = m_world.lock();

        if (!world)
            throw std::runtime_error("World expired");

        return world->Registry;
    }

    entt::registry* Actor::TryRegistry()
    {
        auto world = m_world.lock();

        if (!world)
            return nullptr;

        return &world->Registry;
    }
	
}
