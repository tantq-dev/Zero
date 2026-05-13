#include "UISystem.h"
#include "IRenderer2D.h"
#include "InputSystem.h"
#include "InputComponents.h"
#include <SDL3/SDL.h>

// Internal action names — not exposed to game code.
static constexpr const char* k_uiMouseBtn    = "__ui_mouse_btn";
static constexpr const char* k_uiMouseMotion = "__ui_mouse_motion";

namespace System
{
    // -------------------------------------------------------------------------
    // BeginFrame — snapshot input, clear draw lists
    // -------------------------------------------------------------------------
    void UISystem::BeginFrame(System::InputSystem& input, IRenderer2D& renderer)
    {
        // Register internal actions once at startup.
        if (!m_actionRegistered)
        {
            Components::InputAction btnAction(k_uiMouseBtn);
            btnAction.AddMouseButtonBinding(SDL_BUTTON_LEFT);
            input.RegisterAction(btnAction);

            Components::InputAction motionAction(k_uiMouseMotion);
            motionAction.AddMouseMotionBinding();
            input.RegisterAction(motionAction);

            m_actionRegistered = true;
        }

        m_mousePos     = renderer.ScreenToLogical(input.GetMousePosition(k_uiMouseMotion));
        m_mouseHeld    = input.IsActionHeld(k_uiMouseBtn);
        m_mousePressed = input.IsActionJustPressed(k_uiMouseBtn);

        // Clear draw lists from the previous frame.
        m_rects.clear();
        m_labels.clear();
        m_sprites.clear();
    }

    // -------------------------------------------------------------------------
    // Font cache
    // -------------------------------------------------------------------------
    uint32_t UISystem::LoadFont(IRenderer2D& renderer, const std::string& path, float size)
    {
        for (const auto& c : m_fontCache)
        {
            if (c.path == path && c.size == size)
                return c.id;
        }

        // Load at real output resolution for crisp UI text.
        float realSize = size * renderer.GetUIScale();
        uint32_t id = renderer.LoadFont(path, realSize);
        m_fontCache.push_back({ path, size, id });
        return id;
    }

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------
    bool UISystem::ContainsPoint(const Components::Rect& r, const Vec2& p)
    {
        return p.x >= r.x && p.x <= r.x + r.w
            && p.y >= r.y && p.y <= r.y + r.h;
    }

    // -------------------------------------------------------------------------
    // Widgets
    // -------------------------------------------------------------------------
    void UISystem::Panel(Components::Rect rect, Components::Color color)
    {
        m_rects.push_back({ rect, color, true });
    }

    void UISystem::Label(const std::string& text,
                         float x, float y,
                         uint32_t fontId,
                         Components::Color color,
                         Components::TextAlign align)
    {
        m_labels.push_back({ text, color, fontId, x, y, align });
    }

    bool UISystem::Button(const std::string& label,
                          Components::Rect rect,
                          uint32_t fontId,
                          UI::UIStyle style)
    {
        bool hovered = ContainsPoint(rect, m_mousePos);
        bool clicked = hovered && m_mousePressed;

        // Background color based on interaction state.
        Components::Color bgColor = style.buttonNormal;
        if (hovered && m_mouseHeld)
            bgColor = style.buttonPressed;
        else if (hovered)
            bgColor = style.buttonHover;

        // Background panel.
        m_rects.push_back({ rect, bgColor, true });

        // Centered label.
        float cx = rect.x + rect.w * 0.5f;
        float cy = rect.y + rect.h * 0.5f;
        m_labels.push_back({ label, style.textColor, fontId, cx, cy, Components::TextAlign::Center });

        return clicked;
    }

    bool UISystem::Button(Components::Texture tex,
                          Components::Rect rect,
                          UI::UIStyle style)
    {
        bool hovered = ContainsPoint(rect, m_mousePos);
        bool clicked = hovered && m_mousePressed;

        // Background color based on interaction state.
        Components::Color bgColor = style.buttonNormal;
        if (hovered && m_mouseHeld)
            bgColor = style.buttonPressed;
        else if (hovered)
            bgColor = style.buttonHover;

        // Background panel.
        m_rects.push_back({ rect, bgColor, true });

        // Image.
        m_sprites.push_back({ tex, rect });

        return clicked;
    }

    void UISystem::Image(Components::Texture tex, Components::Rect rect)
    {
        m_sprites.push_back({ tex, rect });
    }

    // -------------------------------------------------------------------------
    // Flush — draw all commands to the renderer in screen space
    // -------------------------------------------------------------------------
    void UISystem::Flush(IRenderer2D& renderer)
    {
        // 1. Rectangles (panels, button backgrounds).
        for (const auto& cmd : m_rects)
        {
            renderer.DrawRectScreen(cmd.rect, cmd.color, cmd.fill);
        }

        // 2. Sprite images.
        for (const auto& cmd : m_sprites)
        {
            renderer.PushSpriteScreen(cmd.texture, cmd.destRect);
        }

        // 3. Text labels — rasterised at native resolution, positioned in virtual coords.
        float uiScale = renderer.GetUIScale();

        for (const auto& lbl : m_labels)
        {
            float w = 0.0f, h = 0.0f;
            uint32_t texId = renderer.RenderTextToTexture(
                lbl.fontId, lbl.text, lbl.color,
                false, 0, w, h);

            // w, h are in real pixels — convert to virtual coords for layout.
            float vw = w / uiScale;
            float vh = h / uiScale;

            // Resolve pixel x based on alignment.
            float drawX = lbl.x;
            if (lbl.align == Components::TextAlign::Center)
                drawX = lbl.x - vw * 0.5f;
            else if (lbl.align == Components::TextAlign::Right)
                drawX = lbl.x - vw;

            // Vertically center on the given y.
            float drawY = lbl.y - vh * 0.5f;

            renderer.PushTextScreen(texId, vw, vh, drawX, drawY, lbl.align);
        }
    }

} // namespace System
