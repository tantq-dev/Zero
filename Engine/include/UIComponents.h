#pragma once
#include "RenderComponents.h"
#include "TextComponents.h"

namespace UI
{
    // ---------------------------------------------------------------------------
    // Public style struct — configure colors for widgets
    // ---------------------------------------------------------------------------
    struct UIStyle
    {
        Components::Color panelColor    = { 20.0f,  20.0f,  20.0f,  200.0f };
        Components::Color textColor     = { 255.0f, 255.0f, 255.0f, 255.0f };
        Components::Color buttonNormal  = {  60.0f,  60.0f,  60.0f, 220.0f };
        Components::Color buttonHover   = {  90.0f,  90.0f,  90.0f, 255.0f };
        Components::Color buttonPressed = {  40.0f, 120.0f, 200.0f, 255.0f };
    };

    // ---------------------------------------------------------------------------
    // Internal draw commands — not ECS components, used only inside UISystem
    // ---------------------------------------------------------------------------
    struct UIRectCmd
    {
        Components::Rect rect;
        Components::Color color;
        bool fill = true;
    };

    struct UITextCmd
    {
        uint32_t textureId = 0;
        float w = 0.0f;
        float h = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        Components::TextAlign align = Components::TextAlign::Left;
    };

    struct UISpriteCmd
    {
        Components::Texture texture;
        Components::Rect destRect;
    };

} // namespace UI
