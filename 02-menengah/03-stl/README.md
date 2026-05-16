# Bab 13 — STL (Standard Template Library)

STL adalah kumpulan **container**, **algoritma**, dan **iterator** yang sangat powerful yang sudah tersedia di C++ standar.

---

## Tiga Komponen Utama STL

```
STL
├── Container     — menyimpan koleksi data
│   ├── Sequence  : vector, list, deque, array
│   ├── Associative: map, set, multimap, multiset
│   └── Unordered : unordered_map, unordered_set
├── Algoritma     — fungsi untuk memproses container
│   ├── sort, find, count, transform, accumulate...
└── Iterator      — penghubung container dan algoritma
    ├── Input, Output, Forward, Bidirectional, Random
```

---

## Container Sekuensial

### `vector<T>` — Array Dinamis
```cpp
#include <vector>
vector<int> v = {5, 2, 8, 1};
v.push_back(9);         // tambah di belakang: {5,2,8,1,9}
v.pop_back();           // hapus belakang
v.insert(v.begin(), 0); // sisipkan di depan
v.erase(v.begin()+2);   // hapus indeks 2
cout << v.size()  << "\n"; // ukuran
cout << v.front() << "\n"; // elemen pertama
cout << v.back()  << "\n"; // elemen terakhir
v.clear();               // kosongkan
```

### `list<T>` — Doubly Linked List
```cpp
#include <list>
list<int> l = {3, 1, 4, 1, 5};
l.push_front(0);         // tambah di depan
l.push_back(9);          // tambah di belakang
l.remove(1);             // hapus semua elemen bernilai 1
l.sort();                // sort in-place
l.reverse();             // balik
l.unique();              // hapus duplikat berurutan
```

### `deque<T>` — Double-Ended Queue
```cpp
#include <deque>
deque<int> dq = {2, 3, 4};
dq.push_front(1);   // tambah depan
dq.push_back(5);    // tambah belakang
dq.pop_front();     // hapus depan
dq.pop_back();      // hapus belakang
```

### `array<T,N>` — Array Fixed-size Modern
```cpp
#include <array>
array<int, 5> a = {1, 2, 3, 4, 5};
a.fill(0);           // isi semua 0
a.size();            // 5 (compile-time constant)
```

---

## Container Asosiatif

### `map<K,V>` — Key-Value, Terurut
```cpp
#include <map>
map<string, int> nilai;
nilai["Budi"] = 85;
nilai["Ani"]  = 90;
nilai["Candra"] = 78;

// Iterasi (urutan alphabet)
for (const auto& [nama, n] : nilai)  // C++17 structured binding
    cout << nama << ": " << n << "\n";

// Akses dan cek
if (nilai.count("Budi")) cout << "Budi ada\n";
auto it = nilai.find("Ani");
if (it != nilai.end()) cout << it->second << "\n";

nilai.erase("Candra");  // hapus
```

### `unordered_map<K,V>` — Key-Value, Tidak Terurut (Hash Map)
```cpp
#include <unordered_map>
unordered_map<string, string> kamus;
kamus["apple"]  = "apel";
kamus["orange"] = "jeruk";
kamus["banana"] = "pisang";

cout << kamus.at("apple") << "\n";  // "apel"
cout << kamus.count("mango") << "\n";  // 0
```

### `set<T>` — Kumpulan Unik, Terurut
```cpp
#include <set>
set<int> s = {5, 3, 8, 1, 9, 3, 5};  // duplikat diabaikan
// s = {1, 3, 5, 8, 9}

s.insert(7);          // tambah
s.erase(3);           // hapus
s.count(5);           // 1 jika ada, 0 jika tidak
s.find(8) != s.end(); // true
```

### `multimap` dan `multiset` — Boleh Duplikat
```cpp
#include <map>
multimap<string, int> jadwal;
jadwal.insert({"Senin", 8});
jadwal.insert({"Senin", 14});   // boleh duplikat key
jadwal.insert({"Selasa", 9});

auto range = jadwal.equal_range("Senin");
for (auto it = range.first; it != range.second; ++it)
    cout << it->second << "\n";  // 8, 14
```

---

## Container Adaptor

```cpp
#include <stack>
stack<int> st;
st.push(1); st.push(2); st.push(3);
cout << st.top() << "\n";   // 3
st.pop();

#include <queue>
queue<int> q;
q.push(1); q.push(2); q.push(3);
cout << q.front() << "\n";  // 1
q.pop();

#include <queue>  // priority_queue
priority_queue<int> pq;  // max-heap
pq.push(3); pq.push(1); pq.push(4); pq.push(1);
while (!pq.empty()) { cout << pq.top() << " "; pq.pop(); }
// Output: 4 3 1 1
```

---

## Algoritma STL

```cpp
#include <algorithm>
#include <numeric>

vector<int> v = {5, 2, 8, 1, 9, 3, 7, 4, 6};

// Sort
sort(v.begin(), v.end());                     // ascending
sort(v.begin(), v.end(), greater<int>());     // descending
sort(v.begin(), v.end(), [](int a, int b){return a%2 < b%2;});  // genap dulu

// Find
auto it = find(v.begin(), v.end(), 5);
if (it != v.end()) cout << "Ditemukan di indeks " << distance(v.begin(), it) << "\n";

// Count
cout << count(v.begin(), v.end(), 1) << "\n";       // jumlah elemen = 1
cout << count_if(v.begin(), v.end(), [](int x){ return x%2==0; }) << "\n";

// Min/Max
cout << *min_element(v.begin(), v.end()) << "\n";
cout << *max_element(v.begin(), v.end()) << "\n";
auto [lo, hi] = minmax_element(v.begin(), v.end());

// Accumulate (sum)
int total = accumulate(v.begin(), v.end(), 0);
int produk = accumulate(v.begin(), v.end(), 1, multiplies<int>());

// Transform
vector<int> kuadrat(v.size());
transform(v.begin(), v.end(), kuadrat.begin(), [](int x){ return x*x; });

// Reverse
reverse(v.begin(), v.end());

// Unique (hapus berurutan duplikat — array harus terurut dulu)
sort(v.begin(), v.end());
auto ujung = unique(v.begin(), v.end());
v.erase(ujung, v.end());

// Binary Search (harus terurut)
bool ada = binary_search(v.begin(), v.end(), 5);
auto lb = lower_bound(v.begin(), v.end(), 5);  // iterator ke elemen >= 5
auto ub = upper_bound(v.begin(), v.end(), 5);  // iterator ke elemen > 5

// For each
for_each(v.begin(), v.end(), [](int& x){ x *= 2; });

// Rotate, shuffle, fill
fill(v.begin(), v.end(), 0);
iota(v.begin(), v.end(), 1);  // isi 1,2,3,...
```

---

## Iterator

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// Forward iterator
for (auto it = v.begin(); it != v.end(); ++it)
    cout << *it << " ";

// Reverse iterator
for (auto it = v.rbegin(); it != v.rend(); ++it)
    cout << *it << " ";

// Const iterator (tidak bisa ubah nilai)
for (auto it = v.cbegin(); it != v.cend(); ++it)
    cout << *it << " ";

// advance dan distance
auto it = v.begin();
advance(it, 3);         // maju 3 langkah
int jarak = distance(v.begin(), it);  // 3
```

---

## `string_view` (C++17)

```cpp
#include <string_view>
// Referensi ringan ke string (tidak menyalin!)
void cetak(string_view sv) {
    cout << sv.substr(0, 5) << "\n";
}
string s = "Halo, Dunia!";
cetak(s);          // dari std::string
cetak("Selamat"); // dari string literal
```

---

## Latihan

1. Hitung frekuensi setiap karakter dalam string menggunakan `unordered_map`
2. Implementasikan Dijkstra's shortest path menggunakan `priority_queue` dan `map`
3. Gunakan `multiset` untuk membuat leaderboard yang otomatis terurut
4. Buat program `anagram grouping` menggunakan `map<string, vector<string>>`

---

**[← Template](../02-template/README.md)** | **[Berikutnya → Exception Handling](../04-exception/README.md)**
