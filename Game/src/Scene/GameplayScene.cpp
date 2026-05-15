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
#include "Desk.h"

namespace {
constexpr const char* CustomerAtlasPath = "game_assets/json/Customer1SpriteAtlas.json";
constexpr const char* CustomerTexturePath = "game_assets/images/Customer1.png";
constexpr const char* ChairAtlasPath = "game_assets/json/ChairSpriteAtlas.json";
constexpr const char* ChairTexturePath = "game_assets/images/Street_Food_Chair_1.png";
}




void GameplayScene::Initialize()
{
    auto game = m_game.lock();
    if (!game)
    {
        LOG_INFO("Game is empty");
        return;
    }
    m_Font = game->GetResources().GetFontId(EngieResources::DEFAULT_FONT, 24);
    //add camera
    entt::entity cam = m_world->Registry.create();
    m_world->Registry.emplace<Components::CameraComponent>(cam,Vec2{0,0}, 1280.0f,720.0f);

    m_backgroundActor = m_world->SpawnActor<Background>();
    
    
   
    // Load customer atlas
    m_customerAtlasId = game->GetResources().GetOrLoadSpriteAtlas(
        CustomerAtlasPath,
        CustomerTexturePath,
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/StreetFoodTable4.json",
        "game_assets/images/Street_Food_Table_4.png",
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/StreetFoodTable5.json",
        "game_assets/images/Street_Food_Table_5.png",
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/StreetFoodTable6.json",
        "game_assets/images/Street_Food_Table_6.png",
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        "game_assets/json/StreetFoodTable7.json",
        "game_assets/images/Street_Food_Table_7.png",
        game->GetRenderSystem().GetRenderer()
    );
    game->GetResources().GetOrLoadSpriteAtlas(
        ChairAtlasPath,
        ChairTexturePath,
        game->GetRenderSystem().GetRenderer()
    );

    const auto* chairTex = game->GetResources().GetTexture(ChairTexturePath);

    const auto* deskTex4 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_4.png");
    const auto* deskTex5 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_5.png");
    const auto* deskTex6 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_6.png");
    const auto* deskTex7 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_7.png");


    if (!chairTex || !deskTex4 || !deskTex5 || !deskTex6 || !deskTex7) {
        LOG_ERROR("Failed to get gameplay table/chair textures");
        return;
    }

    for (size_t i = 0; i < 2; i++)
    {
        const float seatX = static_cast<float>(i * 35);
        const float deskX = static_cast<float>(i * 32);

        auto seat = m_world->SpawnActor<Seat>(SeatConfig{
            .texture = *chairTex,
            .position = { seatX, -70.0f },
            .layer = 2,
        });
        auto desk = m_world->SpawnActor<Desk>(DeskConfig{
            .emptyTexture = i == 0 ? *deskTex5 : *deskTex6,
            .occupiedTexture = i == 0 ? *deskTex4 : *deskTex7,
            .position = { deskX, -110.0f },
            .layer = 2,
        });

        seat->SetDesk(desk);
        m_seats.push_back(seat);
        m_desks.push_back(desk);
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
            for (auto& seat : m_seats) {
                if (seat->IsEmpty()) {
                    seat->Reserve();
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

std::string GameplayScene::GetCurrentTimeString(double time)
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


