# Bab 16 — Lambda & Functional Programming

Lambda adalah **fungsi anonim** yang bisa dideklarasikan langsung di tempat pemakaian.

---

## Sintaks Lambda

```
[capture](parameter) mutable -> return_type { body }
  ^^^      ^^^                    ^^^
  1        2                      3
1: apa yang "ditangkap" dari scope luar
2: parameter fungsi (opsional)
3: tipe return (opsional, bisa auto-deduced)
```

---

## Lambda Dasar

```cpp
// Lambda tanpa parameter
auto salam = []() { cout << "Halo!\n"; };
salam();  // panggil seperti fungsi biasa

// Lambda dengan parameter
auto tambah = [](int a, int b) { return a + b; };
cout << tambah(3, 5) << "\n";  // 8

// Lambda dengan return type eksplisit
auto bagi = [](double a, double b) -> double {
    if (b == 0) return 0.0;
    return a / b;
};

// Lambda dengan auto parameter (C++14 generic lambda)
auto cetakApa = [](auto x) { cout << x << "\n"; };
cetakApa(42);
cetakApa("halo");
cetakApa(3.14);
```

---

## Capture List

```cpp
int x = 10, y = 20;

// Capture by value
auto f1 = [x, y]() { cout << x + y << "\n"; };  // salinan x dan y
f1();   // 30, x dan y tidak berubah walau kita ubah

// Capture by reference
auto f2 = [&x, &y]() { x *= 2; y *= 2; };
f2();
cout << x << " " << y << "\n";  // 20 40

// Capture semua by value
auto f3 = [=]() { return x + y; };

// Capture semua by reference
auto f4 = [&]() { x = 0; y = 0; };

// Capture campuran
string nama = "Budi";
auto f5 = [=, &nama]() { nama += " Santoso"; return x + y; };
// = untuk semua, kecuali nama yang by reference

// this pointer
struct MyClass {
    int nilai = 100;
    auto buatLambda() {
        return [this]() { return nilai * 2; };  // capture this
    }
    auto buatLambda2() {
        return [*this]() { return nilai * 2; };  // capture salinan *this (C++17)
    }
};
```

---

## `mutable` Lambda

```cpp
int hitungan = 0;

// Tanpa mutable: capture by value TIDAK BISA diubah
auto f1 = [hitungan]() {
    // hitungan++;  // ERROR tanpa mutable
    return hitungan;
};

// Dengan mutable: salinan lokal bisa diubah
auto f2 = [hitungan]() mutable {
    hitungan++;  // mengubah salinan lokal, bukan variabel asli
    return hitungan;
};

cout << f2() << "\n";  // 1 (salinan lokal)
cout << f2() << "\n";  // 2
cout << hitungan << "\n";  // 0 (asli tidak berubah!)
```

---

## Lambda sebagai Parameter Fungsi

```cpp
#include <algorithm>
#include <functional>

// std::function — wrapper untuk callable apapun
function<int(int, int)> op;
op = [](int a, int b) { return a + b; };
cout << op(3, 4) << "\n";  // 7

// Fungsi yang menerima lambda
void terapkan(vector<int>& v, function<void(int&)> f) {
    for (auto& x : v) f(x);
}

vector<int> angka = {1, 2, 3, 4, 5};
terapkan(angka, [](int& x) { x *= 2; });
// angka sekarang: {2, 4, 6, 8, 10}

// Dengan template (lebih efisien dari std::function)
template<typename F>
void terapkanT(vector<int>& v, F f) {
    for (auto& x : v) f(x);
}
```

---

## Higher-Order Functions

```cpp
// Map (transform)
vector<int> nums = {1, 2, 3, 4, 5};
vector<int> kuadrat;
transform(nums.begin(), nums.end(), back_inserter(kuadrat),
          [](int x) { return x * x; });

// Filter
vector<int> genap;
copy_if(nums.begin(), nums.end(), back_inserter(genap),
        [](int x) { return x % 2 == 0; });

// Reduce (accumulate)
int jumlah = accumulate(nums.begin(), nums.end(), 0,
                        [](int acc, int x) { return acc + x; });

// Sort dengan comparator kustom
vector<pair<string,int>> data = {{"Budi",85},{"Ani",92},{"Candra",78}};
sort(data.begin(), data.end(),
     [](const auto& a, const auto& b) { return a.second > b.second; });
// Urutkan berdasarkan nilai (descending)
```

---

## Immediately Invoked Lambda (IIFE)

```cpp
// Inisialisasi kompleks dengan IIFE
const int nilai = []() {
    int x = 10;
    x *= 2;
    x += 5;
    return x;
}();  // dipanggil langsung!
cout << nilai << "\n";  // 25

// Berguna untuk inisialisasi const dengan logika kompleks
const string label = [](int n) -> string {
    if (n >= 90) return "A";
    if (n >= 80) return "B";
    if (n >= 70) return "C";
    return "D";
}(85);
```

---

## Rekursi dengan Lambda (C++14)

```cpp
// Lambda rekursif menggunakan std::function
function<int(int)> faktorial = [&faktorial](int n) -> int {
    return n <= 1 ? 1 : n * faktorial(n-1);
};
cout << faktorial(10) << "\n";

// Cara lebih efisien dengan Y-combinator
auto faktorial2 = [](auto self, int n) -> int {
    return n <= 1 ? 1 : n * self(self, n-1);
};
cout << faktorial2(faktorial2, 10) << "\n";
```

---

## `std::bind` (C++11)

```cpp
#include <functional>

double pangkat(double basis, int exp) {
    return pow(basis, exp);
}

// Bind argumen pertama
auto kuadrat  = bind(pangkat, placeholders::_1, 2);
auto kubik    = bind(pangkat, placeholders::_1, 3);
auto duaToPow = bind(pangkat, 2.0, placeholders::_1);

cout << kuadrat(5)   << "\n";  // 25
cout << kubik(3)     << "\n";  // 27
cout << duaToPow(10) << "\n";  // 1024
```

---

## Latihan

1. Buat pipeline pemrosesan data: filter genap → kali 2 → jumlahkan
2. Buat fungsi `compose(f, g)` yang mengembalikan `f(g(x))`
3. Implementasikan quicksort menggunakan lambda sebagai comparator
4. Buat event system sederhana menggunakan `map<string, vector<function<void()>>>`

---

**[← Smart Pointer](../05-smart-pointer/README.md)** | **[Berikutnya → Multithreading](../07-multithreading/README.md)**
