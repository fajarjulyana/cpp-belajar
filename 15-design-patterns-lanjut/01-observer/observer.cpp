// ============================================================
// File  : observer.cpp
// Topik : Observer Pattern — 3 Variasi: Klasik, EventBus, Signal
//
// Kompilasi:
//   g++ -std=c++17 -o observer observer.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>
#include <any>
#include <typeindex>

// ============================================================
// ANSI Warna
// ============================================================
const char* RST  = "\033[0m";
const char* BOLD = "\033[1m";
const char* GRN  = "\033[32m";
const char* YLW  = "\033[33m";
const char* BLU  = "\033[34m";
const char* CYN  = "\033[36m";

void judul(const std::string& s) {
    std::cout << "\n" << BOLD << BLU << "══ " << s << " ══" << RST << "\n";
}

// ============================================================
// VARIASI 1: Observer Klasik (Interface + Virtual)
// ============================================================
namespace Klasik {

// Interface yang harus diimplementasi setiap observer
struct IObserver {
    virtual ~IObserver() = default;
    virtual void onEvent(const std::string& event, int nilai) = 0;
};

// Subject — menyimpan daftar observer dan mengirim notifikasi
class Subject {
    std::vector<IObserver*> observers_;
    std::string nama_;
public:
    explicit Subject(const std::string& nama) : nama_(nama) {}

    void subscribe(IObserver* obs) {
        observers_.push_back(obs);
    }

    void unsubscribe(IObserver* obs) {
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), obs),
            observers_.end());
    }

    // Broadcast ke semua observer
    void emit(const std::string& event, int nilai = 0) {
        std::cout << "  [" << nama_ << "] emit → \"" << event
                  << "\" (nilai=" << nilai << ")\n";
        for (auto* obs : observers_)
            obs->onEvent(event, nilai);
    }
};

// Concrete observers
struct HUD : IObserver {
    int skor = 0;
    void onEvent(const std::string& ev, int n) override {
        if (ev == "skor_tambah") {
            skor += n;
            std::cout << "    " << GRN << "[HUD]" << RST
                      << " Skor diperbarui: " << skor << "\n";
        }
    }
};

struct AudioSystem : IObserver {
    void onEvent(const std::string& ev, int) override {
        if (ev == "musuh_mati")
            std::cout << "    " << YLW << "[Audio]" << RST
                      << " 🔊 Putar 'die.wav'\n";
        else if (ev == "player_luka")
            std::cout << "    " << YLW << "[Audio]" << RST
                      << " 🔊 Putar 'ouch.wav'\n";
    }
};

struct AchievementSystem : IObserver {
    int musuhMati = 0;
    void onEvent(const std::string& ev, int) override {
        if (ev == "musuh_mati") {
            ++musuhMati;
            if (musuhMati == 5)
                std::cout << "    " << CYN << "[Achievement]" << RST
                          << " 🏆 UNLOCKED: \"5 Musuh Dibunuh!\"\n";
        }
    }
};

void demo() {
    judul("Variasi 1: Observer Klasik (Interface)");

    Subject subj("GameWorld");
    HUD hud;
    AudioSystem audio;
    AchievementSystem achiev;

    subj.subscribe(&hud);
    subj.subscribe(&audio);
    subj.subscribe(&achiev);

    subj.emit("skor_tambah", 100);
    subj.emit("musuh_mati");
    subj.emit("skor_tambah", 50);
    subj.emit("musuh_mati");
    subj.emit("player_luka");

    // Emit 3 lagi untuk trigger achievement
    for (int i = 0; i < 3; ++i) subj.emit("musuh_mati");

    // Unsubscribe audio lalu emit lagi
    std::cout << "\n  [unsubscribe Audio]\n";
    subj.unsubscribe(&audio);
    subj.emit("musuh_mati");
}

} // namespace Klasik

// ============================================================
// VARIASI 2: Event Bus — Callback Berbasis String Key
// ============================================================
namespace EventBus {

using Callback = std::function<void(const std::string& data)>;

class Bus {
    std::map<std::string, std::vector<Callback>> listeners_;
public:
    // Daftar listener
    void on(const std::string& event, Callback cb) {
        listeners_[event].push_back(std::move(cb));
    }

    // Kirim event dengan data string (bisa JSON, dsb)
    void emit(const std::string& event, const std::string& data = "") {
        std::cout << "  [Bus] emit \"" << event << "\""
                  << (data.empty() ? "" : " data=" + data) << "\n";
        auto it = listeners_.find(event);
        if (it == listeners_.end()) return;
        for (auto& cb : it->second) cb(data);
    }
};

void demo() {
    judul("Variasi 2: Event Bus (Callback + string key)");

    Bus bus;

    // Player mendaftar event
    bus.on("item_diambil", [](const std::string& item) {
        std::cout << "    " << GRN << "[Player]" << RST
                  << " Mengambil: " << item << "\n";
    });

    // Inventory mendaftar event
    bus.on("item_diambil", [](const std::string& item) {
        std::cout << "    " << YLW << "[Inventory]" << RST
                  << " Tambah '" << item << "' ke tas\n";
    });

    // Quest mendaftar event
    bus.on("item_diambil", [](const std::string& item) {
        if (item == "kunci_emas")
            std::cout << "    " << CYN << "[Quest]" << RST
                      << " ✓ Misi 'Temukan Kunci Emas' selesai!\n";
    });

    bus.on("level_naik", [](const std::string& lvl) {
        std::cout << "    " << GRN << "[Player]" << RST
                  << " LEVEL UP! → Level " << lvl << "\n";
        std::cout << "    " << BLU << "[UI]" << RST
                  << " Tampilkan animasi level up\n";
    });

    bus.on("game_over", [](const std::string&) {
        std::cout << "    " << "\033[31m[GameOver]\033[0m"
                  << " Simpan high score...\n";
        std::cout << "    " << "\033[31m[GameOver]\033[0m"
                  << " Tampilkan layar game over\n";
    });

    // Emit events
    bus.emit("item_diambil", "pedang_besi");
    bus.emit("item_diambil", "kunci_emas");
    bus.emit("level_naik", "5");
    bus.emit("game_over");
    bus.emit("event_tidak_ada", "data");  // tidak ada handler, aman
}

} // namespace EventBus

// ============================================================
// VARIASI 3: Signal<Args...> — Type-safe, Template-based
// ============================================================
namespace Signal {

// Signal yang bisa dihubungkan ke banyak fungsi dengan tanda tangan sama
template<typename... Args>
class Signal {
    using Slot = std::function<void(Args...)>;
    std::vector<std::pair<int, Slot>> slots_;
    int nextId_ = 0;
public:
    // Hubungkan slot, kembalikan ID untuk disconnect
    int connect(Slot slot) {
        int id = nextId_++;
        slots_.push_back({id, std::move(slot)});
        return id;
    }

    // Putus koneksi berdasarkan ID
    void disconnect(int id) {
        slots_.erase(
            std::remove_if(slots_.begin(), slots_.end(),
                [id](auto& p){ return p.first == id; }),
            slots_.end());
    }

    // Emit — panggil semua slot terhubung
    void emit(Args... args) {
        for (auto& [id, slot] : slots_)
            slot(args...);
    }

    size_t count() const { return slots_.size(); }
};

// Kelas yang memakai Signal
class Pemain {
public:
    Signal<int>         onHpBerubah;     // sinyal baru HP
    Signal<int>         onSkorBerubah;   // sinyal skor baru
    Signal<std::string> onMati;          // sinyal sebab kematian
    Signal<>            onJump;          // sinyal tanpa argumen

    int hp   = 100;
    int skor = 0;

    void ambilKerusakan(int dmg) {
        hp -= dmg;
        std::cout << "  [Pemain] HP " << (hp + dmg) << " → " << hp << "\n";
        onHpBerubah.emit(hp);
        if (hp <= 0) onMati.emit("kehabisan HP");
    }

    void tambahSkor(int poin) {
        skor += poin;
        onSkorBerubah.emit(skor);
    }

    void lompat() {
        std::cout << "  [Pemain] melompat!\n";
        onJump.emit();
    }
};

void demo() {
    judul("Variasi 3: Signal<Args...> — Type-safe Template");

    Pemain p;

    // Hubungkan slot ke signal
    p.onHpBerubah.connect([](int hp) {
        std::cout << "    " << GRN << "[HUD]" << RST
                  << " Health bar: " << hp << "/100\n";
    });

    int audioId = p.onHpBerubah.connect([](int hp) {
        if (hp <= 30)
            std::cout << "    " << YLW << "[Audio]" << RST
                      << " 🔊 Putar 'heartbeat.ogg' (HP kritis!)\n";
    });

    p.onSkorBerubah.connect([](int s) {
        std::cout << "    " << GRN << "[HUD]" << RST
                  << " Skor: " << s << "\n";
    });

    p.onMati.connect([](const std::string& sebab) {
        std::cout << "    " << "\033[31m[GameOver]\033[0m"
                  << " Pemain mati! Sebab: " << sebab << "\n";
    });

    p.onJump.connect([]() {
        std::cout << "    " << YLW << "[Audio]" << RST
                  << " 🔊 Putar 'jump.wav'\n";
    });

    // Simulasi
    p.tambahSkor(200);
    p.lompat();
    p.ambilKerusakan(50);
    p.ambilKerusakan(30);

    // Putus slot audio saat HP rendah untuk coba tanpa efek suara
    std::cout << "  [disconnect slot audio HP]\n";
    p.onHpBerubah.disconnect(audioId);

    p.ambilKerusakan(30);   // HP = -10, trigger onMati
}

} // namespace Signal

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n" << BOLD
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Observer Pattern — 3 Variasi                ║\n"
              << "║  Klasik | EventBus | Signal<T>               ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << RST;

    Klasik::demo();
    EventBus::demo();
    Signal::demo();

    std::cout << "\n" << BOLD
              << "─────────────────────────────────────────────\n"
              << " Ringkasan:\n"
              << "  Klasik   → interface sederhana, cocok untuk C++11\n"
              << "  EventBus → decoupled total, mudah extend\n"
              << "  Signal   → type-safe, compile-time check args\n"
              << "─────────────────────────────────────────────\n"
              << RST << "\n";
    return 0;
}
