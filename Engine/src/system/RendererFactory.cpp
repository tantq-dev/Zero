#include "RendererFactory.h"
#include "SDLRenderer2D.h"
#include "Window.h"

namespace System {
    std::shared_ptr<IRenderer2D> RendererFactory::CreateRenderer(std::shared_ptr<Core::Window> window) {
        return std::make_shared<SDLRenderer2D>(window->GetRenderer());
    }
}
