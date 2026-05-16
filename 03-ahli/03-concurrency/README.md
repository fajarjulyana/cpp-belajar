# Bab 21 — Concurrency Lanjut

Pembahasan teknik-teknik sinkronisasi tingkat lanjut: lock-free programming, memory model, dan parallel algorithms.

---

## Memory Order & Memory Model

C++ memory model mendefinisikan bagaimana operasi pada memori terlihat antar thread.

```cpp
#include <atomic>
#include <thread>

atomic<int> x = 0, y = 0;
int r1, r2;

// Sequential Consistency (default, paling kuat)
void t1() { x.store(1, memory_order_seq_cst); r1 = y.load(memory_order_seq_cst); }
void t2() { y.store(1, memory_order_seq_cst); r2 = x.load(memory_order_seq_cst); }
// Jaminan: tidak mungkin r1==0 && r2==0

// Acquire-Release (lebih cepat)
atomic<bool> siap = false;
int data = 0;
void produser() { data = 42; siap.store(true, memory_order_release); }
void konsumer() {
    while (!siap.load(memory_order_acquire)); // spin
    cout << data << "\n"; // pasti 42
}

// Relaxed (paling cepat, hanya atomicity)
atomic<int> counter = 0;
void increment() { counter.fetch_add(1, memory_order_relaxed); }
// Cocok untuk hitungan sederhana tanpa urutan/synchronization
```

---

## Lock-Free Data Structures

### Lock-Free Stack

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        atomic<Node*> berikut;
        Node(T d) : data(d), berikut(nullptr) {}
    };
    atomic<Node*> kepala{nullptr};

public:
    void push(T data) {
        Node* baru = new Node(data);
        baru->berikut = kepala.load();
        while (!kepala.compare_exchange_weak(baru->berikut, baru));
    }

    optional<T> pop() {
        Node* tua = kepala.load();
        while (tua && !kepala.compare_exchange_weak(tua, tua->berikut.load()));
        if (!tua) return nullopt;
        T data = tua->data;
        delete tua;
        return data;
    }
};
```

---

## `compare_exchange` — CAS (Compare-And-Swap)

```cpp
atomic<int> nilai = 0;

// compare_exchange_strong: atomis ubah jika == expected
int expected = 0;
int desired  = 42;
bool berhasil = nilai.compare_exchange_strong(expected, desired);
// Jika nilai == 0 (expected): ubah ke 42, return true
// Jika tidak: expected = nilai sekarang, return false

// Pola ABA problem:
// Thread 1: read A → do work → CAS(A, B) → berhasil (tapi state mungkin berubah!)
// Solusi: Tagged Pointer / Version Counter
struct TaggedPtr {
    Node*   ptr;
    int64_t tag;   // increment setiap push/pop
};
atomic<TaggedPtr> kepala;
```

---

## `std::shared_mutex` — Multiple Readers / One Writer

```cpp
#include <shared_mutex>

class Cache {
    map<string, string> data;
    mutable shared_mutex mtx;

public:
    // Banyak thread bisa baca bersamaan
    optional<string> get(const string& k) const {
        shared_lock<shared_mutex> lock(mtx);  // read lock
        auto it = data.find(k);
        return it != data.end() ? optional<string>(it->second) : nullopt;
    }

    // Hanya satu thread yang bisa tulis
    void set(const string& k, const string& v) {
        unique_lock<shared_mutex> lock(mtx);  // write lock
        data[k] = v;
    }
};
```

---

## Parallel Algorithms (C++17)

```cpp
#include <algorithm>
#include <execution>
#include <numeric>

vector<int> v(1000000);
iota(v.begin(), v.end(), 1);

// Sort paralel
sort(execution::par, v.begin(), v.end());

// Transform paralel
transform(execution::par_unseq, v.begin(), v.end(), v.begin(),
          [](int x) { return x * 2; });

// Reduce paralel
auto total = reduce(execution::par, v.begin(), v.end(), 0LL);

// Execution policies:
// execution::seq       — sequential (default)
// execution::par       — parallel
// execution::par_unseq — parallel + SIMD vectorized
```

---

## `std::latch` & `std::barrier` (C++20)

```cpp
#include <latch>
#include <barrier>

// latch: countdown, satu kali pakai
latch siap(4);  // tunggu 4 thread
thread workers[4];
for (int i = 0; i < 4; i++) {
    workers[i] = thread([&, i]() {
        // ... inisialisasi ...
        siap.count_down();  // kurangi counter
    });
}
siap.wait();  // blokir sampai counter = 0
for (auto& t : workers) t.join();

// barrier: reusable synchronization point
barrier sync(4);
for (int i = 0; i < 4; i++) {
    workers[i] = thread([&, i]() {
        for (int fase = 0; fase < 3; fase++) {
            // ... kerjakan fase ini ...
            sync.arrive_and_wait();  // tunggu semua thread sampai di sini
        }
    });
}
```

---

## Thread-Local Storage

```cpp
// Setiap thread punya salinan sendiri
thread_local int threadID = 0;
thread_local vector<int> riwayatLokal;

void worker(int id) {
    threadID = id;           // tidak konflik antar thread
    riwayatLokal.push_back(id);
    cout << "Thread " << threadID << ": " << riwayatLokal.size() << " item\n";
}
```

---

## Latihan

1. Implementasikan lock-free queue menggunakan atomic dan CAS
2. Buat shared cache dengan `shared_mutex` dan benchmark vs `mutex` biasa
3. Gunakan parallel algorithms untuk menghitung statistik array besar
4. Implementasikan pipeline paralel: stage1 → stage2 → stage3 dengan channel antar stage

---

**[← Metaprogramming](../02-metaprogramming/README.md)** | **[Berikutnya → Optimasi](../04-optimasi/README.md)**
