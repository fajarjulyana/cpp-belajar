// ============================================================
// File  : object_pool.cpp
// Topik : Object Pool Pattern — Reuse Objek, Hindari new/delete
//
// Tiga implementasi:
//   1. Pool Sederhana (fixed-size array)
//   2. Pool Generic (template)
//   3. Pool dengan RAII guard (auto-release)
//   Plus: benchmark new/delete vs pool
//
// Kompilasi:
//   g++ -std=c++17 -O2 -o object_pool object_pool.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <functional>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <array>

const char* RST  = "\033[0m";
const char* BOLD = "\033[1m";
const char* GRN  = "\033[32m";
const char* YLW  = "\033[33m";
const char* RED  = "\033[31m";
const char* BLU  = "\033[34m";
const char* CYN  = "\033[36m";

void judul(const std::string& s) {
    std::cout << "\n" << BOLD << BLU << "══ " << s << " ══" << RST << "\n";
}

// ============================================================
// Objek yang sering dibuat/dihapus: Peluru (Particle)
// ============================================================
struct Peluru {
    float x = 0, y = 0;
    float vx = 0, vy = 0;
    int   dmg    = 10;
    float ttl    = 2.0f;  // time-to-live (detik)
    bool  aktif  = false;

    void init(float px, float py, float pvx, float pvy, int pdmg) {
        x = px; y = py; vx = pvx; vy = pvy;
        dmg = pdmg; ttl = 2.0f; aktif = true;
    }

    void reset() {
        aktif = false;
        x = y = vx = vy = 0;
        dmg = 0; ttl = 0;
    }

    void update(float dt) {
        if (!aktif) return;
        x += vx * dt;
        y += vy * dt;
        ttl -= dt;
        if (ttl <= 0) aktif = false;
    }
};

// ============================================================
// IMPLEMENTASI 1: Object Pool Sederhana (fixed array)
// ============================================================
class PeluruPool {
    static constexpr size_t KAPASITAS = 64;
    std::array<Peluru, KAPASITAS> pool_;
    std::stack<Peluru*>           bebas_;  // pointer ke objek yang tidak dipakai

public:
    PeluruPool() {
        for (auto& p : pool_)
            bebas_.push(&p);
    }

    // Ambil peluru dari pool (tidak ada alokasi heap!)
    Peluru* acquire() {
        if (bebas_.empty()) {
            std::cout << "  " << RED << "[Pool] PENUH! Tidak bisa acquire\n" << RST;
            return nullptr;
        }
        Peluru* p = bebas_.top();
        bebas_.pop();
        return p;
    }

    // Kembalikan peluru ke pool (tidak ada deallokasi!)
    void release(Peluru* p) {
        if (!p) return;
        p->reset();
        bebas_.push(p);
    }

    size_t tersedia()   const { return bebas_.size(); }
    size_t terpakai()   const { return KAPASITAS - bebas_.size(); }
    size_t kapasitas()  const { return KAPASITAS; }

    void status() const {
        std::cout << "  Pool: " << terpakai() << "/" << kapasitas()
                  << " dipakai, " << tersedia() << " tersedia\n";
    }
};

void demoPoolSederhana() {
    judul("Demo 1: Object Pool Sederhana (Fixed Array)");

    PeluruPool pool;
    pool.status();

    // Tembakkan beberapa peluru
    std::vector<Peluru*> aktif;
    for (int i = 0; i < 8; ++i) {
        Peluru* p = pool.acquire();
        if (p) {
            p->init(0, 0, (float)i, (float)(i % 3), 10 + i * 5);
            aktif.push_back(p);
        }
    }
    std::cout << "  Setelah acquire 8 peluru:\n";
    pool.status();

    // Simulasi: beberapa peluru kembali
    std::cout << "  Simulasi update (beberapa peluru mati)...\n";
    for (auto* p : aktif) {
        p->update(3.0f);  // TTL=2s, semua mati setelah dt=3s
    }

    // Kembalikan yang sudah tidak aktif
    int dikembalikan = 0;
    for (auto* p : aktif) {
        if (!p->aktif) {
            pool.release(p);
            ++dikembalikan;
        }
    }
    aktif.erase(std::remove_if(aktif.begin(), aktif.end(),
        [](Peluru* p){ return !p->aktif; }), aktif.end());

    std::cout << "  " << dikembalikan << " peluru dikembalikan ke pool\n";
    pool.status();
}

// ============================================================
// IMPLEMENTASI 2: Generic Object Pool (Template)
// ============================================================
template<typename T, size_t N = 32>
class ObjectPool {
    std::array<T, N>    storage_;
    std::array<bool, N> used_;

public:
    ObjectPool() { used_.fill(false); }

    template<typename... Args>
    T* acquire(Args&&... args) {
        for (size_t i = 0; i < N; ++i) {
            if (!used_[i]) {
                used_[i] = true;
                new (&storage_[i]) T(std::forward<Args>(args)...);
                return &storage_[i];
            }
        }
        return nullptr;  // pool penuh
    }

    void release(T* ptr) {
        for (size_t i = 0; i < N; ++i) {
            if (&storage_[i] == ptr) {
                ptr->~T();
                new (ptr) T{};
                used_[i] = false;
                return;
            }
        }
    }

    size_t terpakai() const {
        size_t n = 0;
        for (bool b : used_) if (b) ++n;
        return n;
    }
    size_t tersedia() const { return N - terpakai(); }
    size_t kapasitas() const { return N; }
};

// Objek berbeda untuk demo generic pool
struct Partikel {
    float x, y, vx, vy;
    float lifetime;
    int   r, g, b;  // warna

    Partikel() : x(0), y(0), vx(0), vy(0), lifetime(1.0f), r(255), g(255), b(255) {}
    Partikel(float px, float py, float pvx, float pvy,
             float lt, int pr, int pg, int pb)
        : x(px), y(py), vx(pvx), vy(pvy),
          lifetime(lt), r(pr), g(pg), b(pb) {}
};

void demoPoolGeneric() {
    judul("Demo 2: Generic Object Pool (Template)");

    ObjectPool<Partikel, 16> pool;

    std::cout << "  Pool<Partikel, 16> dibuat\n";
    std::cout << "  Kapasitas: " << pool.kapasitas() << " partikel\n\n";

    // Efek ledakan: spawn banyak partikel
    std::cout << "  Efek ledakan: spawn 10 partikel...\n";
    std::vector<Partikel*> partikel;
    for (int i = 0; i < 10; ++i) {
        float sudut = (float)i / 10.f * 6.2831f;
        Partikel* p = pool.acquire(
            5.f, 5.f,
            std::cos(sudut) * 3.f, std::sin(sudut) * 3.f,
            0.5f + (float)(i % 4) * 0.3f,
            255, 128 + i * 10, i * 20
        );
        if (p) partikel.push_back(p);
    }
    std::cout << "  Terpakai: " << pool.terpakai()
              << "/" << pool.kapasitas() << "\n";

    // Efek asap: spawn 6 lagi
    std::cout << "  Efek asap: spawn 6 partikel lagi...\n";
    for (int i = 0; i < 6; ++i) {
        Partikel* p = pool.acquire(
            3.f, 8.f, 0.f, -1.f, 1.2f, 180, 180, 180
        );
        if (p) partikel.push_back(p);
        else std::cout << "  " << RED << "Pool penuh!\n" << RST;
    }
    std::cout << "  Terpakai: " << pool.terpakai()
              << "/" << pool.kapasitas() << "\n";

    // Kembalikan setengah partikel
    std::cout << "  Kembalikan 8 partikel...\n";
    for (int i = 0; i < 8 && i < (int)partikel.size(); ++i)
        pool.release(partikel[i]);
    std::cout << "  Terpakai: " << pool.terpakai()
              << "/" << pool.kapasitas() << "\n";
}

// ============================================================
// IMPLEMENTASI 3: Pool dengan RAII Guard (auto-release)
// ============================================================
template<typename T>
class PoolHandle {
    T*                   ptr_   = nullptr;
    std::function<void(T*)> release_;

public:
    PoolHandle() = default;
    PoolHandle(T* ptr, std::function<void(T*)> rel)
        : ptr_(ptr), release_(std::move(rel)) {}

    // Tidak bisa disalin, hanya dipindah
    PoolHandle(const PoolHandle&) = delete;
    PoolHandle& operator=(const PoolHandle&) = delete;

    PoolHandle(PoolHandle&& o) noexcept
        : ptr_(o.ptr_), release_(std::move(o.release_)) {
        o.ptr_ = nullptr;
    }

    PoolHandle& operator=(PoolHandle&& o) noexcept {
        if (this != &o) {
            if (ptr_) release_(ptr_);
            ptr_ = o.ptr_; release_ = std::move(o.release_);
            o.ptr_ = nullptr;
        }
        return *this;
    }

    // Auto-release saat handle keluar scope
    ~PoolHandle() { if (ptr_) release_(ptr_); }

    T* get() { return ptr_; }
    T* operator->() { return ptr_; }
    T& operator*() { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
};

class PeluruPoolRAII {
    static constexpr size_t N = 32;
    std::array<Peluru, N> storage_;
    std::stack<size_t>    freeIdx_;

public:
    PeluruPoolRAII() {
        for (size_t i = 0; i < N; ++i) freeIdx_.push(i);
    }

    PoolHandle<Peluru> acquire(float px, float py, float pvx, float pvy, int dmg) {
        if (freeIdx_.empty()) return {};
        size_t idx = freeIdx_.top(); freeIdx_.pop();
        storage_[idx].init(px, py, pvx, pvy, dmg);

        return PoolHandle<Peluru>(&storage_[idx],
            [this, idx](Peluru* p) {
                p->reset();
                freeIdx_.push(idx);
            });
    }

    size_t tersedia() const { return freeIdx_.size(); }
};

void demoPoolRAII() {
    judul("Demo 3: Pool dengan RAII Handle (auto-release)");

    PeluruPoolRAII pool;
    std::cout << "  Pool RAII dibuat, tersedia: " << pool.tersedia() << "\n\n";

    {
        std::cout << "  Masuk scope...\n";
        auto h1 = pool.acquire(0, 0, 5, 3, 20);
        auto h2 = pool.acquire(1, 0, -5, 3, 15);
        auto h3 = pool.acquire(2, 0, 0, 7, 30);

        if (h1) std::cout << "  Peluru1: (" << h1->x << "," << h1->y
                          << ") dmg=" << h1->dmg << "\n";
        if (h2) std::cout << "  Peluru2: (" << h2->x << "," << h2->y
                          << ") dmg=" << h2->dmg << "\n";

        std::cout << "  Tersedia: " << pool.tersedia() << " (3 dipakai)\n";
        std::cout << "  Keluar scope...\n";
    }  // h1, h2, h3 auto-release di sini!

    std::cout << "  Tersedia setelah keluar scope: " << pool.tersedia()
              << " (semua dikembalikan otomatis!)\n";
}

// ============================================================
// BENCHMARK: new/delete vs Object Pool
// ============================================================
void benchmark() {
    judul("Benchmark: new/delete vs Object Pool");

    const int ITERASI   = 100'000;
    const int SEKALIGUS = 10;

    // ── Benchmark new/delete ──
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERASI; ++i) {
        std::vector<Peluru*> v;
        for (int j = 0; j < SEKALIGUS; ++j)
            v.push_back(new Peluru());
        for (auto* p : v) delete p;
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    auto msNewDelete = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();

    // ── Benchmark Object Pool ──
    ObjectPool<Peluru, 64> pool;
    auto t2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERASI; ++i) {
        std::vector<Peluru*> v;
        for (int j = 0; j < SEKALIGUS; ++j)
            v.push_back(pool.acquire());
        for (auto* p : v) pool.release(p);
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    auto msPool = std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count();

    std::cout << "  Iterasi    : " << ITERASI
              << " × " << SEKALIGUS << " objek\n\n";
    std::cout << "  new/delete : " << std::setw(6) << msNewDelete << " ms\n";
    std::cout << "  Pool       : " << std::setw(6) << msPool << " ms\n";

    if (msPool > 0) {
        double speedup = (double)msNewDelete / msPool;
        std::cout << "\n  " << GRN << BOLD << "Speedup: "
                  << std::fixed << std::setprecision(1) << speedup
                  << "x lebih cepat!" << RST << "\n";
    }
    std::cout << "\n  (Pool lebih cepat karena tidak ada alokasi heap,\n"
              << "   tidak ada pencarian blok bebas, tidak ada fragmentasi)\n";
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n" << BOLD
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Object Pool Pattern                         ║\n"
              << "║  Sederhana | Generic | RAII | Benchmark      ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << RST;

    demoPoolSederhana();
    demoPoolGeneric();
    demoPoolRAII();
    benchmark();

    std::cout << "\n" << BOLD
              << "─────────────────────────────────────────────\n"
              << " Ringkasan:\n"
              << "  Pool Sederhana → fixed array + stack indeks\n"
              << "  Generic Pool   → template, construct-in-place\n"
              << "  RAII Handle    → auto-release via destructor\n"
              << "  Benchmark      → lihat angka speedup di atas!\n"
              << "─────────────────────────────────────────────\n"
              << RST << "\n";
    return 0;
}
