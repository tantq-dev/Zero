#include "Actor.h"
#include "World.h"
#include <ActorComponent.h>

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
        auto world = m_world.lock();

        if (world &&
            world->Registry.valid(entity))
        {
            world->Registry.destroy(entity);
        }
    }
	
}