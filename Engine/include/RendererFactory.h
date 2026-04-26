#pragma once
#include <memory>
#include "IRenderer2D.h"

namespace Core {
    class Window;
}

namespace System {
    class RendererFactory {
    public:
        static std::shared_ptr<IRenderer2D> CreateRenderer(std::shared_ptr<Core::Window> window);
    };
}
