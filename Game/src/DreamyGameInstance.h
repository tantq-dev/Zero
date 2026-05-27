#pragma once
#include "IGameInstance.h"
#include "RunState.h"

/// The one game instance for My Dreamy Foodiness.
/// Persists for the entire application lifetime — survives all scene switches.
/// Access it from any scene:
///   auto& gi = game->GetGameInstance<DreamyGameInstance>();
class DreamyGameInstance : public Core::IGameInstance
{
public:
    RunState runState;

    void OnCreate() override
    {
        runState.Reset();
    }

    void OnDestroy() override {}
};
