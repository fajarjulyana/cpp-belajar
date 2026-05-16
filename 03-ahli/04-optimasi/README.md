# Bab 22 — Optimasi Performa

Menulis kode C++ yang cepat membutuhkan pemahaman tentang hardware, compiler, dan algoritma.

---

## Tools Profiling

```bash
# Compile dengan debug info untuk profiling
g++ -std=c++17 -O2 -pg -o program program.cpp
./program
gprof program gmon.out > laporan.txt

# Valgrind Cachegrind (cache analysis)
valgrind --tool=cachegrind ./program

# perf (Linux)
perf stat ./program
perf record ./program && perf report

# AddressSanitizer (deteksi memory error)
g++ -fsanitize=address -O1 -g -o program program.cpp
```

---

## Compiler Optimizations

```bash
# Level optimasi
g++ -O0   # tanpa optimasi (debug)
g++ -O1   # optimasi dasar
g++ -O2   # optimasi standar (production)
g++ -O3   # agresif (mungkin break kode UB)
g++ -Os   # optimasi ukuran
g++ -Ofast # O3 + unsafe math

# Fitur tambahan
g++ -O2 -march=native    # instruksi CPU lokal (AVX, SSE4, dll)
g++ -O2 -flto            # Link-Time Optimization
g++ -O2 -fprofile-use    # Profile-Guided Optimization
```

---

## Cache Locality — Akses Memori

```cpp
// BURUK: row-major access pada 2D array (kolom demi kolom)
for (int j = 0; j < N; j++)
    for (int i = 0; i < N; i++)
        sum += matrix[i][j];  // cache miss setiap iterasi!

// BAGUS: row-major access (baris demi baris)
for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
        sum += matrix[i][j];  // cache hit hampir selalu!

// AoS vs SoA
// Array of Structures (buruk untuk SIMD)
struct Partikel { float x, y, z, mass; };
vector<Partikel> partikel(N);

// Structure of Arrays (lebih baik untuk SIMD/vektorisasi)
struct PartikelSoA {
    vector<float> x, y, z, mass;
};

// SoA lebih baik jika memproses satu field sekaligus (misal: update semua x)
for (int i = 0; i < N; i++) partikel.x[i] += partikel.vx[i] * dt;
```

---

## Branch Prediction

```cpp
#include <algorithm>

// BURUK: branch sulit diprediksi (data acak)
int sum = 0;
for (int x : data) if (x > 128) sum += x;

// BAGUS: sort dulu agar branch predictable
sort(data.begin(), data.end());
for (int x : data) if (x > 128) sum += x;

// Branchless alternatives
// Cara 1: conditional dengan multiply
int sum2 = 0;
for (int x : data) sum2 += (x > 128) * x;

// Cara 2: __builtin_expect
for (int x : data)
    if (__builtin_expect(x > 128, 1)) sum += x;  // hint: likely true
```

---

## Inlining & Templates

```cpp
// inline manual
inline int max2(int a, int b) { return a > b ? a : b; }

// Template sudah inline secara default
template<typename T>
T tambah(T a, T b) { return a + b; }

// __attribute__((always_inline)) — paksa inline
__attribute__((always_inline)) static int cepat(int x) { return x * x; }

// NOINLINE — kadang berguna untuk profiling
__attribute__((noinline)) int lambat(int x) { return x * x; }
```

---

## SIMD Intrinsics (AVX/SSE)

```cpp
#include <immintrin.h>  // AVX

// Tambah 8 float sekaligus (AVX)
void tambahAVX(float* a, const float* b, int n) {
    int i = 0;
    for (; i <= n - 8; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(a + i, vc);
    }
    for (; i < n; i++) a[i] += b[i];  // sisa
}
```

---

## Alokasi Memori

```cpp
// BURUK: banyak alokasi kecil
vector<int*> ptrs;
for (int i = 0; i < 10000; i++) ptrs.push_back(new int(i));
// banyak malloc overhead!

// BAGUS: arena/pool allocator
class PoolAllocator {
    vector<byte> pool;
    size_t offset = 0;
public:
    PoolAllocator(size_t ukuran) : pool(ukuran) {}
    void* alokasi(size_t ukuran) {
        void* ptr = pool.data() + offset;
        offset += ukuran;
        return ptr;
    }
    void reset() { offset = 0; }  // gratis semua sekaligus
};

// reserve untuk menghindari reallokasi
vector<int> v;
v.reserve(10000);  // alokasi sekali di awal
for (int i = 0; i < 10000; i++) v.push_back(i);
```

---

## Move Semantics

```cpp
// Hindari salinan tidak perlu
vector<string> buat() {
    vector<string> result;
    result.reserve(1000);
    for (int i = 0; i < 1000; i++) result.push_back("item_" + to_string(i));
    return result;  // NRVO atau move, bukan copy
}

auto v = buat();  // move, bukan copy

// emplace_back vs push_back
vector<pair<int,string>> data;
data.push_back({1, "satu"});     // buat temp, lalu copy/move
data.emplace_back(2, "dua");     // buat langsung di tempat (lebih efisien)
```

---

## Latihan

1. Benchmark akses row-major vs column-major pada matrix 1000x1000
2. Implementasikan pool allocator dan bandingkan dengan `new/delete`
3. Profil program dengan gprof dan identifikasi bottleneck
4. Implementasikan dot product SIMD-friendly dan bandingkan dengan versi scalar

---

**[← Concurrency](../03-concurrency/README.md)** | **[→ GUI Framework](../../04-gui/01-gtkmm/README.md)**
