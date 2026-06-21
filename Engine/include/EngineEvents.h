#pragma once

#include <memory>
#include <string>

namespace Core
{
    class Scene;

    namespace Events
    {
        struct FrameStarted
        {
            double deltaTime = 0.0;
        };

        struct FixedUpdate
        {
            float deltaTime = 0.0f;
        };

        struct FrameEnded
        {
            double deltaTime = 0.0;
        };

        struct SceneChanging
        {
            std::string previousScene;
            std::string nextScene;
        };

        struct SceneChanged
        {
            std::string name;
            std::shared_ptr<Scene> scene;
        };
    }
}
