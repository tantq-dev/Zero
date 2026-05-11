#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Player/Player.h"
#include "Game.h"
#include "UISystem.h"
#include <iomanip>
#include <sstream>
#include "Background.h"
#undef max

void GameplayScene::Initialize()
{
    auto game = m_game.lock();
    if (!game)
    {
        LOG_INFO("Game is empty");
    }
    m_Font = game->GetResources().GetFontId(EngieResources::DEFAULT_FONT, 24);
    m_backgroundActor = m_world->SpawnActor<Background>();
}

void GameplayScene::Update(const double& deltaTime)
{
 
    m_current += deltaTime * 100;
    if (m_current>1440)
    {
        m_current = 0;
    }
    float daytime = m_current / 60.0f;
    float t = std::sin((daytime / 24.0f) * 3.14f);
    t = std::max(t, 0.0f);

    if (m_backgroundActor) {
        Components::Color nightColor = { 20,20,40,255 };
        Components::Color dayColor = { 135,206,235,255 };
        Components::Color currentColor = Components::LerpColor(nightColor, dayColor, t);
        auto& shape = m_backgroundActor->GetComponent<Components::Shape>();
        shape.color = currentColor;

    }
   
}

void GameplayScene::FixedUpdate(const double& deltaTime)
{
}

void GameplayScene::Render(::IRenderer2D& renderer)
{
}

void GameplayScene::HandleInput()
{
}

void GameplayScene::HandleUI(System::UISystem& ui)
{
    ui.Label(GetCurrentTimeString(m_current), 160, 10, m_Font, {255,255,255,255}, Components::TextAlign::Center);
}

std::string GameplayScene::GetCurrentTimeString(double& time)
{
    uint32_t timeInt = std::round(time);
    uint32_t m = timeInt % 60;
    uint32_t h = timeInt / 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h
        << ":"
        << std::setw(2) << std::setfill('0') << m;
    return oss.str();
}


