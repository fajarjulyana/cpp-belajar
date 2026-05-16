// ============================================================
// File  : concurrency.cpp
// Topik : Concurrency Lanjut — Lock-Free, Memory Order, Parallel Algo
// Kompilasi: g++ -std=c++17 -pthread -O2 -Wall -o conc_demo concurrency.cpp
// ============================================================
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <map>
#include <optional>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

// ---- Lock-Free Stack ----
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* berikut;
        Node(T d) : data(d), berikut(nullptr) {}
    };
    atomic<Node*> kepala{nullptr};
    atomic<int>   ukuran{0};
public:
    void push(T data) {
        Node* baru = new Node(data);
        baru->berikut = kepala.load(memory_order_relaxed);
        while (!kepala.compare_exchange_weak(
            baru->berikut, baru, memory_order_release, memory_order_relaxed));
        ukuran.fetch_add(1, memory_order_relaxed);
    }
    optional<T> pop() {
        Node* tua = kepala.load(memory_order_acquire);
        while (tua && !kepala.compare_exchange_weak(
            tua, tua->berikut, memory_order_acquire, memory_order_relaxed));
        if (!tua) return nullopt;
        T val = tua->data;
        delete tua;
        ukuran.fetch_sub(1, memory_order_relaxed);
        return val;
    }
    int size() const { return ukuran.load(); }
    ~LockFreeStack() {
        while (pop()) {}
    }
};

// ---- RW Cache dengan shared_mutex ----
class RWCache {
    map<string, int> data;
    mutable shared_mutex mtx;
    mutable atomic<int> readCount{0};
    mutable atomic<int> writeCount{0};
public:
    optional<int> get(const string& k) const {
        shared_lock lock(mtx);
        readCount++;
        auto it = data.find(k);
        return it != data.end() ? optional<int>(it->second) : nullopt;
    }
    void set(const string& k, int v) {
        unique_lock lock(mtx);
        writeCount++;
        data[k] = v;
    }
    void stats() const {
        cout << "  Cache: " << data.size() << " entri, "
             << readCount << " reads, " << writeCount << " writes\n";
    }
};

// ---- Memory Order Demo ----
atomic<int>  counter_seq{0};
atomic<int>  counter_relaxed{0};

void benchSeq(int n)     { for(int i=0;i<n;i++) counter_seq.fetch_add(1, memory_order_seq_cst); }
void benchRelaxed(int n) { for(int i=0;i<n;i++) counter_relaxed.fetch_add(1, memory_order_relaxed); }

// ---- Acquire-Release Pattern ----
atomic<bool> dataSiap{false};
int sharedData = 0;

void produser_ar() {
    sharedData = 12345;
    dataSiap.store(true, memory_order_release);
}
void konsumer_ar(int& hasil) {
    while (!dataSiap.load(memory_order_acquire))
        this_thread::yield();
    hasil = sharedData;
}

// ---- Thread-Local Storage ----
thread_local int threadID = -1;
thread_local vector<int> riwayat;

mutex printMtx;
void workerTLS(int id, int tugas) {
    threadID = id;
    for (int i = 0; i < tugas; i++) {
        riwayat.push_back(i);
        this_thread::sleep_for(microseconds(10));
    }
    lock_guard<mutex> lock(printMtx);
    cout << "  Thread-" << threadID << " selesai, "
         << riwayat.size() << " item lokal\n";
}

// ---- Parallel Benchmark ----
long long hitungJumlahSingle(const vector<int>& v) {
    return accumulate(v.begin(), v.end(), 0LL);
}

long long hitungJumlahParalel(const vector<int>& v, int nThread) {
    int n = (int)v.size();
    int chunk = n / nThread;
    vector<long long> hasilThread(nThread, 0);
    vector<thread> threads;
    for (int i = 0; i < nThread; i++) {
        int awal = i * chunk;
        int akhir = (i == nThread-1) ? n : (i+1)*chunk;
        threads.emplace_back([&, i, awal, akhir]() {
            hasilThread[i] = accumulate(v.begin()+awal, v.begin()+akhir, 0LL);
        });
    }
    for (auto& t : threads) t.join();
    return accumulate(hasilThread.begin(), hasilThread.end(), 0LL);
}

int main() {
    // ---- Lock-Free Stack ----
    cout << "=== Lock-Free Stack ===" << endl;
    LockFreeStack<int> stack;
    const int N = 1000;
    vector<thread> pushers, poppers;

    for (int i = 0; i < 4; i++) {
        pushers.emplace_back([&, i]() {
            for (int j = 0; j < N/4; j++) stack.push(i*1000 + j);
        });
    }
    for (auto& t : pushers) t.join();
    cout << "  Setelah 4 thread push: size=" << stack.size() << "\n";

    atomic<int> popped{0};
    for (int i = 0; i < 4; i++) {
        poppers.emplace_back([&]() {
            while (auto v = stack.pop()) popped++;
        });
    }
    for (auto& t : poppers) t.join();
    cout << "  Setelah 4 thread pop: popped=" << popped << ", size=" << stack.size() << "\n";

    // ---- RW Cache ----
    cout << "\n=== shared_mutex RW Cache ===" << endl;
    RWCache cache;
    vector<thread> workers;
    for (int i = 0; i < 3; i++) {
        workers.emplace_back([&, i]() {
            cache.set("key" + to_string(i), i * 100);
        });
    }
    for (auto& t : workers) t.join();
    workers.clear();

    for (int i = 0; i < 8; i++) {
        workers.emplace_back([&, i]() {
            auto v = cache.get("key" + to_string(i % 3));
            (void)v;
        });
    }
    for (auto& t : workers) t.join();
    cache.stats();

    // ---- Memory Order Benchmark ----
    cout << "\n=== Memory Order Benchmark ===" << endl;
    const int M = 100000;
    {
        counter_seq = 0;
        auto t1 = high_resolution_clock::now();
        vector<thread> ts; for(int i=0;i<4;i++) ts.emplace_back(benchSeq, M/4);
        for(auto& t:ts) t.join();
        auto t2 = high_resolution_clock::now();
        cout << "  seq_cst  : " << counter_seq << " (" << duration<double,milli>(t2-t1).count() << "ms)\n";
    }
    {
        counter_relaxed = 0;
        auto t1 = high_resolution_clock::now();
        vector<thread> ts; for(int i=0;i<4;i++) ts.emplace_back(benchRelaxed, M/4);
        for(auto& t:ts) t.join();
        auto t2 = high_resolution_clock::now();
        cout << "  relaxed  : " << counter_relaxed << " (" << duration<double,milli>(t2-t1).count() << "ms)\n";
    }

    // ---- Acquire-Release ----
    cout << "\n=== Acquire-Release Pattern ===" << endl;
    dataSiap = false; sharedData = 0;
    int hasil = 0;
    thread tp(produser_ar), tc(konsumer_ar, ref(hasil));
    tp.join(); tc.join();
    cout << "  Data diterima: " << hasil << "\n";

    // ---- Thread-Local Storage ----
    cout << "\n=== Thread-Local Storage ===" << endl;
    vector<thread> tlsWorkers;
    for (int i = 0; i < 4; i++) tlsWorkers.emplace_back(workerTLS, i, 5);
    for (auto& t : tlsWorkers) t.join();

    // ---- Parallel Compute Benchmark ----
    cout << "\n=== Parallel Sum Benchmark ===" << endl;
    vector<int> bigData(4000000);
    iota(bigData.begin(), bigData.end(), 1);

    auto t1 = high_resolution_clock::now();
    long long s1 = hitungJumlahSingle(bigData);
    auto t2 = high_resolution_clock::now();
    double msSingle = duration<double,milli>(t2-t1).count();

    auto t3 = high_resolution_clock::now();
    long long s2 = hitungJumlahParalel(bigData, 4);
    auto t4 = high_resolution_clock::now();
    double msParalel = duration<double,milli>(t4-t3).count();

    cout << "  Single (1 thread): " << s1 << " (" << msSingle << " ms)\n";
    cout << "  Paralel (4 thread): " << s2 << " (" << msParalel << " ms)\n";
    cout << "  Hasil sama: " << boolalpha << (s1==s2) << "\n";
    if (msParalel > 0)
        cout << "  Speedup: " << msSingle/msParalel << "x\n";

    return 0;
}
