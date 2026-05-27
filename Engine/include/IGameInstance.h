#pragma once

namespace Core
{
    /// Abstract interface for a persistent game instance.
    /// Survives across all scene switches for the lifetime of the application.
    /// 
    /// Usage:
    ///   1. Derive from IGameInstance in your game code.
    ///   2. Store whatever cross-scene state you need (run state, settings, etc.).
    ///   3. Register via Game::CreateGameInstance<T>() in main().
    ///   4. Retrieve from any Scene via game->GetGameInstance<T>().
    class IGameInstance
    {
    public:
        virtual ~IGameInstance() = default;

        /// Called once immediately after creation.
        virtual void OnCreate()  {}

        /// Called once just before destruction.
        virtual void OnDestroy() {}
    };
}
