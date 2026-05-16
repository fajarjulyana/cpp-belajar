// ============================================================
// File  : game_loop_sim.cpp
// Topik : Pola Game Loop SFML — Simulasi ASCII (tanpa SFML)
//
// Mendemonstrasikan pola arsitektur game pakai SFML:
//   - Delta time
//   - Fixed timestep update
//   - Entity dengan komponen Position + Velocity
//   - Collision detection sederhana
//   - State machine (Menu, Bermain, Game Over)
//
// Kode SFML sesungguhnya ada di komentar di atas setiap bagian.
//
// Kompilasi:
//   g++ -std=c++17 -o game_sim game_loop_sim.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <random>

// ============================================================
// Vektor 2D (sama seperti sf::Vector2f di SFML)
// ============================================================
struct Vec2 {
    float x = 0, y = 0;
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator*(float s)       const { return {x*s,   y*s};   }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    float length() const { return std::sqrt(x*x + y*y); }
};

// ============================================================
// Warna ANSI
// ============================================================
const char* RESET  = "\033[0m";
const char* MERAH  = "\033[31m";
const char* HIJAU  = "\033[32m";
const char* KUNING = "\033[33m";
const char* BIRU   = "\033[34m";
const char* CYAN   = "\033[36m";
const char* TEBAL  = "\033[1m";

// ============================================================
// Layar ASCII (simulasi RenderWindow)
// ============================================================
const int LEBAR = 60;
const int TINGGI = 20;

struct Layar {
    char grid[TINGGI][LEBAR + 1];
    std::string warnaGrid[TINGGI][LEBAR];

    void bersihkan(char bg = ' ') {
        for (int y = 0; y < TINGGI; ++y) {
            for (int x = 0; x < LEBAR; ++x) {
                grid[y][x] = bg;
                warnaGrid[y][x] = "";
            }
            grid[y][LEBAR] = '\0';
        }
    }

    void tulis(int x, int y, char c, const char* warna = "") {
        if (x < 0 || x >= LEBAR || y < 0 || y >= TINGGI) return;
        grid[y][x] = c;
        warnaGrid[y][x] = warna;
    }

    void tampilkan(int skor, int nyawa, const std::string& status) {
        // Pindahkan kursor ke atas (ANSI escape)
        std::cout << "\033[" << (TINGGI + 3) << "A";

        // Border atas
        std::cout << "+" << std::string(LEBAR, '-') << "+\n";

        for (int y = 0; y < TINGGI; ++y) {
            std::cout << "|";
            for (int x = 0; x < LEBAR; ++x) {
                if (!warnaGrid[y][x].empty())
                    std::cout << warnaGrid[y][x];
                std::cout << grid[y][x];
                if (!warnaGrid[y][x].empty())
                    std::cout << RESET;
            }
            std::cout << "|\n";
        }

        // Border bawah + HUD
        std::cout << "+" << std::string(LEBAR, '-') << "+\n";
        std::cout << TEBAL << " SKOR: " << std::setw(5) << skor
                  << "  NYAWA: " << nyawa
                  << "  STATUS: " << status
                  << std::string(20, ' ') << RESET << "\n";
        std::cout.flush();
    }
};

// ============================================================
// Entity (setiap objek game: pemain, musuh, peluru)
// ============================================================
struct Entity {
    Vec2    pos;
    Vec2    vel;
    char    simbol;
    const char* warna;
    bool    aktif = true;
    float   radius;
    std::string tipe;

    void update(float dt) {
        pos += vel * dt;
    }
};

// ============================================================
// Collision Detection (lingkaran)
// ============================================================
bool bertabrakan(const Entity& a, const Entity& b) {
    float dx = a.pos.x - b.pos.x;
    float dy = a.pos.y - b.pos.y;
    float jarak = std::sqrt(dx*dx + dy*dy);
    return jarak < (a.radius + b.radius);
}

// ============================================================
// State Machine Game
// ============================================================
enum class GameState { Menu, Bermain, GameOver };

// ============================================================
// Kelas Game Utama
// Dalam SFML nyata, ini akan inherit dari sf::Drawable atau
// dikelola oleh Application class dengan sf::RenderWindow.
// ============================================================
class Game {
public:
    Layar           layar;
    Entity          pemain;
    std::vector<Entity> musuh;
    std::vector<Entity> peluru;
    GameState       state = GameState::Menu;
    int             skor  = 0;
    int             nyawa = 3;
    float           timerMusuh = 0;
    float           timerTembak = 0;
    std::mt19937    rng{42};

    void init() {
        // Inisialisasi pemain (di tengah bawah)
        // SFML: sprite.setPosition(LEBAR/2, TINGGI-2);
        pemain = {{(float)LEBAR/2, (float)TINGGI - 2}, {0, 0},
                  'A', HIJAU, true, 1.5f, "pemain"};
    }

    void spawnMusuh() {
        std::uniform_int_distribution<int> distX(2, LEBAR - 3);
        std::uniform_real_distribution<float> distVx(-5.f, 5.f);
        Entity m;
        m.pos    = {(float)distX(rng), 1.f};
        m.vel    = {distVx(rng), 4.f};
        m.simbol = 'V';
        m.warna  = MERAH;
        m.radius = 1.f;
        m.tipe   = "musuh";
        musuh.push_back(m);
    }

    void tembak() {
        // SFML: sound.play(); + buat sf::CircleShape baru
        Entity p;
        p.pos    = {pemain.pos.x, pemain.pos.y - 1};
        p.vel    = {0, -25.f};
        p.simbol = '|';
        p.warna  = KUNING;
        p.radius = 0.5f;
        p.tipe   = "peluru";
        peluru.push_back(p);
    }

    // Update dipanggil setiap frame dengan dt = delta time (detik)
    // SFML: dipanggil dalam game loop sebelum window.display()
    void update(float dt, int frame) {
        if (state != GameState::Bermain) return;

        // Gerak pemain otomatis (simulasi — di SFML diambil dari input)
        float arahX = std::sin((float)frame * 0.08f) * 8.f;
        pemain.vel.x = arahX;
        pemain.update(dt);
        pemain.pos.x = std::max(1.f, std::min((float)LEBAR-2, pemain.pos.x));

        // Tembak otomatis setiap 0.5 detik
        timerTembak += dt;
        if (timerTembak >= 0.5f) {
            tembak();
            timerTembak = 0;
        }

        // Spawn musuh setiap 1.2 detik
        timerMusuh += dt;
        if (timerMusuh >= 1.2f) {
            spawnMusuh();
            timerMusuh = 0;
        }

        // Update peluru
        for (auto& p : peluru) p.update(dt);

        // Update musuh + pantul dinding
        for (auto& m : musuh) {
            m.update(dt);
            if (m.pos.x <= 1 || m.pos.x >= LEBAR - 2) m.vel.x *= -1;
        }

        // Collision: peluru vs musuh
        for (auto& p : peluru) {
            if (!p.aktif) continue;
            for (auto& m : musuh) {
                if (!m.aktif) continue;
                if (bertabrakan(p, m)) {
                    p.aktif = false;
                    m.aktif = false;
                    skor += 10;
                }
            }
        }

        // Collision: musuh vs pemain
        for (auto& m : musuh) {
            if (!m.aktif) continue;
            if (bertabrakan(m, pemain)) {
                m.aktif = false;
                --nyawa;
                if (nyawa <= 0) state = GameState::GameOver;
            }
        }

        // Hapus yang sudah tidak aktif atau keluar layar
        auto hapus = [](std::vector<Entity>& v) {
            v.erase(std::remove_if(v.begin(), v.end(),
                [](const Entity& e) {
                    return !e.aktif || e.pos.y < 0 || e.pos.y >= TINGGI;
                }), v.end());
        };
        hapus(peluru);
        hapus(musuh);
    }

    // Render dipanggil setelah update
    // SFML: window.clear() → draw(entity) → window.display()
    void render() {
        layar.bersihkan('.');

        // Gambar bintang background
        for (int i = 0; i < 8; ++i) {
            int bx = (i * 7 + 3) % LEBAR;
            int by = (i * 5 + 1) % TINGGI;
            layar.tulis(bx, by, '*');
        }

        // Gambar musuh
        for (auto& m : musuh)
            layar.tulis((int)m.pos.x, (int)m.pos.y, m.simbol, m.warna);

        // Gambar peluru
        for (auto& p : peluru)
            layar.tulis((int)p.pos.x, (int)p.pos.y, p.simbol, p.warna);

        // Gambar pemain
        layar.tulis((int)pemain.pos.x - 1, (int)pemain.pos.y, '(', pemain.warna);
        layar.tulis((int)pemain.pos.x,     (int)pemain.pos.y, pemain.simbol, pemain.warna);
        layar.tulis((int)pemain.pos.x + 1, (int)pemain.pos.y, ')', pemain.warna);

        std::string statusStr;
        if (state == GameState::Menu)     statusStr = "MENU";
        else if (state == GameState::Bermain) statusStr = "BERMAIN";
        else statusStr = std::string(MERAH) + "GAME OVER" + RESET;

        layar.tampilkan(skor, nyawa, statusStr);
    }
};

// ============================================================
// Main — Game Loop
// ============================================================
int main() {
    std::cout << "\n" << TEBAL
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Simulasi Game Loop SFML — Space Shooter    ║\n"
              << "║  Pattern: Delta Time + Fixed Update + ECS   ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << RESET << "\n";

    // Cetak baris kosong untuk area layar (agar \033[nA bekerja)
    for (int i = 0; i < TINGGI + 4; ++i) std::cout << "\n";

    Game game;
    game.init();

    // ---- Fase Menu ----
    game.state = GameState::Menu;
    for (int i = 0; i < 15; ++i) {
        game.layar.bersihkan('.');
        int cx = LEBAR / 2 - 8;
        std::string judul = "=== SPACE SHOOTER ===";
        for (int j = 0; j < (int)judul.size(); ++j)
            game.layar.tulis(cx + j, TINGGI/2 - 1, judul[j], TEBAL);
        std::string sub = "  [Memulai dalam " + std::to_string(15-i) + "...]  ";
        for (int j = 0; j < (int)sub.size(); ++j)
            game.layar.tulis(cx + j, TINGGI/2 + 1, sub[j]);
        game.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ---- Fase Bermain ----
    game.state = GameState::Bermain;
    auto waktuSebelum = std::chrono::steady_clock::now();

    // SFML game loop sesungguhnya:
    // while (window.isOpen()) {
    //     sf::Event event;
    //     while (window.pollEvent(event)) { ... }
    //     float dt = clock.restart().asSeconds();
    //     game.update(dt);
    //     window.clear();
    //     game.render(window);
    //     window.display();
    // }

    for (int frame = 0; frame < 120 && game.state != GameState::GameOver; ++frame) {
        auto sekarang = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(sekarang - waktuSebelum).count();
        waktuSebelum = sekarang;
        if (dt > 0.1f) dt = 0.1f;  // cap delta time

        game.update(dt, frame);
        game.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ---- Fase Game Over ----
    game.state = GameState::GameOver;
    for (int i = 0; i < 20; ++i) {
        game.layar.bersihkan(' ');
        int cx = LEBAR / 2 - 5;
        std::string go = "GAME OVER";
        for (int j = 0; j < (int)go.size(); ++j)
            game.layar.tulis(cx + j, TINGGI/2 - 1, go[j], MERAH);
        std::string sk = "Skor Akhir: " + std::to_string(game.skor);
        for (int j = 0; j < (int)sk.size(); ++j)
            game.layar.tulis(cx - 1 + j, TINGGI/2 + 1, sk[j], KUNING);
        game.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\n" << TEBAL << "Skor akhir: " << game.skor << RESET << "\n";
    std::cout << "Lihat README.md untuk kode SFML sesungguhnya!\n\n";
    return 0;
}
