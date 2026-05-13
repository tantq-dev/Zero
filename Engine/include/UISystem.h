#pragma once
#include <string>
#include <vector>
#include "UIComponents.h"
#include "Vec2.h"

class IRenderer2D;
namespace System { class InputSystem; }

namespace System
{
    // -----------------------------------------------------------------------
    // UISystem — immediate-mode screen-space UI
    //
    // Usage (inside Scene::HandleUI):
    //   ui.Panel({10, 10, 200, 40}, {0,0,0,160});
    //   ui.Label("Score: 99", 20, 15, fontId, {255,255,255,255});
    //   if (ui.Button("Pause", {10, 60, 80, 30}, fontId)) { ... }
    // -----------------------------------------------------------------------
    class UISystem
    {
    public:
        UISystem()  = default;
        ~UISystem() = default;

        // ---- Frame lifecycle (called by Game::Tick) --------------------------

        /// Snapshot mouse state from InputSystem.
        /// Must be called once per frame before HandleUI.
        /// Snapshot mouse state from InputSystem.
        /// Must be called once per frame before HandleUI.
        void BeginFrame(System::InputSystem& input, IRenderer2D& renderer);

        /// Flush all collected draw commands to the renderer in screen space.
        /// Must be called after HandleUI, before renderer EndFrame.
        void Flush(IRenderer2D& renderer);

        // ---- Font helper -----------------------------------------------------

        /// Load a font through the renderer and cache it by path+size.
        /// Safe to call every frame — returns the cached id on subsequent calls.
        uint32_t LoadFont(IRenderer2D& renderer, const std::string& path, float size);

        // ---- Widgets ---------------------------------------------------------

        /// Filled colored rectangle (panel / background).
        void Panel(Components::Rect rect, Components::Color color);

        /// Text label at screen position (x, y) top-left origin by default.
        void Label(const std::string& text,
                   float x, float y,
                   uint32_t fontId,
                   Components::Color color,
                   Components::TextAlign align = Components::TextAlign::Left);

        /// Button — returns true on the frame the mouse button is pressed over it.
        /// Draws a background panel with a centered label.
        bool Button(const std::string& label,
                    Components::Rect rect,
                    uint32_t fontId,
                    UI::UIStyle style = {});

        /// Button with image — returns true on the frame the mouse button is pressed over it.
        bool Button(Components::Texture tex,
                    Components::Rect rect,
                    UI::UIStyle style = {});

        /// Image — render a texture in screen space.
        void Image(Components::Texture tex, Components::Rect rect);

    private:
        // ---- Deferred label record ------------------------------------------
        struct LabelCmd
        {
            std::string text;
            Components::Color color;
            uint32_t fontId = 0;
            float x = 0.0f;
            float y = 0.0f;
            Components::TextAlign align = Components::TextAlign::Left;
        };

        // ---- Mouse state (snapshotted in BeginFrame) ------------------------
        Vec2 m_mousePos      = { 0.0f, 0.0f };
        bool m_mouseHeld     = false;   // LMB currently held
        bool m_mousePressed  = false;   // LMB just went down this frame

        bool m_actionRegistered = false;

        // ---- Draw lists (cleared each BeginFrame) ---------------------------
        std::vector<UI::UIRectCmd>   m_rects;
        std::vector<LabelCmd>        m_labels;
        std::vector<UI::UISpriteCmd> m_sprites;

        // ---- Font cache (path+size -> fontId) --------------------------------
        struct CachedFont { std::string path; float size; uint32_t id; };
        std::vector<CachedFont> m_fontCache;

        // ---- Helpers ---------------------------------------------------------
        static bool ContainsPoint(const Components::Rect& r, const Vec2& p);
    };

} // namespace System
