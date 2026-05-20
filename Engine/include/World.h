#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include "entt.hpp"
#include "Actor.h"
namespace Core
{

    class World : public std::enable_shared_from_this<World>
    {
    private:
        std::vector<std::shared_ptr<Actor>> m_actors;
    public:
        entt::registry Registry;

        template<typename T, typename... Args>
        std::shared_ptr<T> SpawnActor(Args&&... args)
        {
            static_assert(std::is_base_of_v<Actor, T>, "T must inherit from Core::Actor");
            auto actor = std::make_shared<T>(
                shared_from_this(),
                std::forward<Args>(args)...
            );
            m_actors.push_back(actor);
            actor->OnStart();

            return actor;
        }

        bool RemoveActor(const std::shared_ptr<Actor>& actor);
        bool RemoveActor(const Actor* actor);
    };
}
