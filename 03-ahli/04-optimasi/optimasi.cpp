// ============================================================
// File  : optimasi.cpp
// Topik : Optimasi Performa C++
// Kompilasi: g++ -std=c++17 -O2 -Wall -o optim_demo optimasi.cpp
// ============================================================
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>
#include <string>
#include <memory>
using namespace std;
using namespace chrono;

// ---- Timer helper ----
struct Timer {
    string nama;
    high_resolution_clock::time_point awal;
    Timer(const string& n) : nama(n), awal(high_resolution_clock::now()) {}
    ~Timer() {
        auto akhir = high_resolution_clock::now();
        double ms = duration<double, milli>(akhir - awal).count();
        cout << "  [" << nama << "] " << ms << " ms\n";
    }
};

// ---- Cache Locality Demo ----
const int N = 1000;
int matrix[N][N];

long long aksesBuruk() {  // kolom demi kolom (cache miss)
    long long sum = 0;
    for (int j = 0; j < N; j++)
        for (int i = 0; i < N; i++)
            sum += matrix[i][j];
    return sum;
}

long long aksesBagus() {  // baris demi baris (cache hit)
    long long sum = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sum += matrix[i][j];
    return sum;
}

// ---- Branch Prediction Demo ----
long long branchAcak(const vector<int>& data) {
    long long sum = 0;
    for (int x : data) if (x > 500) sum += x;
    return sum;
}

long long branchSorted(const vector<int>& data) {
    long long sum = 0;
    for (int x : data) if (x > 500) sum += x;
    return sum;
}

// ---- Pool Allocator ----
class PoolAllocator {
    vector<uint8_t> pool;
    size_t offset = 0;
public:
    PoolAllocator(size_t sz) : pool(sz) {}
    void* alokasi(size_t sz) {
        sz = (sz + 7) & ~7;  // align ke 8
        if (offset + sz > pool.size()) return nullptr;
        void* ptr = pool.data() + offset;
        offset += sz;
        return ptr;
    }
    void reset() { offset = 0; }
    size_t terpakai() const { return offset; }
};

// ---- Move Semantics Demo ----
class Buffer {
    vector<uint8_t> data;
public:
    Buffer(size_t sz) : data(sz, 0) {}
    Buffer(const Buffer& b) : data(b.data)       { /* copy */ }
    Buffer(Buffer&& b) noexcept : data(move(b.data)) { /* move — gratis */ }
    size_t ukuran() const { return data.size(); }
};

vector<Buffer> buatBuffers(int n, size_t sz) {
    vector<Buffer> v;
    v.reserve(n);
    for (int i = 0; i < n; i++) v.emplace_back(sz);
    return v;  // NRVO
}

// ---- AoS vs SoA ----
struct PartikelAoS { float x, y, z, mass; };
struct PartikelSoA { vector<float> x, y, z, mass; };

double hitungEnergiAoS(const vector<PartikelAoS>& p) {
    double e = 0;
    for (const auto& part : p) e += 0.5 * part.mass * (part.x*part.x + part.y*part.y + part.z*part.z);
    return e;
}

double hitungEnergiSoA(const PartikelSoA& p) {
    double e = 0;
    int n = (int)p.x.size();
    for (int i = 0; i < n; i++) e += 0.5 * p.mass[i] * (p.x[i]*p.x[i] + p.y[i]*p.y[i] + p.z[i]*p.z[i]);
    return e;
}

// ---- String Optimization ----
string buatStringBuruk(int n) {
    string hasil = "";
    for (int i = 0; i < n; i++) hasil += "x";  // banyak reallokasi
    return hasil;
}

string buatStringBagus(int n) {
    string hasil;
    hasil.reserve(n);  // alokasi sekali
    for (int i = 0; i < n; i++) hasil += 'x';
    return hasil;
}

int main() {
    // ---- Init matrix ----
    for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) matrix[i][j] = i+j;

    // ---- Cache Locality ----
    cout << "=== Cache Locality ===" << endl;
    long long s1, s2;
    { Timer t("Column-major (buruk)");  s1 = aksesBuruk(); }
    { Timer t("Row-major    (bagus)");  s2 = aksesBagus(); }
    cout << "  Hasil sama: " << boolalpha << (s1==s2) << "\n";

    // ---- Branch Prediction ----
    cout << "\n=== Branch Prediction ===" << endl;
    const int M = 1000000;
    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 999);
    vector<int> acak(M), terurut(M);
    for (int& x : acak)   x = dist(rng);
    terurut = acak;
    sort(terurut.begin(), terurut.end());

    long long ba, bs;
    { Timer t("Data acak  "); ba = branchAcak(acak); }
    { Timer t("Data sorted"); bs = branchSorted(terurut); }
    cout << "  Jumlah acak=" << ba << " sorted=" << bs << "\n";

    // ---- Memory Allocation ----
    cout << "\n=== Memory Allocation ===" << endl;
    const int NOBJ = 100000;
    {
        Timer t("new/delete (banyak)");
        vector<int*> ptrs(NOBJ);
        for (int i = 0; i < NOBJ; i++) ptrs[i] = new int(i);
        for (int i = 0; i < NOBJ; i++) delete ptrs[i];
    }
    {
        Timer t("Pool Allocator      ");
        PoolAllocator pool(NOBJ * sizeof(int) * 2);
        for (int i = 0; i < NOBJ; i++) {
            int* p = (int*)pool.alokasi(sizeof(int));
            if (p) *p = i;
        }
        pool.reset();
    }

    // ---- Move Semantics ----
    cout << "\n=== Move Semantics ===" << endl;
    {
        Timer t("Buat 100 buffer (move)");
        auto bufs = buatBuffers(100, 1024*1024);
        cout << "  Dibuat: " << bufs.size() << " buffer, ukuran: " << bufs[0].ukuran() << " byte\n";
    }

    // ---- AoS vs SoA ----
    cout << "\n=== AoS vs SoA ===" << endl;
    const int NP = 100000;
    vector<PartikelAoS> aos(NP);
    PartikelSoA soa; soa.x.resize(NP); soa.y.resize(NP); soa.z.resize(NP); soa.mass.resize(NP);
    for (int i = 0; i < NP; i++) {
        float x=i*0.001f, y=i*0.002f, z=i*0.003f, m=1.0f+i*0.0001f;
        aos[i] = {x,y,z,m};
        soa.x[i]=x; soa.y[i]=y; soa.z[i]=z; soa.mass[i]=m;
    }
    double e1, e2;
    { Timer t("AoS energi"); e1 = hitungEnergiAoS(aos); }
    { Timer t("SoA energi"); e2 = hitungEnergiSoA(soa); }
    cout << "  Hasil sama: " << (fabs(e1-e2) < 1.0) << "\n";

    // ---- String Optimization ----
    cout << "\n=== String Optimization ===" << endl;
    const int SLEN = 100000;
    { Timer t("Concat tanpa reserve"); buatStringBuruk(SLEN); }
    { Timer t("Concat dengan reserve"); buatStringBagus(SLEN); }

    // ---- emplace_back vs push_back ----
    cout << "\n=== emplace_back vs push_back ===" << endl;
    const int NV = 500000;
    {
        Timer t("push_back pair ");
        vector<pair<int,string>> v;
        v.reserve(NV);
        for (int i = 0; i < NV; i++) v.push_back({i, to_string(i)});
    }
    {
        Timer t("emplace_back pair");
        vector<pair<int,string>> v;
        v.reserve(NV);
        for (int i = 0; i < NV; i++) v.emplace_back(i, to_string(i));
    }

    return 0;
}
