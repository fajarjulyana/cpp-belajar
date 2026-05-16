# Bab 12 — Template

Template memungkinkan penulisan kode **generik** yang bisa bekerja dengan berbagai tipe data tanpa duplikasi kode.

---

## Function Template

```cpp
// Template fungsi untuk mencari nilai maksimum
template<typename T>
T maks(T a, T b) {
    return a > b ? a : b;
}

cout << maks(3, 7)        << "\n";     // int: 7
cout << maks(3.5, 2.1)    << "\n";     // double: 3.5
cout << maks('a', 'z')    << "\n";     // char: z
cout << maks<int>(3, 4.5) << "\n";     // eksplisit int: 4
```

### Multiple Template Parameters

```cpp
template<typename T, typename U>
auto gabung(T a, U b) {
    return to_string(a) + " + " + to_string(b);
}

template<typename T, typename U>
pair<T, U> buatPasang(T first, U second) {
    return {first, second};
}
```

---

## Class Template

```cpp
template<typename T>
class Tumpukan {
private:
    vector<T> data;
    int kapasitasMaks;
public:
    Tumpukan(int k = 100) : kapasitasMaks(k) {}

    void push(const T& val) {
        if ((int)data.size() >= kapasitasMaks)
            throw overflow_error("Tumpukan penuh!");
        data.push_back(val);
    }

    T pop() {
        if (kosong()) throw underflow_error("Tumpukan kosong!");
        T val = data.back();
        data.pop_back();
        return val;
    }

    T& top() {
        if (kosong()) throw underflow_error("Tumpukan kosong!");
        return data.back();
    }

    bool kosong() const { return data.empty(); }
    int  ukuran() const { return (int)data.size(); }
};

Tumpukan<int>    intStack;
Tumpukan<string> strStack;
Tumpukan<double> dblStack;
```

---

## Template Specialization (Spesialisasi)

```cpp
// Template umum
template<typename T>
string cetakTipe(T val) {
    return "Nilai: " + to_string(val);
}

// Spesialisasi untuk string
template<>
string cetakTipe<string>(string val) {
    return "String: \"" + val + "\" (panjang=" + to_string(val.length()) + ")";
}

// Spesialisasi untuk bool
template<>
string cetakTipe<bool>(bool val) {
    return string("Boolean: ") + (val ? "true" : "false");
}

cout << cetakTipe(42)         << "\n";   // Nilai: 42
cout << cetakTipe("halo"s)    << "\n";   // String: "halo" (panjang=4)
cout << cetakTipe(true)       << "\n";   // Boolean: true
```

---

## Non-type Template Parameters

```cpp
// Ukuran array sebagai template parameter
template<typename T, int N>
class ArrayTetap {
    T data[N];
public:
    ArrayTetap() { fill(data, data+N, T{}); }
    T& operator[](int i) { return data[i]; }
    int ukuran() const { return N; }
    void isi(T val) { fill(data, data+N, val); }
};

ArrayTetap<int, 5>     arr5;
ArrayTetap<double, 10> arr10;
ArrayTetap<char, 26>   alfabet;
```

---

## Variadic Template (C++11)

```cpp
// Fungsi dengan jumlah argumen tak terbatas
template<typename T>
void cetak(T val) {
    cout << val << "\n";  // base case
}

template<typename T, typename... Args>
void cetak(T val, Args... args) {
    cout << val << " ";
    cetak(args...);  // rekursif dengan sisa argumen
}

cetak(1, 2.5, "tiga", 'A', true);
// Output: 1 2.5 tiga A 1

// Jumlah argumen
template<typename... Args>
int hitungArg(Args...) {
    return sizeof...(Args);
}
cout << hitungArg(1, 2.0, "tiga") << "\n";  // 3
```

---

## Template dengan Constraints (C++20 — Concepts)

```cpp
#include <concepts>

// Concept: T harus bisa dijumlahkan
template<typename T>
concept Numerik = requires(T a, T b) {
    a + b;
    a - b;
    a * b;
};

template<Numerik T>
T jumlah(T a, T b) { return a + b; }

// Cara lama (C++17 — SFINAE)
#include <type_traits>
template<typename T, typename = enable_if_t<is_arithmetic_v<T>>>
T kali(T a, T b) { return a * b; }
```

---

## Template Metaprogramming Dasar

```cpp
// Faktorial saat kompilasi
template<int N>
struct Faktorial {
    static constexpr int nilai = N * Faktorial<N-1>::nilai;
};

template<>
struct Faktorial<0> {
    static constexpr int nilai = 1;
};

// Dihitung saat kompilasi!
static_assert(Faktorial<5>::nilai == 120);
cout << Faktorial<10>::nilai << "\n";  // 3628800

// Fibonacci saat kompilasi
template<int N>
struct Fib {
    static constexpr int nilai = Fib<N-1>::nilai + Fib<N-2>::nilai;
};
template<> struct Fib<0> { static constexpr int nilai = 0; };
template<> struct Fib<1> { static constexpr int nilai = 1; };
```

---

## `auto` dan Deduced Return Type (C++14)

```cpp
// Return type otomatis
template<typename T, typename U>
auto tambah(T a, U b) -> decltype(a + b) {  // C++11
    return a + b;
}

template<typename T, typename U>
auto kurang(T a, U b) {  // C++14 — lebih simple
    return a - b;
}

auto hasil = tambah(3, 4.5);  // double: 7.5
```

---

## Latihan

1. Buat class template `Pasangan<T, U>` yang menyimpan dua nilai berbeda tipe
2. Buat function template `urutkan(arr, n)` yang bisa mengurutkan array bertipe apapun
3. Buat class template `Matriks<T, R, C>` dengan operator + dan *
4. Implementasikan function template `hasilBagi(a, b)` yang melempar exception jika b == 0

---

**[← OOP Lanjut](../01-oop-lanjut/README.md)** | **[Berikutnya → STL](../03-stl/README.md)**
