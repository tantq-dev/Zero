//actor is base class for all entity in game without pure ECS implement
#pragma once
#include "entt.hpp"
#include "World.h"
namespace Core {
	class Actor {
	private:
		entt::entity entity = entt::null;
		std::weak_ptr<World> m_world;
	public:
		Actor(std::shared_ptr<World> world);
		~Actor();
		virtual void OnUpdate(float dt) = 0;
		virtual void OnFixedUpdate(float dt) = 0;
		virtual void OnStart() = 0;
		virtual void OnDestroy() = 0;
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            auto world = m_world.lock();

            if (!world)
                throw std::runtime_error("World expired");

            return world->Registry.emplace<T>(
                entity,
                std::forward<Args>(args)...
            );
        }

        template<typename T>
        T& GetComponent()
        {
            auto world = m_world.lock();

            if (!world)
                throw std::runtime_error("World expired");

            return world->Registry.get<T>(entity);
        }

        template<typename T>
        T* TryGetComponent()
        {
            auto world = m_world.lock();

            if (!world)
                return nullptr;

            return world->Registry.try_get<T>(entity);
        }
		
	};
}