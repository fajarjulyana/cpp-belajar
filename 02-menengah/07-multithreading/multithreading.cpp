// ============================================================
// File  : multithreading.cpp
// Topik : Multithreading dalam C++
// Kompilasi: g++ -std=c++17 -pthread -Wall -o threading multithreading.cpp
// ============================================================
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <queue>
#include <vector>
#include <functional>
#include <chrono>
#include <numeric>
#include <string>
using namespace std;
using namespace chrono;

// ---- Mutex & Race Condition ----
mutex coutMtx;  // untuk output aman

void safeLog(const string& msg) {
    lock_guard<mutex> lock(coutMtx);
    cout << "  [Thread-" << this_thread::get_id() << "] " << msg << "\n";
}

// ---- Counter tanpa mutex (SALAH) vs dengan mutex (BENAR) ----
int counterTidakAman = 0;
atomic<int> counterAman = 0;
mutex counterMtx;
int counterMutex = 0;

void incrementTidakAman(int n) { for(int i=0;i<n;i++) counterTidakAman++; }
void incrementAman(int n)      { for(int i=0;i<n;i++) counterAman++; }
void incrementMutex(int n) {
    for (int i = 0; i < n; i++) {
        lock_guard<mutex> lock(counterMtx);
        counterMutex++;
    }
}

// ---- Hitung bilangan prima secara paralel ----
bool adaPembagi(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) if (n % i == 0) return false;
    return true;
}

int hitungPrimaDiRange(int awal, int akhir) {
    int count = 0;
    for (int n = awal; n < akhir; n++) if (adaPembagi(n)) count++;
    return count;
}

// ---- Producer-Consumer ----
queue<int>       buffer;
mutex            bufMtx;
condition_variable bufCV;
const int        KAPASITAS = 5;
bool             prodSelesai = false;

void produser(int id, int jumlah) {
    for (int i = 0; i < jumlah; i++) {
        unique_lock<mutex> lock(bufMtx);
        bufCV.wait(lock, []{ return (int)buffer.size() < KAPASITAS; });
        buffer.push(id * 100 + i);
        safeLog("Produser-" + to_string(id) + " membuat " + to_string(id*100+i));
        lock.unlock();
        bufCV.notify_all();
        this_thread::sleep_for(milliseconds(20));
    }
}

void konsumer(int id, int jumlah) {
    int dikonsumsi = 0;
    while (dikonsumsi < jumlah) {
        unique_lock<mutex> lock(bufMtx);
        bufCV.wait(lock, []{ return !buffer.empty() || prodSelesai; });
        if (!buffer.empty()) {
            int val = buffer.front(); buffer.pop();
            dikonsumsi++;
            safeLog("Konsumer-" + to_string(id) + " mengambil " + to_string(val));
            lock.unlock();
            bufCV.notify_all();
        } else if (prodSelesai) break;
    }
}

// ---- Thread Pool Sederhana ----
class ThreadPool {
    vector<thread> pekerja;
    queue<function<void()>> antrianTugas;
    mutex mtx;
    condition_variable cv;
    bool berhenti = false;

public:
    ThreadPool(int n) {
        for (int i = 0; i < n; i++) {
            pekerja.emplace_back([this]() {
                while (true) {
                    function<void()> tugas;
                    {
                        unique_lock<mutex> lock(mtx);
                        cv.wait(lock, [this]{ return berhenti || !antrianTugas.empty(); });
                        if (berhenti && antrianTugas.empty()) return;
                        tugas = move(antrianTugas.front());
                        antrianTugas.pop();
                    }
                    tugas();
                }
            });
        }
    }

    template<typename F, typename... Args>
    future<invoke_result_t<F, Args...>> kirim(F&& f, Args&&... args) {
        using R = invoke_result_t<F, Args...>;
        auto task = make_shared<packaged_task<R()>>(
            bind(forward<F>(f), forward<Args>(args)...));
        future<R> result = task->get_future();
        {
            lock_guard<mutex> lock(mtx);
            antrianTugas.push([task]{ (*task)(); });
        }
        cv.notify_one();
        return result;
    }

    ~ThreadPool() {
        { lock_guard<mutex> lock(mtx); berhenti = true; }
        cv.notify_all();
        for (auto& t : pekerja) t.join();
    }
};

int main() {
    // ---- Thread Dasar ----
    cout << "=== Thread Dasar ===" << endl;
    vector<thread> threads;
    for (int i = 1; i <= 4; i++) {
        threads.emplace_back([i]() {
            safeLog("Thread-" + to_string(i) + " berjalan");
            this_thread::sleep_for(milliseconds(i * 10));
            safeLog("Thread-" + to_string(i) + " selesai");
        });
    }
    for (auto& t : threads) t.join();

    // ---- Race Condition Demo ----
    cout << "\n=== Race Condition Demo ===" << endl;
    const int N = 10000;
    {
        counterTidakAman = 0;
        thread t1(incrementTidakAman, N), t2(incrementTidakAman, N);
        t1.join(); t2.join();
        cout << "Tanpa mutex (harusnya " << 2*N << "): " << counterTidakAman << "\n";
    }
    {
        counterAman = 0;
        thread t1(incrementAman, N), t2(incrementAman, N);
        t1.join(); t2.join();
        cout << "Dengan atomic    : " << counterAman.load() << "\n";
    }
    {
        counterMutex = 0;
        thread t1(incrementMutex, N), t2(incrementMutex, N);
        t1.join(); t2.join();
        cout << "Dengan mutex     : " << counterMutex << "\n";
    }

    // ---- Paralel Hitung Prima ----
    cout << "\n=== Hitung Prima 1-100000 ===" << endl;
    const int MAKS = 100000;
    const int nThread = 4;
    const int chunk = MAKS / nThread;

    auto mulai = high_resolution_clock::now();

    vector<future<int>> futures;
    for (int i = 0; i < nThread; i++) {
        int awal = i * chunk + 1;
        int akhir = (i == nThread-1) ? MAKS+1 : (i+1) * chunk + 1;
        futures.push_back(async(launch::async, hitungPrimaDiRange, awal, akhir));
    }
    int totalPrima = 0;
    for (auto& f : futures) totalPrima += f.get();

    auto selesai = high_resolution_clock::now();
    double ms = duration<double, milli>(selesai - mulai).count();
    cout << "Jumlah prima 1-" << MAKS << " = " << totalPrima
         << " (waktu: " << ms << " ms, " << nThread << " thread)\n";

    // ---- std::async & future ----
    cout << "\n=== async & future ===" << endl;
    auto fut = async(launch::async, []() {
        this_thread::sleep_for(milliseconds(50));
        return string("Hasil dari async task!");
    });
    cout << "Menunggu hasil async...\n";
    cout << fut.get() << "\n";

    // ---- Thread Pool ----
    cout << "\n=== Thread Pool ===" << endl;
    ThreadPool pool(3);
    vector<future<int>> hasil;
    for (int i = 0; i < 6; i++) {
        hasil.push_back(pool.kirim([i]() {
            safeLog("Tugas-" + to_string(i) + " mulai");
            this_thread::sleep_for(milliseconds(30 + i*10));
            safeLog("Tugas-" + to_string(i) + " selesai, hasil=" + to_string(i*i));
            return i * i;
        }));
    }
    int sumHasil = 0;
    for (auto& h : hasil) sumHasil += h.get();
    cout << "Sum hasil thread pool: " << sumHasil << "\n";

    return 0;
}
