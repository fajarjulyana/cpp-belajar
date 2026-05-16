// ============================================================
// File  : ecs.cpp
// Topik : Entity Component System (ECS) — Implementasi C++17
//
// Implementasi ECS ringan mirip EnTT / Unity DOTS:
//   - Entity = ID integer
//   - Component = struct data murni (POD)
//   - System = fungsi yang query & proses component
//   - World  = registry semua entity + component
//
// Kompilasi:
//   g++ -std=c++17 -o ecs ecs.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <functional>
#include <memory>
#include <any>
#include <typeindex>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <set>

const char* RST  = "\033[0m";
const char* BOLD = "\033[1m";
const char* GRN  = "\033[32m";
const char* YLW  = "\033[33m";
const char* RED  = "\033[31m";
const char* BLU  = "\033[34m";
const char* CYN  = "\033[36m";
const char* MAG  = "\033[35m";

void judul(const std::string& s) {
    std::cout << "\n" << BOLD << BLU << "══ " << s << " ══" << RST << "\n";
}

// ============================================================
// Entity — hanya sebuah integer ID
// ============================================================
using EntityId = uint32_t;
const EntityId INVALID_ENTITY = 0;

// ============================================================
// Components — data murni, tidak ada logika
// ============================================================
struct Position {
    float x = 0, y = 0;
};

struct Velocity {
    float vx = 0, vy = 0;
};

struct Health {
    int hp    = 100;
    int maxHp = 100;
    bool mati() const { return hp <= 0; }
};

struct Damage {
    int nilai   = 10;
    float range = 1.5f;
};

struct Renderable {
    char  simbol = '?';
    const char* warna = RST;
    std::string nama;
};

struct AI {
    float kecepatan   = 2.0f;
    float deteksiRange = 8.0f;
    EntityId target   = INVALID_ENTITY;
    enum class State { Patroli, Kejar, Serang } state = State::Patroli;
};

struct Player {};   // Tag component — tidak ada data, hanya penanda

struct Loot {
    int exp = 0;
    int gold = 0;
};

// ============================================================
// World / Registry — menyimpan semua entity & component
// ============================================================
class World {
    EntityId nextId_ = 1;

    // Setiap tipe component punya map: EntityId → component
    std::unordered_map<std::type_index,
        std::unordered_map<EntityId, std::any>> store_;

    std::set<EntityId> entities_;
    std::set<EntityId> toDestroy_;

public:
    // ── Entity management ──
    EntityId createEntity() {
        EntityId id = nextId_++;
        entities_.insert(id);
        return id;
    }

    void markForDestroy(EntityId id) { toDestroy_.insert(id); }

    void flushDestroyed() {
        for (EntityId id : toDestroy_) {
            entities_.erase(id);
            for (auto& [type, map] : store_) map.erase(id);
        }
        toDestroy_.clear();
    }

    size_t entityCount() const { return entities_.size(); }

    // ── Component management ──
    template<typename T>
    T& add(EntityId id, T component = T{}) {
        auto key = std::type_index(typeid(T));
        store_[key][id] = std::move(component);
        return std::any_cast<T&>(store_[key][id]);
    }

    template<typename T>
    bool has(EntityId id) const {
        auto key = std::type_index(typeid(T));
        auto it = store_.find(key);
        if (it == store_.end()) return false;
        return it->second.count(id) > 0;
    }

    template<typename T>
    T* get(EntityId id) {
        auto key = std::type_index(typeid(T));
        auto it = store_.find(key);
        if (it == store_.end()) return nullptr;
        auto jt = it->second.find(id);
        if (jt == it->second.end()) return nullptr;
        return std::any_cast<T>(&jt->second);
    }

    template<typename T>
    void remove(EntityId id) {
        store_[std::type_index(typeid(T))].erase(id);
    }

    // ── Query — semua entity yang punya semua T... ──
    template<typename... T>
    std::vector<EntityId> query() {
        std::vector<EntityId> hasil;
        for (EntityId id : entities_) {
            if ((has<T>(id) && ...))
                hasil.push_back(id);
        }
        return hasil;
    }
};

// ============================================================
// Systems — fungsi yang bekerja pada entity dengan component tertentu
// ============================================================

// MovementSystem: update Position berdasarkan Velocity
void movementSystem(World& w, float dt) {
    for (EntityId id : w.query<Position, Velocity>()) {
        auto* pos = w.get<Position>(id);
        auto* vel = w.get<Velocity>(id);
        pos->x += vel->vx * dt;
        pos->y += vel->vy * dt;
    }
}

// AISystem: musuh mengejar player
void aiSystem(World& w, float dt) {
    // Cari posisi player
    Position* playerPos = nullptr;
    EntityId playerEnt = INVALID_ENTITY;
    for (EntityId id : w.query<Player, Position>()) {
        playerPos = w.get<Position>(id);
        playerEnt = id;
        break;
    }
    if (!playerPos) return;

    for (EntityId id : w.query<AI, Position, Velocity>()) {
        auto* ai  = w.get<AI>(id);
        auto* pos = w.get<Position>(id);
        auto* vel = w.get<Velocity>(id);

        float dx = playerPos->x - pos->x;
        float dy = playerPos->y - pos->y;
        float jarak = std::sqrt(dx*dx + dy*dy);

        auto* dmg = w.get<Damage>(id);
        float range = dmg ? dmg->range : 1.5f;

        if (jarak <= range) {
            ai->state  = AI::State::Serang;
            ai->target = playerEnt;
            vel->vx = vel->vy = 0;
        } else if (jarak <= ai->deteksiRange) {
            ai->state  = AI::State::Kejar;
            ai->target = playerEnt;
            vel->vx = (dx / jarak) * ai->kecepatan;
            vel->vy = (dy / jarak) * ai->kecepatan;
        } else {
            ai->state  = AI::State::Patroli;
            ai->target = INVALID_ENTITY;
            // Patroli: bergerak sedikit ke kanan
            vel->vx = 0.5f;
            vel->vy = 0;
        }
    }
}

// CombatSystem: musuh yang dekat menyerang player
void combatSystem(World& w) {
    Position* playerPos = nullptr;
    Health*   playerHp  = nullptr;
    for (EntityId id : w.query<Player, Position, Health>()) {
        playerPos = w.get<Position>(id);
        playerHp  = w.get<Health>(id);
        break;
    }
    if (!playerPos) return;

    for (EntityId id : w.query<AI, Position, Damage>()) {
        auto* ai  = w.get<AI>(id);
        auto* pos = w.get<Position>(id);
        auto* dmg = w.get<Damage>(id);

        if (ai->state != AI::State::Serang) continue;

        float dx = playerPos->x - pos->x;
        float dy = playerPos->y - pos->y;
        float jarak = std::sqrt(dx*dx + dy*dy);

        if (jarak <= dmg->range && playerHp) {
            playerHp->hp -= dmg->nilai;
            auto* rend = w.get<Renderable>(id);
            std::string nama = rend ? rend->nama : "Musuh";
            std::cout << "    " << RED << "[Combat]" << RST
                      << " " << nama << " menyerang Player! HP:"
                      << playerHp->hp << "/" << playerHp->maxHp << "\n";
        }
    }
}

// HealthSystem: hapus entity yang mati, berikan loot
void healthSystem(World& w) {
    Health* playerHp = nullptr;
    for (EntityId id : w.query<Player, Health>()) {
        playerHp = w.get<Health>(id);
        break;
    }

    for (EntityId id : w.query<Health, Renderable>()) {
        auto* hp   = w.get<Health>(id);
        auto* rend = w.get<Renderable>(id);
        if (!hp->mati() || w.has<Player>(id)) continue;

        std::cout << "    " << YLW << "[Health]" << RST
                  << " " << rend->nama << " mati!\n";

        auto* loot = w.get<Loot>(id);
        if (loot && playerHp) {
            std::cout << "    " << GRN << "[Loot]" << RST
                      << " +EXP:" << loot->exp << " +Gold:" << loot->gold << "\n";
        }

        w.markForDestroy(id);
    }
    w.flushDestroyed();
}

// RenderSystem: gambar semua entity dengan Renderable ke ASCII grid
void renderSystem(World& w, int gridW = 20, int gridH = 10) {
    std::vector<std::string> grid(gridH, std::string(gridW, '.'));
    std::vector<std::pair<int,int>> tulis;

    struct Cell { char s; const char* warna; std::string nama; };
    std::vector<std::vector<Cell>> cells(gridH,
        std::vector<Cell>(gridW, {'.', RST, ""}));

    for (EntityId id : w.query<Position, Renderable>()) {
        auto* pos  = w.get<Position>(id);
        auto* rend = w.get<Renderable>(id);
        int gx = std::max(0, std::min(gridW-1, (int)pos->x));
        int gy = std::max(0, std::min(gridH-1, (int)pos->y));
        cells[gy][gx] = {rend->simbol, rend->warna, rend->nama};
    }

    std::cout << "  +" << std::string(gridW, '-') << "+\n";
    for (int y = 0; y < gridH; ++y) {
        std::cout << "  |";
        for (int x = 0; x < gridW; ++x) {
            auto& c = cells[y][x];
            std::cout << c.warna << c.s << RST;
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(gridW, '-') << "+\n";

    // Legenda
    std::cout << "  ";
    for (EntityId id : w.query<Position, Renderable>()) {
        auto* rend = w.get<Renderable>(id);
        auto* hp   = w.get<Health>(id);
        std::cout << rend->warna << rend->simbol << RST
                  << "=" << rend->nama;
        if (hp) std::cout << "(" << hp->hp << "hp)";
        std::cout << " ";
    }
    std::cout << "\n";
}

// ============================================================
// Factory functions — buat entity siap pakai
// ============================================================
EntityId buatPlayer(World& w, float x, float y) {
    EntityId id = w.createEntity();
    w.add<Position>(id, {x, y});
    w.add<Velocity>(id, {0, 0});
    w.add<Health>(id, {100, 100});
    w.add<Player>(id);
    w.add<Renderable>(id, {'@', GRN, "Player"});
    return id;
}

EntityId buatMusuh(World& w, float x, float y,
                   const std::string& nama, char simbol,
                   int hp, int dmg, int exp, int gold) {
    EntityId id = w.createEntity();
    w.add<Position>(id, {x, y});
    w.add<Velocity>(id, {0, 0});
    w.add<Health>(id, {hp, hp});
    w.add<Damage>(id, {dmg, 1.5f});
    w.add<AI>(id, {3.0f, 7.0f});
    w.add<Renderable>(id, {simbol, RED, nama});
    w.add<Loot>(id, {exp, gold});
    return id;
}

EntityId buatItem(World& w, float x, float y, const std::string& nama, char s) {
    EntityId id = w.createEntity();
    w.add<Position>(id, {x, y});
    w.add<Renderable>(id, {s, YLW, nama});
    return id;
}

// ============================================================
// Demo
// ============================================================
void demoECS() {
    judul("Entity Component System — Mini Game Simulation");

    World w;

    // Buat dunia
    EntityId player  = buatPlayer(w, 2, 5);
    EntityId goblin  = buatMusuh(w, 15, 3, "Goblin",   'g', 30, 8, 50, 10);
    buatMusuh(w, 18, 7, "Orc",      'O', 60, 15, 100, 25);
    EntityId skeleton= buatMusuh(w, 12, 8, "Skeleton", 's', 20, 5, 30, 5);

    buatItem(w, 5, 2, "Kunci", 'K');
    buatItem(w, 10, 6, "Chest", 'C');

    std::cout << "\n  Entity dibuat: " << w.entityCount() << "\n";
    std::cout << "\n  Frame 0 — State awal:\n";
    renderSystem(w);

    // Simulasi beberapa frame
    const float DT = 0.5f;

    for (int frame = 1; frame <= 6; ++frame) {
        std::cout << "\n  " << BOLD << "── Frame " << frame << " ──" << RST << "\n";

        // Simulasi player bergerak ke kanan
        if (auto* vel = w.get<Velocity>(player)) {
            vel->vx = 1.5f;
            vel->vy = (frame % 2 == 0) ? 0.3f : -0.1f;
        }

        aiSystem(w, DT);
        movementSystem(w, DT);
        combatSystem(w);
        healthSystem(w);

        if (frame % 2 == 0) {
            std::cout << "  Render frame " << frame << ":\n";
            renderSystem(w);
        }

        // Simulasi: musuh terkena serangan player
        if (frame == 3) {
            std::cout << "    " << GRN << "[Player]" << RST
                      << " Menyerang Goblin!\n";
            if (auto* hp = w.get<Health>(goblin)) {
                hp->hp -= 25;
                std::cout << "    " << YLW << "[Combat]" << RST
                          << " Goblin HP: " << hp->hp << "/30\n";
            }
        }
        if (frame == 4) {
            if (auto* hp = w.get<Health>(goblin)) {
                hp->hp = 0;
            }
            if (auto* hp = w.get<Health>(skeleton)) {
                hp->hp = 0;
            }
            healthSystem(w);
        }

        if (auto* hp = w.get<Health>(player); hp && hp->mati()) {
            std::cout << "    " << RED << "GAME OVER! Player mati.\n" << RST;
            break;
        }
    }

    std::cout << "\n  Frame akhir:\n";
    renderSystem(w);

    std::cout << "\n  Entity tersisa: " << w.entityCount() << "\n";
    std::cout << "\n  " << BOLD << "Query examples:" << RST << "\n";
    std::cout << "    Entity dengan AI+Health: "
              << w.query<AI, Health>().size() << "\n";
    std::cout << "    Entity dengan Renderable: "
              << w.query<Renderable>().size() << "\n";
    std::cout << "    Entity Player: "
              << w.query<Player>().size() << "\n";
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n" << BOLD
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Entity Component System (ECS)               ║\n"
              << "║  Entity=ID | Component=Data | System=Logic   ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << RST;

    demoECS();

    std::cout << "\n" << BOLD
              << "─────────────────────────────────────────────\n"
              << " Ringkasan ECS:\n"
              << "  Entity     → uint32 ID, tidak ada data\n"
              << "  Component  → struct data murni (POD)\n"
              << "  System     → fungsi yang query & proses\n"
              << "  World      → registry (type-erased map)\n"
              << "  query<T..> → semua entity yang punya semua T\n"
              << "─────────────────────────────────────────────\n"
              << RST << "\n";
    return 0;
}
