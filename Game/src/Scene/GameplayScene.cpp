#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Player/Player.h"
#include "Game.h"
#include "UISystem.h"
#include <iomanip>
#include <sstream>
#include "Background.h"
#undef max

#include "Seat.h"
#include "Customer.h"




void GameplayScene::Initialize()
{
    auto game = m_game.lock();
    if (!game)
    {
        LOG_INFO("Game is empty");
    }
    m_Font = game->GetResources().GetFontId(EngieResources::DEFAULT_FONT, 24);
    //add camera
    entt::entity cam = m_world->Registry.create();
    m_world->Registry.emplace<Components::CameraComponent>(cam,Vec2{0,0}, 1280.0f,720.0f);

    m_backgroundActor = m_world->SpawnActor<Background>();
    
    
   
    // Load customer atlas
    m_customerAtlasId = game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/Customer1SpriteAtlas.json", 
        "game_assets/images/Customer1.png", 
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/ChairSpriteAtlas.json",
        "game_assets/images/Street_Food_Chair_1.png",
        game->GetRenderSystem().GetRenderer()
    );

    const auto* chairTex = game->GetResources().GetTexture("game_assets/images/Street_Food_Chair_1.png");
    if (!chairTex) {
        LOG_ERROR("Failed to get chair texture");
    }

    for (size_t i = 0; i < 2; i++)
    {
        auto seat = m_world->SpawnActor<Seat>(chairTex ? *chairTex : Components::Texture{});
        m_seat.push_back(seat);

        auto& transform = seat->GetComponent<Components::Transform2D>();
        float x = (i * 40);
        transform.position = { x, -70.0f };
    }
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

    // Customer management logic
    m_spawnTimer += deltaTime;
    if (m_spawnTimer >= 3.0) {
        m_spawnTimer = 0;
        m_customers.push_back(m_world->SpawnActor<Customer>(m_customerAtlasId));
    }

    int waitingIndex = 0;
    for (auto& actor : m_customers) {
        auto customer = std::static_pointer_cast<Customer>(actor);
        if (customer->m_state == Customer::State::Waiting) {
            customer->m_queueIndex = waitingIndex++;
            
            // Try to assign seat
            for (auto& seatActor : m_seat) {
                auto seat = std::static_pointer_cast<Seat>(seatActor);
                if (seat->isEmpty) {
                    seat->isEmpty = false;
                    customer->m_targetSeat = seat;
                    customer->m_state = Customer::State::MovingToSeat;
                    break;
                }
            }
        }
    }

    // Cleanup finished customers and collect money
    auto it = m_customers.begin();
    while (it != m_customers.end()) {
        auto customer = std::static_pointer_cast<Customer>(*it);
        if (customer->m_state == Customer::State::Finished) {
            m_currentMoney += 10;
            it = m_customers.erase(it);
        } else {
            ++it;
        }
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
    ui.Label(std::to_string(m_currentMoney), 10, 10, m_Font, {255,255,0,255}, Components::TextAlign::Center);

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


