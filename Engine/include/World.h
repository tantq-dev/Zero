#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include "entt.hpp"
#include "Actor.h"
namespace Core
{

    class World : public std::enable_shared_from_this<World>
    {
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
            actor->OnStart();

            return actor;
        }
    };
}
