#include "World.h"
#include <algorithm>

namespace Core
{
    bool World::RemoveActor(const std::shared_ptr<Actor>& actor)
    {
        if (!actor)
        {
            return false;
        }

        return RemoveActor(actor.get());
    }

    bool World::RemoveActor(const Actor* actor)
    {
        if (!actor)
        {
            return false;
        }

        auto it = std::find_if(
            m_actors.begin(),
            m_actors.end(),
            [actor](const std::shared_ptr<Actor>& storedActor)
            {
                return storedActor.get() == actor;
            });

        if (it == m_actors.end())
        {
            return false;
        }

        auto removedActor = *it;
        removedActor->OnDestroy();
        removedActor->DestroyEntity();
        m_actors.erase(it);

        return true;
    }

    void World::Clear()
    {
        auto actors = std::move(m_actors);
        m_actors.clear();

        for (auto& actor : actors)
        {
            if (!actor)
            {
                continue;
            }

            actor->OnDestroy();
            actor->DestroyEntity();
        }

        if (m_gameMode)
        {
            m_gameMode->EndPlay();
            m_gameMode.reset();
        }

        Registry.clear();
    }
}
