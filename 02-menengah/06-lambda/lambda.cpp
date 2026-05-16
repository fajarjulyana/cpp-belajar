// ============================================================
// File  : lambda.cpp
// Topik : Lambda & Functional Programming dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o lambda_demo lambda.cpp
// ============================================================
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <map>
#include <string>
using namespace std;

// ---- Higher-order function helpers ----
template<typename C, typename F>
auto filter(const C& c, F pred) {
    C hasil;
    copy_if(c.begin(), c.end(), back_inserter(hasil), pred);
    return hasil;
}

template<typename C, typename F>
auto transformAll(const C& c, F func) {
    vector<decltype(func(*c.begin()))> hasil;
    transform(c.begin(), c.end(), back_inserter(hasil), func);
    return hasil;
}

template<typename C, typename T, typename F>
T reduce(const C& c, T init, F f) {
    return accumulate(c.begin(), c.end(), init, f);
}

int main() {
    // ---- Lambda Dasar ----
    cout << "=== Lambda Dasar ===" << endl;
    auto salam  = []() { cout << "Halo, Lambda!\n"; };
    auto tambah = [](int a, int b) { return a + b; };
    auto bagi   = [](double a, double b) -> double {
        return b != 0 ? a / b : 0.0;
    };
    auto cetakApa = [](auto x) { cout << x << "\n"; };  // generic

    salam();
    cout << "tambah(3,5) = " << tambah(3, 5) << "\n";
    cout << "bagi(10,3)  = " << bagi(10, 3)  << "\n";
    cetakApa(42); cetakApa("teks"); cetakApa(3.14);

    // ---- Capture ----
    cout << "\n=== Capture ===" << endl;
    int x = 10, y = 20;
    auto byVal = [x, y]() { return x + y; };
    auto byRef = [&x, &y]() { x *= 2; y += 5; };
    cout << "byVal() = " << byVal() << "\n";  // 30
    byRef();
    cout << "Setelah byRef: x=" << x << " y=" << y << "\n";  // 20 25

    string nama = "Budi";
    auto ubahNama = [&nama](const string& suffix) { nama += " " + suffix; };
    ubahNama("Santoso");
    cout << "nama: " << nama << "\n";

    // ---- mutable ----
    cout << "\n=== mutable Lambda ===" << endl;
    int counter = 0;
    auto increment = [counter]() mutable { return ++counter; };
    cout << increment() << "\n";  // 1
    cout << increment() << "\n";  // 2
    cout << "counter asli: " << counter << "\n";  // 0 (tidak berubah)

    // Simulasi counter state
    auto makeCounter = [](int awal) {
        return [n = awal]() mutable { return n++; };
    };
    auto cnt = makeCounter(100);
    cout << cnt() << " " << cnt() << " " << cnt() << "\n";  // 100 101 102

    // ---- std::function ----
    cout << "\n=== std::function ===" << endl;
    function<int(int, int)> op = [](int a, int b) { return a + b; };
    cout << "op(5,3) = " << op(5, 3) << "\n";
    op = [](int a, int b) { return a * b; };
    cout << "op(5,3) = " << op(5, 3) << "\n";

    // ---- Higher-Order Functions ----
    cout << "\n=== Higher-Order Functions ===" << endl;
    vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Filter
    auto genap = filter(nums, [](int n) { return n % 2 == 0; });
    cout << "Genap: ";
    for (int n : genap) cout << n << " "; cout << "\n";

    // Transform (map)
    auto kuadrat = transformAll(nums, [](int n) { return n * n; });
    cout << "Kuadrat: ";
    for (int n : kuadrat) cout << n << " "; cout << "\n";

    // Reduce
    int total = reduce(nums, 0, [](int acc, int n) { return acc + n; });
    cout << "Total: " << total << "\n";

    int produk = reduce(nums, 1, [](int acc, int n) { return acc * n; });
    cout << "Produk: " << produk << "\n";

    // Pipeline: filter genap → kuadrat → jumlahkan
    auto hasilPipeline = reduce(
        transformAll(filter(nums, [](int n){ return n%2==0; }),
                     [](int n){ return n*n; }),
        0, [](int acc, int n){ return acc + n; });
    cout << "Pipeline (genap→kuadrat→jumlah): " << hasilPipeline << "\n";

    // ---- Sort dengan lambda ----
    cout << "\n=== Sort dengan Lambda ===" << endl;
    vector<pair<string,int>> mahasiswa = {
        {"Budi",85},{"Ani",92},{"Candra",78},{"Dewi",95},{"Eko",70}
    };

    sort(mahasiswa.begin(), mahasiswa.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });
    cout << "Berdasarkan nilai (desc):\n";
    for (const auto& [n, v] : mahasiswa) cout << "  " << n << ": " << v << "\n";

    sort(mahasiswa.begin(), mahasiswa.end(),
         [](const auto& a, const auto& b) { return a.first < b.first; });
    cout << "Berdasarkan nama (asc):\n";
    for (const auto& [n, v] : mahasiswa) cout << "  " << n << ": " << v << "\n";

    // ---- Lambda Rekursif ----
    cout << "\n=== Lambda Rekursif ===" << endl;
    function<long long(int)> faktorial = [&faktorial](int n) -> long long {
        return n <= 1 ? 1 : n * faktorial(n-1);
    };
    for (int i = 0; i <= 10; i++)
        cout << i << "! = " << faktorial(i) << "\n";

    // ---- IIFE ----
    cout << "\n=== IIFE (Immediately Invoked) ===" << endl;
    const string label = [](int n) -> string {
        if (n >= 90) return "A";
        if (n >= 80) return "B";
        if (n >= 70) return "C";
        return "D";
    }(85);
    cout << "Label untuk 85: " << label << "\n";

    // ---- Event System ----
    cout << "\n=== Event System ===" << endl;
    map<string, vector<function<void(const string&)>>> events;

    events["klik"].push_back([](const string& data){ cout << "  Handler-1: klik " << data << "\n"; });
    events["klik"].push_back([](const string& data){ cout << "  Handler-2: klik " << data << "\n"; });
    events["hover"].push_back([](const string& data){ cout << "  Hover: " << data << "\n"; });

    auto emit = [&events](const string& event, const string& data) {
        if (events.count(event))
            for (auto& h : events[event]) h(data);
    };

    emit("klik", "tombol-submit");
    emit("hover", "menu-item");
    emit("kosong", "test");  // tidak ada handler, diam saja

    // ---- std::bind ----
    cout << "\n=== std::bind ===" << endl;
    auto pow = [](double b, int e) { double r=1; for(int i=0;i<e;i++) r*=b; return r; };
    auto kuadratFn  = bind(pow, placeholders::_1, 2);
    auto kubikFn    = bind(pow, placeholders::_1, 3);
    auto duaToPow   = bind(pow, 2.0, placeholders::_1);
    cout << "5^2 = " << kuadratFn(5) << "\n";
    cout << "3^3 = " << kubikFn(3)   << "\n";
    cout << "2^10= " << duaToPow(10) << "\n";

    return 0;
}
