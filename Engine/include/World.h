#pragma once

#include <memory>
#include "entt.hpp"

namespace Core
{

    class World : public std::enable_shared_from_this<World>
    {
    public:
        entt::registry Registry;

        template<typename T, typename... Args>
        std::shared_ptr<T> SpawnActor(Args&&... args)
        {
            auto actor = std::make_shared<T>(
                shared_from_this(),
                std::forward<Args>(args)...
            );


            return actor;
        }
    };
}