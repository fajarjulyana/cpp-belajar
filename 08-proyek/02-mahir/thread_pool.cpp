// ============================================================
// File  : thread_pool.cpp
// Proyek: Thread Pool & Task Queue
//
// Kompilasi:
//   g++ -std=c++17 -Wall -pthread -o thread_pool thread_pool.cpp
// Jalankan:
//   ./thread_pool
// ============================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <string>
#include <sstream>
#include <numeric>
#include <cmath>

// ============================================================
// Logger Thread-Safe
// ============================================================
class Logger {
    std::mutex mtx;
public:
    template<typename... Args>
    void log(Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) % 1000;
        auto t = std::chrono::system_clock::to_time_t(now);
        std::cout << "[" << std::put_time(std::localtime(&t), "%H:%M:%S")
                  << "." << std::setfill('0') << std::setw(3) << ms.count()
                  << "] ";
        (std::cout << ... << std::forward<Args>(args));
        std::cout << "\n";
    }
};
static Logger gLog;

// ============================================================
// Thread Pool
// ============================================================
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this, i]() { workerLoop(i); });
        }
        gLog.log("ThreadPool dibuat dengan ", numThreads, " worker threads");
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMtx);
            stop = true;
        }
        cv.notify_all();
        for (auto& t : workers) t.join();
        gLog.log("ThreadPool dimatikan. Total task: ", totalTasksDone.load());
    }

    // Submit task dengan return value (menggunakan std::future)
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<decltype(f(args...))>
    {
        using ReturnType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMtx);
            if (stop) throw std::runtime_error("ThreadPool sudah dihentikan");
            tasks.emplace([task]() { (*task)(); });
            tasksPending++;
        }
        cv.notify_one();
        return result;
    }

    // Tunggu semua task selesai
    void waitAll() {
        std::unique_lock<std::mutex> lock(queueMtx);
        cvDone.wait(lock, [this]() {
            return tasks.empty() && tasksPending == 0;
        });
    }

    // Statistik
    size_t threadCount()    const { return workers.size(); }
    size_t queueSize()      const {
        std::lock_guard<std::mutex> lock(queueMtx);
        return tasks.size();
    }
    long long tasksDone()   const { return totalTasksDone.load(); }
    bool      isBusy()      const { return tasksPending > 0; }

private:
    void workerLoop(size_t threadId) {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMtx);
                cv.wait(lock, [this]() { return stop || !tasks.empty(); });

                if (stop && tasks.empty()) return;

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();
            totalTasksDone++;
            {
                std::unique_lock<std::mutex> lock(queueMtx);
                tasksPending--;
            }
            cvDone.notify_all();
        }
    }

    std::vector<std::thread>        workers;
    std::queue<std::function<void()>> tasks;
    mutable std::mutex              queueMtx;
    std::condition_variable         cv;
    std::condition_variable         cvDone;
    std::atomic<bool>               stop { false };
    std::atomic<long long>          totalTasksDone { 0 };
    int                             tasksPending { 0 };
};

// ============================================================
// Demo 1: Komputasi Paralel (hitung bilangan prima)
// ============================================================
bool isPrima(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

long long hitungPrimaRange(long long mulai, long long akhir) {
    long long count = 0;
    for (long long i = mulai; i <= akhir; i++)
        if (isPrima(i)) count++;
    return count;
}

void demoKomputasiParalel(ThreadPool& pool) {
    gLog.log("\n=== DEMO 1: Hitung Bilangan Prima (0 - 1.000.000) ===");

    const long long TOTAL  = 1'000'000;
    const int       CHUNKS = 8;
    const long long CHUNK  = TOTAL / CHUNKS;

    // Submit semua task sekaligus
    std::vector<std::future<long long>> futures;
    auto t0 = std::chrono::steady_clock::now();

    for (int i = 0; i < CHUNKS; i++) {
        long long mulai = i * CHUNK + 1;
        long long akhir = (i == CHUNKS - 1) ? TOTAL : (i + 1) * CHUNK;
        futures.push_back(pool.submit(hitungPrimaRange, mulai, akhir));
    }

    long long total = 0;
    for (auto& f : futures) total += f.get();

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    gLog.log("Jumlah prima 1 - ", TOTAL, ": ", total, " (", ms, " ms, ", CHUNKS, " thread)");

    // Bandingkan dengan single-thread
    auto t2 = std::chrono::steady_clock::now();
    long long singleResult = hitungPrimaRange(1, TOTAL);
    auto t3 = std::chrono::steady_clock::now();
    auto msSingle = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();

    gLog.log("Single-thread: ", singleResult, " (", msSingle, " ms)");
    gLog.log("Speedup: ", std::fixed, std::setprecision(2),
             (double)msSingle / std::max(1LL, (long long)ms), "x");
}

// ============================================================
// Demo 2: Pipeline dengan dependencies
// ============================================================
void demoPipeline(ThreadPool& pool) {
    gLog.log("\n=== DEMO 2: Pipeline Pemrosesan Data ===");

    std::vector<int> data(20);
    std::iota(data.begin(), data.end(), 1);  // 1..20

    // Stage 1: Filter bilangan genap
    auto futureFilter = pool.submit([&data]() {
        std::vector<int> result;
        for (int x : data) if (x % 2 == 0) result.push_back(x);
        gLog.log("  Stage 1 (filter genap): ", result.size(), " elemen");
        return result;
    });

    auto filtered = futureFilter.get();

    // Stage 2: Kuadratkan
    auto futureSquare = pool.submit([filtered]() {
        std::vector<long long> result;
        for (int x : filtered) result.push_back((long long)x * x);
        gLog.log("  Stage 2 (kuadrat): selesai");
        return result;
    });

    auto squared = futureSquare.get();

    // Stage 3: Jumlahkan
    auto futureSum = pool.submit([squared]() {
        long long sum = 0;
        for (long long x : squared) sum += x;
        gLog.log("  Stage 3 (jumlah): ", sum);
        return sum;
    });

    long long hasil = futureSum.get();
    gLog.log("  Hasil pipeline: ", hasil, " (sum of squares of even numbers 1-20)");
}

// ============================================================
// Demo 3: Task dengan timeout
// ============================================================
void demoTaskTimeout(ThreadPool& pool) {
    gLog.log("\n=== DEMO 3: Task dengan Timeout ===");

    // Task yang lama
    auto future = pool.submit([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return std::string("Task selesai!");
    });

    // Coba ambil hasil dengan timeout
    auto status = future.wait_for(std::chrono::milliseconds(100));
    if (status == std::future_status::ready) {
        gLog.log("  Task selesai dalam waktu: ", future.get());
    } else {
        gLog.log("  Task belum selesai dalam 100ms, menunggu...");
        gLog.log("  Hasil: ", future.get());
    }
}

// ============================================================
// Demo 4: Fire-and-forget tasks
// ============================================================
std::atomic<int> counterGlobal { 0 };

void demoFireAndForget(ThreadPool& pool) {
    gLog.log("\n=== DEMO 4: 100 Task Paralel (fire-and-forget) ===");
    counterGlobal = 0;

    auto t0 = std::chrono::steady_clock::now();

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 100; i++) {
        futures.push_back(pool.submit([i]() {
            // Simulasi kerja berbeda
            int delay = (i % 5) * 5;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            counterGlobal++;
        }));
    }

    for (auto& f : futures) f.wait();

    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    gLog.log("  100 task selesai dalam ", ms, " ms (counter=", counterGlobal.load(), ")");
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║   THREAD POOL C++   v1.0                ║\n";
    std::cout << "  ║   Hardware threads: "
              << std::setw(2) << std::thread::hardware_concurrency()
              << "                     ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n\n";

    const int NUM_THREADS = std::max(2u, std::thread::hardware_concurrency());
    ThreadPool pool(NUM_THREADS);

    demoKomputasiParalel(pool);
    demoPipeline(pool);
    demoTaskTimeout(pool);
    demoFireAndForget(pool);

    gLog.log("\n=== Statistik Thread Pool ===");
    gLog.log("  Total worker threads : ", pool.threadCount());
    gLog.log("  Total task selesai   : ", pool.tasksDone());
    gLog.log("\nProgram selesai.\n");

    return 0;
}
