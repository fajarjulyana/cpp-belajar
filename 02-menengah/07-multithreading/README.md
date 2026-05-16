# Bab 17 — Multithreading

Multithreading memungkinkan eksekusi beberapa tugas **secara bersamaan** memanfaatkan multiple CPU cores.

---

## `std::thread` — Thread Dasar

```cpp
#include <thread>
#include <iostream>
using namespace std;

void tugas(int id, int iterasi) {
    for (int i = 0; i < iterasi; i++) {
        cout << "Thread-" << id << ": iterasi " << i << "\n";
    }
}

int main() {
    // Buat 3 thread
    thread t1(tugas, 1, 3);
    thread t2(tugas, 2, 3);
    thread t3(tugas, 3, 3);

    t1.join();  // tunggu t1 selesai
    t2.join();  // tunggu t2 selesai
    t3.join();  // tunggu t3 selesai
    return 0;
}
```

### Cara Membuat Thread
```cpp
// 1. Dengan fungsi biasa
thread t1(fungsi, arg1, arg2);

// 2. Dengan lambda
thread t2([](int x) { cout << x << "\n"; }, 42);

// 3. Dengan method class
struct Pekerja {
    void jalankan(int n) { cout << "Pekerja: " << n << "\n"; }
};
Pekerja p;
thread t3(&Pekerja::jalankan, &p, 10);

// 4. Detach (berjalan mandiri, tidak perlu join)
thread t4([]{ cout << "Detached!\n"; });
t4.detach();  // jangan akses t4 setelah ini!
```

---

## Mutex — Mencegah Race Condition

```cpp
#include <mutex>
#include <thread>

int counter = 0;
mutex mtx;

void increment(int n) {
    for (int i = 0; i < n; i++) {
        lock_guard<mutex> lock(mtx);  // auto-unlock saat keluar scope
        ++counter;
    }
}

// unique_lock — lebih fleksibel dari lock_guard
void operasi() {
    unique_lock<mutex> lock(mtx);
    // ... lakukan sesuatu ...
    lock.unlock();   // bisa unlock manual
    // ... kode yang tidak butuh lock ...
    lock.lock();     // bisa lock lagi
}
```

---

## Condition Variable — Sinkronisasi Antar Thread

```cpp
#include <condition_variable>

queue<int> antrian;
mutex mtx;
condition_variable cv;
bool selesai = false;

// Producer
void produser() {
    for (int i = 0; i < 5; i++) {
        {
            lock_guard<mutex> lock(mtx);
            antrian.push(i);
            cout << "Diproduksi: " << i << "\n";
        }
        cv.notify_one();  // bangunkan konsumer
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    {
        lock_guard<mutex> lock(mtx);
        selesai = true;
    }
    cv.notify_all();
}

// Consumer
void konsumer() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, []{ return !antrian.empty() || selesai; });
        while (!antrian.empty()) {
            cout << "Dikonsumsi: " << antrian.front() << "\n";
            antrian.pop();
        }
        if (selesai) break;
    }
}
```

---

## `std::async` & `std::future`

```cpp
#include <future>

// async — jalankan fungsi secara asinkron
auto future = async(launch::async, []() {
    this_thread::sleep_for(chrono::seconds(1));
    return 42;
});

cout << "Menunggu hasil...\n";
int hasil = future.get();  // blokir sampai selesai
cout << "Hasil: " << hasil << "\n";

// promise & future — komunikasi antar thread
promise<string> janji;
future<string> hadiah = janji.get_future();

thread t([&janji]() {
    this_thread::sleep_for(chrono::milliseconds(500));
    janji.set_value("Halo dari thread!");
});

cout << hadiah.get() << "\n";  // tunggu nilai
t.join();
```

---

## `atomic` — Operasi Tanpa Lock

```cpp
#include <atomic>

atomic<int> counter = 0;

void tambah(int n) {
    for (int i = 0; i < n; i++) {
        ++counter;           // atomic — thread-safe tanpa mutex
        counter.fetch_add(1);  // eksplisit
    }
}

// atomic_flag — mutex sederhana
atomic_flag flag = ATOMIC_FLAG_INIT;
void spinlock_acquire() {
    while (flag.test_and_set(memory_order_acquire));  // spin
}
void spinlock_release() {
    flag.clear(memory_order_release);
}
```

---

## Thread Pool Sederhana

```cpp
class ThreadPool {
    vector<thread> pekerja;
    queue<function<void()>> tugas;
    mutex mtx;
    condition_variable cv;
    bool berhenti = false;

public:
    ThreadPool(size_t n) {
        for (size_t i = 0; i < n; i++) {
            pekerja.emplace_back([this]() {
                while (true) {
                    function<void()> t;
                    {
                        unique_lock<mutex> lock(mtx);
                        cv.wait(lock, [this]{ return berhenti || !tugas.empty(); });
                        if (berhenti && tugas.empty()) return;
                        t = move(tugas.front());
                        tugas.pop();
                    }
                    t();
                }
            });
        }
    }

    template<typename F>
    void tambahTugas(F&& f) {
        {
            lock_guard<mutex> lock(mtx);
            tugas.push(forward<F>(f));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        { lock_guard<mutex> lock(mtx); berhenti = true; }
        cv.notify_all();
        for (auto& t : pekerja) t.join();
    }
};
```

---

## Compile

```bash
# Linux/macOS — tambahkan -pthread
g++ -std=c++17 -pthread -O2 -o multithreading multithreading.cpp

# Windows (MinGW)
g++ -std=c++17 -pthread -o multithreading.exe multithreading.cpp
```

---

## Latihan

1. Hitung jumlah bilangan prima 1-100000 menggunakan 4 thread
2. Implementasikan producer-consumer pattern dengan buffer terbatas
3. Buat thread pool dan gunakan untuk memproses 20 task
4. Bandingkan waktu eksekusi single-thread vs multi-thread

---

**[← Lambda](../06-lambda/README.md)** | **[Berikutnya → Filesystem](../08-filesystem/README.md)**
