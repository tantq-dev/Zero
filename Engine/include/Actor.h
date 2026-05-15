//actor is base class for all entity in game without pure ECS implement
#pragma once
#include "entt.hpp"
#include <memory>
#include <utility>
namespace Core {
    class World; //forward declaration 
	class Actor {
	private:
		entt::entity entity = entt::null;
		std::weak_ptr<World> m_world;
        entt::registry& Registry();
        entt::registry* TryRegistry();
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
            return Registry().emplace<T>(
                entity,
                std::forward<Args>(args)...
            );
        }

        template<typename T>
        T& GetComponent()
        {
            return Registry().get<T>(entity);
        }

        template<typename T>
        T* TryGetComponent()
        {
            auto* registry = TryRegistry();
            if (!registry)
                return nullptr;

            return registry->try_get<T>(entity);
        }
		
	};
}
