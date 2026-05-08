//#include "World.h"
//namespace Core
//{
//    template<typename T, typename... Args>
//    std::shared_ptr<T> World::SpawnActor(Args&&... args)
//    {
//        auto actor = std::make_shared<T>(
//            shared_from_this(),
//            std::forward<Args>(args)...
//        );
//
//        actor->OnStart();
//
//        return actor;
//    }
//}