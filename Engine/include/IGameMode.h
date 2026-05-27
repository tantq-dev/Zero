#pragma once

namespace Core
{
    /// Abstract interface for per-scene game rules.
    /// Owned by Core::World. Recreated each time a scene initialises.
    /// 
    /// Usage:
    ///   1. Derive from IGameMode in your game code.
    ///   2. Store per-scene rule data (spawn rates, win conditions, etc.).
    ///   3. Set via world->SetGameMode<T>(...) inside Scene::Initialize().
    ///   4. Retrieve via world->GetGameMode<T>() — intended for init time only.
    ///      Copy hot-path values into plain Scene members; avoid per-frame virtual calls.
    class IGameMode
    {
    public:
        virtual ~IGameMode() = default;

        /// Called immediately after the game mode is created and attached to the world.
        virtual void BeginPlay() {}

        /// Called just before the game mode is replaced or the world is destroyed.
        virtual void EndPlay()   {}
    };
}
