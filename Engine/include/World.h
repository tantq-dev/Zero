#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include "entt.hpp"
#include "Actor.h"
#include "IGameMode.h"
namespace Core
{

    class World : public std::enable_shared_from_this<World>
    {
    private:
        std::vector<std::shared_ptr<Actor>> m_actors;
        std::unique_ptr<IGameMode> m_gameMode;
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
        void Clear();

        // GameMode — per-scene rules, recreated each Initialize()
        template<typename T, typename... Args>
        T& SetGameMode(Args&&... args)
        {
            static_assert(std::is_base_of_v<IGameMode, T>,
                "T must inherit from Core::IGameMode");
            if (m_gameMode) m_gameMode->EndPlay();
            m_gameMode = std::make_unique<T>(std::forward<Args>(args)...);
            m_gameMode->BeginPlay();
            return static_cast<T&>(*m_gameMode);
        }

        template<typename T>
        T* GetGameMode()
        {
            return dynamic_cast<T*>(m_gameMode.get());
        }
    };
}
