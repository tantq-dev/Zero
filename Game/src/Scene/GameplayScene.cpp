#include "Logger.h"
#include "Scene/GameplayScene.h"
#include "Game.h"
#include "UISystem.h"
#include "Background.h"
#include "Seat.h"
#include "Character.h"
#include "Desk.h"
#include "DreamyGameInstance.h"
#include "RestaurantGameMode.h"
#include "EventRegistry.h"
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#undef max
#undef min

// ---- Asset paths ------------------------------------------------------------
namespace {
constexpr const char* CustomerAtlasPath   = "game_assets/json/Customer1SpriteAtlas.json";
constexpr const char* CustomerTexturePath = "game_assets/images/Customer1.png";
constexpr const char* ChairAtlasPath      = "game_assets/json/ChairSpriteAtlas.json";
constexpr const char* ChairTexturePath    = "game_assets/images/Street_Food_Chair_1.png";
constexpr const char* FoodCartTexturePath = "game_assets/images/FoodCart.png";
constexpr const char* FoodCartAtlasPath   = "game_assets/json/StreetFoodCart.json";
constexpr const char* StaffAtlasPath      = "game_assets/json/Staff1SpriteAtlas.json";
constexpr const char* StaffTexturePath    = "game_assets/images/Staff.png";

// Table asset paths indexed as {empty L, empty R, occupied L, occupied R}
constexpr const char* Table4Json = "game_assets/json/StreetFoodTable4.json";
constexpr const char* Table4Png  = "game_assets/images/Street_Food_Table_4.png";
constexpr const char* Table5Json = "game_assets/json/StreetFoodTable5.json";
constexpr const char* Table5Png  = "game_assets/images/Street_Food_Table_5.png";
constexpr const char* Table6Json = "game_assets/json/StreetFoodTable6.json";
constexpr const char* Table6Png  = "game_assets/images/Street_Food_Table_6.png";
constexpr const char* Table7Json = "game_assets/json/StreetFoodTable7.json";
constexpr const char* Table7Png  = "game_assets/images/Street_Food_Table_7.png";
} // namespace

// ---- FoodCart actor (local, unchanged) --------------------------------------
struct FoodCartConfig {
    Components::Texture texture = {};
    Vec2 position = { 0.0f, 0.0f };
    int layer = 4;
};

class FoodCart : public Core::Actor {
public:
    FoodCart(std::shared_ptr<Core::World> world, FoodCartConfig config)
        : Actor(world), m_config(config) {}
    void OnUpdate(float)       override {}
    void OnFixedUpdate(float)  override {}
    void OnStart()             override {
        auto& t = AddComponent<Components::Transform2D>();
        t.position = m_config.position;
        auto& s = AddComponent<Components::Sprite>();
        s.texture = m_config.texture;
        s.source  = { 0, 0, 96, 96 };
        s.layer   = m_config.layer;
    }
    void OnDestroy() override {}
private:
    const FoodCartConfig m_config;
};

// ---- Initialize -------------------------------------------------------------
void GameplayScene::Initialize()
{
    auto game = m_game.lock();
    if (!game) { LOG_INFO("Game is empty"); return; }

    auto& renderer = game->GetRenderSystem().GetRenderer();

    // ---- Fonts ----
    m_font      = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 20, renderer);
    m_fontBig   = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 28, renderer);
    m_fontEvent = game->GetResources().GetOrLoadFont(EngieResources::DEFAULT_FONT, 18, renderer);

    // ---- Bake simulation params from GameMode ----
    auto& gi = game->GetGameInstance<DreamyGameInstance>();
    auto& gm = m_world->SetGameMode<RestaurantGameMode>(gi.runState);

    m_spawnInterval    = gm.customerSpawnInterval;
    m_dayDurationSec   = gm.dayDurationSeconds;
    m_moneyPerCustomer = gm.moneyPerCustomer;
    m_customerPatience = gm.customerPatience;
    m_cookTimeMult     = gm.cookTimeMultiplier;
    m_hasDelivery      = gm.hasDeliveryCounter;
    m_deliveryIncome   = gm.deliveryIncomeRate;
    m_reputation       = std::clamp(gi.runState.reputation + gm.reputationBonus, 0, 100);

    // ---- Reset per-day counters ----
    m_dayTimer        = 0.0f;
    m_dayEnded        = false;
    m_spawnTimer      = 0.0f;
    m_deliveryTimer   = 0.0f;
    m_currentMoney    = 0;
    m_totalServed     = 0;
    m_satisfiedCount  = 0;
    m_eventPending    = false;
    m_eventTriggered  = false;
    m_activeEventId   = -1;
    m_eventSpawnMult  = 1.0f;
    m_eventPatMult    = 1.0f;
    m_clockTime       = 0.0;

    // ---- Camera ----
    entt::entity cam = m_world->Registry.create();
    m_world->Registry.emplace<Components::CameraComponent>(cam, Vec2{0,0}, 1280.0f, 720.0f);

    // ---- Background ----
    m_backgroundActor = m_world->SpawnActor<Background>();

    // ---- Load all textures / atlases ----
    m_customerAtlasId = game->GetResources().GetOrLoadSpriteAtlas(CustomerAtlasPath, CustomerTexturePath, renderer);
    game->GetResources().GetOrLoadSpriteAtlas(Table4Json, Table4Png, renderer);
    game->GetResources().GetOrLoadSpriteAtlas(Table5Json, Table5Png, renderer);
    game->GetResources().GetOrLoadSpriteAtlas(Table6Json, Table6Png, renderer);
    game->GetResources().GetOrLoadSpriteAtlas(Table7Json, Table7Png, renderer);
    game->GetResources().GetOrLoadSpriteAtlas(ChairAtlasPath,    ChairTexturePath,    renderer);
    game->GetResources().GetOrLoadSpriteAtlas(FoodCartAtlasPath, FoodCartTexturePath, renderer);
    const auto staffAtlasId = game->GetResources().GetOrLoadSpriteAtlas(StaffAtlasPath, StaffTexturePath, renderer);

    const auto* chairTex  = game->GetResources().GetTexture(ChairTexturePath);
    const auto* deskTex4  = game->GetResources().GetTexture(Table4Png);
    const auto* deskTex5  = game->GetResources().GetTexture(Table5Png);
    const auto* deskTex6  = game->GetResources().GetTexture(Table6Png);
    const auto* deskTex7  = game->GetResources().GetTexture(Table7Png);
    const auto* foodCartTex = game->GetResources().GetTexture(FoodCartTexturePath);

    if (!chairTex || !deskTex4 || !deskTex5 || !deskTex6 || !deskTex7 || !foodCartTex) {
        LOG_ERROR("Failed to get gameplay textures");
        return;
    }

    // ---- Spawn helpers ----
    auto spawnDesk = [&](Vec2 pos, bool isLeft) {
        auto desk = m_world->SpawnActor<Desk>(DeskConfig{
            .emptyTexture    = isLeft ? *deskTex5 : *deskTex6,
            .occupiedTexture = isLeft ? *deskTex4 : *deskTex7,
            .position = pos, .layer = 2,
        });
        m_desks.push_back(desk);
        return desk;
    };

    auto spawnSeat = [&](Vec2 pos, SeatDirection dir, std::shared_ptr<Desk> desk) {
        auto seat = m_world->SpawnActor<Seat>(SeatConfig{
            .texture = *chairTex, .position = pos, .layer = 2, .direction = dir,
        });
        seat->SetDesk(desk);
        m_seats.push_back(seat);
    };

    auto spawnTwoSeatTable = [&](Vec2 deskPos) {
        Vec2 lPos = deskPos - Vec2(16, 0);
        Vec2 rPos = deskPos + Vec2(16, 0);
        auto dL = spawnDesk(lPos, true);
        auto dR = spawnDesk(rPos, false);
        spawnSeat({ lPos.x - 35.0f, lPos.y + 10.0f }, SeatDirection::Right, dL);
        spawnSeat({ rPos.x + 35.0f, rPos.y + 10.0f }, SeatDirection::Left,  dR);
    };

    // ---- Base layout: 3 tables ----
    spawnTwoSeatTable({ -150.0f,  0.0f });
    spawnTwoSeatTable({    0.0f,  0.0f });
    spawnTwoSeatTable({  150.0f,  0.0f });

    // ---- Upgrade-driven extra tables ----
    for (int i = 0; i < gm.extraTableCount; ++i) {
        // Alternate: first row below, second row further below
        float yOffset = (i % 2 == 0) ? 90.0f : 180.0f;
        float xPos    = -150.0f + (i / 2) * 150.0f;
        spawnTwoSeatTable({ xPos, yOffset });
    }

    // ---- Food cart ----
    auto foodCart = m_world->SpawnActor<FoodCart>(FoodCartConfig{
        .texture = *foodCartTex, .position = {0, -50}, .layer = 2
    });
    m_FoodCart.push_back(foodCart);

    // ---- Staff (cook time affected by upgrade) ----
    auto staff = m_world->SpawnActor<Staff>(staffAtlasId);
    auto& staffTransform = staff->GetComponent<Components::Transform2D>();
    staffTransform.position = { -10, -30 };
    staff->m_cookTime *= m_cookTimeMult;
    m_staffs.push_back(staff);
}

// ---- Update -----------------------------------------------------------------
void GameplayScene::Update(const double& deltaTime)
{
    if (m_dayEnded) return;

    const float dt = static_cast<float>(deltaTime);

    // ---- Day timer ----
    m_dayTimer += dt;
    if (m_dayTimer >= m_dayDurationSec) {
        EndDay();
        return;
    }

    // ---- Cosmetic clock ----
    m_clockTime += deltaTime * 100.0;
    if (m_clockTime > 1440.0) m_clockTime = 1440.0;

    // ---- Day-night tint ----
    if (m_backgroundActor) {
        float t = static_cast<float>(std::sin((m_clockTime / 1440.0) * 3.14159));
        t = std::max(t, 0.0f);
        Components::Color night = { 20, 20, 40, 255 };
        Components::Color day   = { 135, 206, 235, 255 };
        auto& shape = m_backgroundActor->GetComponent<Components::Shape>();
        shape.color = Components::LerpColor(night, day, t);
    }

    // ---- Event trigger (fire once at ~50% of day) ----
    if (!m_eventTriggered && m_dayTimer >= m_dayDurationSec * 0.5f) {
        m_eventTriggered = true;
        // Pick a random event
        const int eventCount = static_cast<int>(EventRegistry::k_events.size());
        m_activeEventId = std::rand() % eventCount;
        m_eventPending  = true;
    }

    // Pause simulation while event overlay is shown
    if (m_eventPending) return;

    // ---- Customer spawn ----
    float effectiveInterval = m_spawnInterval / m_eventSpawnMult;
    m_spawnTimer += dt;
    if (m_spawnTimer >= effectiveInterval) {
        m_spawnTimer = 0.0f;
        auto customer = m_world->SpawnActor<Customer>(m_customerAtlasId);
        customer->m_patientTime = m_customerPatience * m_eventPatMult;
        m_customers.push_back(customer);
    }

    // ---- Assign waiting customers to free seats ----
    int waitingIndex = 0;
    for (auto& actor : m_customers) {
        auto customer = std::static_pointer_cast<Customer>(actor);
        if (customer->m_state == Customer::State::Waiting) {
            customer->m_queueIndex = waitingIndex++;
            for (auto& seat : m_seats) {
                if (seat->IsAvailable()) {
                    seat->Reserve();
                    customer->m_targetSeat = seat;
                    customer->ChangeState(Customer::State::MovingToSeat);
                    break;
                }
            }
        }
    }

    // ---- Staff work context ----
    Vec2 foodCartPos = { 0.0f, -50.0f };
    if (!m_FoodCart.empty() && m_FoodCart.front())
        foodCartPos = m_FoodCart.front()->GetComponent<Components::Transform2D>().position;

    for (auto& actor : m_staffs) {
        auto staff = std::static_pointer_cast<Staff>(actor);
        staff->SetWorkContext(&m_customers, &m_desks, foodCartPos);
    }

    // ---- Cleanup finished customers + collect money + update reputation ----
    auto it = m_customers.begin();
    while (it != m_customers.end()) {
        auto customer = std::static_pointer_cast<Customer>(*it);
        if (customer->m_state == Customer::State::Finished) {
            bool satisfied = customer->m_isHaveFood;
            m_totalServed++;
            if (satisfied) {
                m_satisfiedCount++;
                m_currentMoney += m_moneyPerCustomer;
                m_reputation = std::clamp(m_reputation + 2, 0, 100);
            } else {
                // Left without food (patience ran out)
                m_reputation = std::clamp(m_reputation - 3, 0, 100);
            }
            m_world->RemoveActor(*it);
            it = m_customers.erase(it);
        } else {
            ++it;
        }
    }

    // ---- Passive delivery income ----
    if (m_hasDelivery && m_deliveryIncome > 0.0f) {
        m_deliveryTimer += dt;
        if (m_deliveryTimer >= 1.0f) {
            m_currentMoney += static_cast<int>(m_deliveryIncome);
            m_deliveryTimer = 0.0f;
        }
    }
}

void GameplayScene::FixedUpdate(const double&) {}
void GameplayScene::Render(::IRenderer2D&)     {}
void GameplayScene::HandleInput()              {}

// ---- HandleUI ---------------------------------------------------------------
void GameplayScene::HandleUI(System::UISystem& ui)
{
    if (m_eventPending) {
        DrawEventOverlay(ui);
        return;
    }
    DrawHUD(ui);
}

// ---- HUD --------------------------------------------------------------------
void GameplayScene::DrawHUD(System::UISystem& ui)
{
    auto game = m_game.lock();
    if (!game) return;
    auto& gi = game->GetGameInstance<DreamyGameInstance>();

    // Top bar background
    ui.Panel({ 0, 0, 1280, 44 }, { 10, 8, 22, 210 });

    // Day / time
    std::string dayLabel = "Day " + std::to_string(gi.runState.currentDay) + " / 7";
    ui.Label(dayLabel, 10.0f, 10.0f, m_font, { 255, 220, 100, 255 }, Components::TextAlign::Left);
    ui.Label(GetCurrentTimeString(m_clockTime), 640.0f, 10.0f, m_font, { 255, 255, 255, 255 }, Components::TextAlign::Center);

    // Money
    std::string moneyStr = "$" + std::to_string(m_currentMoney);
    ui.Label(moneyStr, 1270.0f, 10.0f, m_font, { 100, 255, 120, 255 }, Components::TextAlign::Right);

    // Reputation bar
    float barW = 200.0f, barH = 12.0f;
    float barX = 1270.0f - barW;
    float barY = 30.0f;
    ui.Panel({ static_cast<float>(barX),                          static_cast<float>(barY),
               static_cast<float>(barW),                          static_cast<float>(barH) }, { 40, 20, 20, 200 });
    float fillW = barW * (static_cast<float>(m_reputation) / 100.0f);
    Components::Color repColor = m_reputation > 60 ? Components::Color{80, 200, 120, 255}
                               : m_reputation > 30 ? Components::Color{220, 180, 50, 255}
                                                   : Components::Color{200, 60, 60, 255};
    ui.Panel({ static_cast<float>(barX), static_cast<float>(barY),
               static_cast<float>(fillW), static_cast<float>(barH) }, repColor);
    ui.Label("REP", barX - 38.0f, barY - 1.0f, m_font, { 180, 180, 200, 255 }, Components::TextAlign::Left);

    // Day progress bar
    float progress = m_dayTimer / m_dayDurationSec;
    float pBarW = 400.0f, pBarH = 8.0f;
    float pBarX = 640.0f - pBarW * 0.5f;
    float pBarY = 34.0f;
    ui.Panel({ static_cast<float>(pBarX), static_cast<float>(pBarY),
               static_cast<float>(pBarW), static_cast<float>(pBarH) }, { 30, 30, 60, 200 });
    ui.Panel({ static_cast<float>(pBarX), static_cast<float>(pBarY),
               static_cast<float>(pBarW * progress), static_cast<float>(pBarH) }, { 80, 140, 220, 255 });

    // Delivery indicator
    if (m_hasDelivery) {
        ui.Label("📦 Delivery Active", 10.0f, 50.0f, m_font, { 100, 200, 255, 200 }, Components::TextAlign::Left);
    }

    // Satisfaction counter
    std::string satStr = "Served: " + std::to_string(m_satisfiedCount) + " / " + std::to_string(m_totalServed);
    ui.Label(satStr, 10.0f, 70.0f, m_font, { 200, 200, 220, 200 }, Components::TextAlign::Left);
}

// ---- Event overlay ----------------------------------------------------------
void GameplayScene::DrawEventOverlay(System::UISystem& ui)
{
    if (m_activeEventId < 0) { m_eventPending = false; return; }
    const EventData* ev = EventRegistry::Find(m_activeEventId);
    if (!ev) { m_eventPending = false; return; }

    // Dim background
    ui.Panel({ 0, 0, 1280, 720 }, { 0, 0, 0, 160 });

    // Event card
    float cW = 660.0f, cH = 320.0f;
    float cX = (1280.0f - cW) * 0.5f;
    float cY = (720.0f  - cH) * 0.5f;

    ui.Panel({ static_cast<float>(cX),   static_cast<float>(cY),
               static_cast<float>(cW),   static_cast<float>(cH) }, { 22, 18, 45, 250 });
    ui.Panel({ static_cast<float>(cX)+3, static_cast<float>(cY)+3,
               static_cast<float>(cW)-6, static_cast<float>(cH)-6 }, { 80, 60, 140, 200 });
    ui.Panel({ static_cast<float>(cX)+5, static_cast<float>(cY)+5,
               static_cast<float>(cW)-10, static_cast<float>(cH)-10 }, { 22, 18, 45, 255 });

    // Event title
    ui.Label("⚡ Event!", cX + cW * 0.5f, cY + 18.0f, m_fontBig, { 255, 220, 80, 255 }, Components::TextAlign::Center);
    ui.Label(ev->name, cX + cW * 0.5f, cY + 60.0f, m_fontBig, { 255, 200, 150, 255 }, Components::TextAlign::Center);
    ui.Label(ev->description, cX + 30.0f, cY + 108.0f, m_fontEvent, { 200, 200, 220, 220 }, Components::TextAlign::Left);

    // Choice A
    float btnW = 240.0f, btnH = 52.0f;
    float aX = cX + 40.0f,         aY = cY + cH - 80.0f;
    float bX = cX + cW - 40.0f - btnW, bY = aY;

    ui.Panel({ static_cast<float>(aX), static_cast<float>(aY), static_cast<float>(btnW), static_cast<float>(btnH) }, { 60, 140, 90, 255 });
    ui.Label(ev->choiceAText, aX + btnW * 0.5f, aY + 14.0f, m_fontEvent, { 240, 255, 240, 255 }, Components::TextAlign::Center);

    ui.Panel({ static_cast<float>(bX), static_cast<float>(bY), static_cast<float>(btnW), static_cast<float>(btnH) }, { 140, 70, 60, 255 });
    ui.Label(ev->choiceBText, bX + btnW * 0.5f, bY + 14.0f, m_fontEvent, { 255, 230, 220, 255 }, Components::TextAlign::Center);

    if (ui.Button(ev->choiceAText,
        { static_cast<float>(aX), static_cast<float>(aY), static_cast<float>(btnW), static_cast<float>(btnH) },
        m_fontEvent))
    {
        ApplyEventConsequence(0);
    }
    if (ui.Button(ev->choiceBText,
        { static_cast<float>(bX), static_cast<float>(bY), static_cast<float>(btnW), static_cast<float>(btnH) },
        m_fontEvent))
    {
        ApplyEventConsequence(1);
    }
}

// ---- Event consequence application -----------------------------------------
void GameplayScene::ApplyEventConsequence(int choiceIndex)
{
    const EventData* ev = EventRegistry::Find(m_activeEventId);
    if (!ev) { m_eventPending = false; return; }

    const EventConsequence& c = (choiceIndex == 0) ? ev->choiceA : ev->choiceB;

    m_currentMoney     += c.moneyDelta;
    m_reputation        = std::clamp(m_reputation + c.reputationDelta, 0, 100);
    m_eventSpawnMult   *= c.spawnMultiplier;
    m_eventPatMult     *= c.patienceMultiplier;

    // Apply patience to already-spawned customers
    for (auto& actor : m_customers) {
        auto cust = std::static_pointer_cast<Customer>(actor);
        cust->m_patientTime *= c.patienceMultiplier;
    }

    m_eventPending  = false;
    m_activeEventId = -1;
}

// ---- End of day -------------------------------------------------------------
void GameplayScene::EndDay()
{
    if (m_dayEnded) return;
    m_dayEnded = true;

    auto game = m_game.lock();
    if (!game) return;

    auto& gi = game->GetGameInstance<DreamyGameInstance>();
    RunState& rs = gi.runState;

    // Accumulate into RunState
    rs.money               += m_currentMoney;
    rs.reputation           = std::clamp(m_reputation, 0, 100);
    rs.totalCustomersServed += m_totalServed;
    rs.satisfiedCustomers  += m_satisfiedCount;
    rs.currentDay++;

    if (rs.currentDay > 7) {
        game->SetActiveScene("ResultScene");
    } else {
        game->SetActiveScene("PreparationScene");
    }
}

// ---- Helpers ----------------------------------------------------------------
std::string GameplayScene::GetCurrentTimeString(double time) const
{
    uint32_t t = static_cast<uint32_t>(std::round(time));
    uint32_t h = t / 60, m = t % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << h
        << ":"
        << std::setw(2) << std::setfill('0') << m;
    return oss.str();
}
