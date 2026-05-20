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
#include "Character.h"
#include "Desk.h"

namespace {
constexpr const char* CustomerAtlasPath = "game_assets/json/Customer1SpriteAtlas.json";
constexpr const char* CustomerTexturePath = "game_assets/images/Customer1.png";
constexpr const char* ChairAtlasPath = "game_assets/json/ChairSpriteAtlas.json";
constexpr const char* ChairTexturePath = "game_assets/images/Street_Food_Chair_1.png";
constexpr const char* FoodCartTexturePath = "game_assets/images/FoodCart.png";
constexpr const char* FoodCartAtlasPath = "game_assets/json/StreetFoodCart.json";
constexpr const char* StaffAtlasPath = "game_assets/json/Staff1SpriteAtlas.json";
constexpr const char* StaffTexturePath = "game_assets/images/Staff.png";

}

struct FoodCartConfig {
    Components::Texture texture = {};
    Vec2 position = { 0.0f, 0.0f };
    int layer = 4;
};

class FoodCart : public Core::Actor {

public:
    // Inherited via Actor
    FoodCart(std::shared_ptr<Core::World> world, FoodCartConfig config): Actor(world), m_config(config) {

    }

    void OnUpdate(float dt) override
    {
    }
    void OnFixedUpdate(float dt) override
    {
    }
    void OnStart() override
    {
        auto& transform = AddComponent<Components::Transform2D>();
        transform.position = m_config.position;

        auto& sprite = AddComponent<Components::Sprite>();
        sprite.texture = m_config.texture;
        sprite.source = { 0, 0, 96, 96 };
        sprite.layer = m_config.layer;
    }
    void OnDestroy() override
    {
    }
private: 
    const FoodCartConfig m_config;

};

struct StaffConfig {
    Components::Texture texture = {};
    Vec2 position = { 0.0f, 0.0f };
    int layer = 4;
};


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
    game->GetResources().GetOrLoadSpriteAtlas(
        FoodCartAtlasPath,
        FoodCartTexturePath,
        game->GetRenderSystem().GetRenderer()
    );
    const auto staffAtlas  =  game->GetResources().GetOrLoadSpriteAtlas(
        StaffAtlasPath,
        StaffTexturePath,
        game->GetRenderSystem().GetRenderer()
    );

    const auto* chairTex = game->GetResources().GetTexture(ChairTexturePath);

    const auto* deskTex4 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_4.png");
    const auto* deskTex5 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_5.png");
    const auto* deskTex6 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_6.png");
    const auto* deskTex7 = game->GetResources().GetTexture("game_assets/images/Street_Food_Table_7.png");
    const auto* foodCartTex = game->GetResources().GetTexture(FoodCartTexturePath);


    if (!chairTex || !deskTex4 || !deskTex5 || !deskTex6 || !deskTex7 || !foodCartTex ) {
        LOG_ERROR("Failed to get gameplay table/chair textures");
        return;
    }

    auto spawnDesk = [&](Vec2 position, bool isLeft) {
        auto desk = m_world->SpawnActor<Desk>(DeskConfig{
            .emptyTexture = isLeft ? *deskTex5 : *deskTex6,
            .occupiedTexture = isLeft ? *deskTex4 : *deskTex7,
            .position = position,
            .layer = 2,
        });
        m_desks.push_back(desk);
        return desk;
    };

    auto spawnSeat = [&](Vec2 position, SeatDirection direction, std::shared_ptr<Desk> desk) {
        auto seat = m_world->SpawnActor<Seat>(SeatConfig{
            .texture = *chairTex,
            .position = position,
            .layer = 2,
            .direction = direction,
        });
        seat->SetDesk(desk);
        m_seats.push_back(seat);
    };

    auto spawnTwoSeatTable = [&](Vec2 deskPos) {
        Vec2 leftPos = deskPos - Vec2(16, 0);
        Vec2 rightPos = deskPos + Vec2(16, 0);
        auto deskLeft = spawnDesk(leftPos, true);
        auto deskRight = spawnDesk(rightPos, false);


        spawnSeat({ leftPos.x - 35.0f, leftPos.y + 10.0f }, SeatDirection::Right, deskLeft);
        spawnSeat({ rightPos.x + 35.0f, rightPos.y + 10.0f }, SeatDirection::Left, deskRight);
    };

    spawnTwoSeatTable({ -150.0f, 0.0f });
    spawnTwoSeatTable({ 0.0f, 0.0f });
    spawnTwoSeatTable({ 150.0f, -0.0f });

    auto foodCart = m_world->SpawnActor<FoodCart>(FoodCartConfig{
           .texture = *foodCartTex,
           .position = {0,-50},
           .layer = 2
        });
    m_FoodCart.push_back(foodCart);
    auto staff = m_world->SpawnActor<Staff>(staffAtlas);
   auto& staffTransform = staff->GetComponent<Components::Transform2D>();
   staffTransform.position = { -10,-30 };
    m_staffs.push_back(staff);
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
    if (m_spawnTimer >= 10.0) {
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
                if (seat->IsAvailable()) {
                    seat->Reserve();
                    customer->m_targetSeat = seat;
                    customer->m_state = Customer::State::MovingToSeat;
                    break;
                }
            }
        }
    }

    Vec2 foodCartPosition = { 0.0f, -50.0f };
    if (!m_FoodCart.empty() && m_FoodCart.front()) {
        foodCartPosition = m_FoodCart.front()->GetComponent<Components::Transform2D>().position;
    }

    for (auto& actor : m_staffs) {
        auto staff = std::static_pointer_cast<Staff>(actor);
        staff->SetWorkContext(&m_customers, &m_desks, foodCartPosition);
    }

    // Cleanup finished customers and collect money
    auto it = m_customers.begin();
    while (it != m_customers.end()) {
        auto customer = std::static_pointer_cast<Customer>(*it);
        if (customer->m_state == Customer::State::Finished) {
            m_currentMoney += 10;
            m_world->RemoveActor(*it);
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


