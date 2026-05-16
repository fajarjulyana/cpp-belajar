// ============================================================
// File  : stl.cpp
// Topik : Standard Template Library (STL)
// Kompilasi: g++ -std=c++17 -Wall -o stl_demo stl.cpp
// ============================================================
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <numeric>
#include <string>
#include <functional>
using namespace std;

int main() {
    // ---- vector ----
    cout << "=== vector ===" << endl;
    vector<int> v = {5, 2, 8, 1, 9, 3, 7};
    v.push_back(4); v.push_back(6);
    sort(v.begin(), v.end());
    cout << "Terurut: ";
    for (int x : v) cout << x << " "; cout << "\n";
    cout << "Min=" << *min_element(v.begin(),v.end())
         << " Max=" << *max_element(v.begin(),v.end())
         << " Sum=" << accumulate(v.begin(),v.end(),0) << "\n";

    // ---- map ----
    cout << "\n=== map<string,int> ===" << endl;
    map<string, int> frekuensi;
    string kalimat = "belajar cpp belajar stl belajar algoritma";
    stringstream ss(kalimat);
    string kata;
    while (ss >> kata) frekuensi[kata]++;
    for (const auto& [k, f] : frekuensi)
        cout << "  " << k << ": " << f << "x\n";

    // ---- unordered_map ----
    cout << "\n=== unordered_map (Kamus EN-ID) ===" << endl;
    unordered_map<string, string> kamus = {
        {"apple","apel"},{"orange","jeruk"},{"banana","pisang"},
        {"grape","anggur"},{"mango","mangga"}
    };
    vector<string> cari = {"apple","mango","durian"};
    for (const auto& c : cari) {
        auto it = kamus.find(c);
        cout << c << " → " << (it != kamus.end() ? it->second : "tidak ditemukan") << "\n";
    }

    // ---- set ----
    cout << "\n=== set (Unik & Terurut) ===" << endl;
    vector<int> data = {3,1,4,1,5,9,2,6,5,3,5};
    set<int> unik(data.begin(), data.end());
    cout << "Duplikat dihapus: ";
    for (int x : unik) cout << x << " "; cout << "\n";

    // ---- stack ----
    cout << "\n=== stack ===" << endl;
    stack<string> riwayat;
    riwayat.push("halaman-1");
    riwayat.push("halaman-2");
    riwayat.push("halaman-3");
    cout << "Browser back:\n";
    while (!riwayat.empty()) {
        cout << "  " << riwayat.top() << "\n";
        riwayat.pop();
    }

    // ---- queue ----
    cout << "\n=== queue (FIFO) ===" << endl;
    queue<string> antrian;
    antrian.push("Pelanggan-1");
    antrian.push("Pelanggan-2");
    antrian.push("Pelanggan-3");
    cout << "Layanan:\n";
    while (!antrian.empty()) {
        cout << "  Melayani: " << antrian.front() << "\n";
        antrian.pop();
    }

    // ---- priority_queue ----
    cout << "\n=== priority_queue (Leaderboard) ===" << endl;
    priority_queue<pair<int,string>> leaderboard;
    leaderboard.push({85,  "Alfa"});
    leaderboard.push({92,  "Beta"});
    leaderboard.push({78,  "Gamma"});
    leaderboard.push({95,  "Delta"});
    leaderboard.push({88,  "Epsilon"});
    int rank = 1;
    while (!leaderboard.empty()) {
        auto [skor, nama] = leaderboard.top();
        cout << "  #" << rank++ << " " << nama << ": " << skor << "\n";
        leaderboard.pop();
    }

    // ---- Algoritma STL ----
    cout << "\n=== Algoritma STL ===" << endl;
    vector<int> nums = {4, 7, 2, 9, 1, 5, 8, 3, 6};

    // count_if
    int genap = count_if(nums.begin(), nums.end(), [](int x){ return x%2==0; });
    int ganjil = count_if(nums.begin(), nums.end(), [](int x){ return x%2!=0; });
    cout << "Genap: " << genap << ", Ganjil: " << ganjil << "\n";

    // transform
    vector<int> kuadrat(nums.size());
    transform(nums.begin(), nums.end(), kuadrat.begin(), [](int x){ return x*x; });
    cout << "Kuadrat: ";
    for (int x : kuadrat) cout << x << " "; cout << "\n";

    // partition
    vector<int> partisi = nums;
    auto mid = partition(partisi.begin(), partisi.end(), [](int x){ return x%2==0; });
    cout << "Genap dulu: ";
    for (int x : partisi) cout << x << " "; cout << "\n";

    // rotate
    vector<int> rot = {1,2,3,4,5};
    rotate(rot.begin(), rot.begin()+2, rot.end());
    cout << "Setelah rotate(2): ";
    for (int x : rot) cout << x << " "; cout << "\n";

    // Anagram grouping
    cout << "\n=== Anagram Grouping ===" << endl;
    vector<string> kata_kata = {"eat","tea","tan","ate","nat","bat"};
    map<string, vector<string>> anagram;
    for (const auto& w : kata_kata) {
        string key = w; sort(key.begin(), key.end());
        anagram[key].push_back(w);
    }
    for (const auto& [k, gr] : anagram) {
        cout << "[" << k << "]: ";
        for (const auto& w : gr) cout << w << " ";
        cout << "\n";
    }

    // Frekuensi karakter
    cout << "\n=== Frekuensi Karakter ===" << endl;
    string teks = "programming";
    map<char, int> freq;
    for (char c : teks) freq[c]++;
    for (const auto& [c, f] : freq)
        cout << "'" << c << "': " << f << "\n";

    return 0;
}
