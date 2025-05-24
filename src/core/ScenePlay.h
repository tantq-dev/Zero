#pragma once
#include "Scene.h"
#include "Component.h"

namespace Core
{
    class Game;

    class ScenePlay : public Scene
    {
    public:
        ScenePlay() = default;
        ~ScenePlay() = default;
        
        void Initialize() override;
        void Update(float deltaTime) override;
        void SRender(SDL_Renderer* renderer) override;
        void SDoAction() override;
    };
}

