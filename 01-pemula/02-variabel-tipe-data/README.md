# Bab 2 — Variabel & Tipe Data

## Apa itu Variabel?

Variabel adalah **wadah bernama** di dalam memori yang menyimpan nilai. Setiap variabel memiliki:
- **Nama** (identifier) — digunakan untuk mengaksesnya
- **Tipe data** — menentukan jenis nilai yang bisa disimpan
- **Nilai** — data yang tersimpan

```cpp
int umur = 25;       // tipe: int, nama: umur, nilai: 25
double tinggi = 170.5;  // tipe: double, nama: tinggi, nilai: 170.5
```

---

## Tipe Data Fundamental

### Tipe Integer (Bilangan Bulat)

| Tipe | Ukuran | Rentang Nilai |
|------|--------|---------------|
| `short` | 2 byte | -32,768 s/d 32,767 |
| `int` | 4 byte | -2,147,483,648 s/d 2,147,483,647 |
| `long` | 4/8 byte | Bergantung platform |
| `long long` | 8 byte | -9.2×10¹⁸ s/d 9.2×10¹⁸ |
| `unsigned int` | 4 byte | 0 s/d 4,294,967,295 |

```cpp
short  s = 100;
int    i = 42000;
long   l = 1000000L;
long long ll = 9000000000LL;
unsigned int ui = 4000000000U;
```

### Tipe Floating Point (Bilangan Desimal)

| Tipe | Ukuran | Presisi |
|------|--------|---------|
| `float` | 4 byte | ~7 digit desimal |
| `double` | 8 byte | ~15 digit desimal |
| `long double` | 12/16 byte | ~18-19 digit desimal |

```cpp
float  f  = 3.14f;
double d  = 3.14159265358979;
long double ld = 3.141592653589793238L;
```

### Tipe Karakter

```cpp
char c = 'A';          // 1 byte, satu karakter
char c2 = 65;          // sama dengan 'A' (kode ASCII)
wchar_t wc = L'Ω';    // karakter lebar (Unicode)
char16_t c16 = u'あ'; // UTF-16
char32_t c32 = U'𝄞';  // UTF-32
```

### Tipe Boolean

```cpp
bool benar = true;
bool salah = false;
bool hasil = (5 > 3);  // true
```

### Tipe String (bukan fundamental, tapi umum dipakai)

```cpp
#include <string>
std::string nama = "Budi Santoso";
std::string kosong = "";
```

---

## Deklarasi & Inisialisasi

```cpp
// Deklarasi tanpa inisialisasi (nilai tidak terdefinisi!)
int x;          // BAHAYA: nilai garbage

// Inisialisasi langsung
int a = 10;          // gaya C lama
int b(20);           // gaya C++ (constructor)
int c{30};           // gaya C++11 (uniform initialization, DIREKOMENDASIKAN)
int d = {40};        // gaya C++11 dengan tanda =

// Auto — compiler menentukan tipe
auto angka = 42;        // int
auto pi = 3.14;         // double
auto teks = "halo";     // const char*
auto nama = std::string("Budi");  // std::string
```

> **Rekomendasi:** Selalu inisialisasi variabel saat deklarasi!

---

## Konstanta

```cpp
// const — tidak bisa diubah setelah inisialisasi
const double PI = 3.14159265358979;
const int MAKS = 100;

// constexpr — dievaluasi saat kompilasi (lebih efisien)
constexpr int UKURAN = 256;
constexpr double GRAVITASI = 9.81;

// #define — makro preprocessor (hindari, gunakan const)
#define NAMA "Indonesia"  // tidak type-safe
```

---

## Konversi Tipe (Type Casting)

### Konversi Implisit (otomatis)

```cpp
int    i = 42;
double d = i;   // int → double, aman (widening)
int    x = 3.9; // double → int, truncation! x = 3 (bukan 4)
```

### Konversi Eksplisit (casting)

```cpp
// C-style cast (hindari)
int i = (int)3.7;  // 3

// C++ static_cast (direkomendasikan)
int j = static_cast<int>(3.7);    // 3
double k = static_cast<double>(5) / 2;  // 2.5, bukan 2!

// dynamic_cast — untuk pointer/referensi class (OOP)
// reinterpret_cast — reinterpretasi bit (low-level)
// const_cast — hapus modifier const
```

---

## sizeof & Ukuran Tipe

```cpp
#include <iostream>
using namespace std;

int main() {
    cout << "Ukuran tipe data:\n";
    cout << "char:      " << sizeof(char)      << " byte\n";
    cout << "short:     " << sizeof(short)     << " byte\n";
    cout << "int:       " << sizeof(int)       << " byte\n";
    cout << "long:      " << sizeof(long)      << " byte\n";
    cout << "long long: " << sizeof(long long) << " byte\n";
    cout << "float:     " << sizeof(float)     << " byte\n";
    cout << "double:    " << sizeof(double)    << " byte\n";
    cout << "bool:      " << sizeof(bool)      << " byte\n";
    return 0;
}
```

---

## Batas Nilai Tipe (limits)

```cpp
#include <climits>    // untuk integer
#include <cfloat>     // untuk float/double
#include <limits>     // C++ style (direkomendasikan)

cout << INT_MAX    << "\n";  // 2147483647
cout << INT_MIN    << "\n";  // -2147483648
cout << DBL_MAX    << "\n";  // 1.79769e+308

// C++ style
cout << std::numeric_limits<int>::max()    << "\n";
cout << std::numeric_limits<double>::min() << "\n";
```

---

## Tipe Data Tambahan (C++11 dan seterusnya)

```cpp
#include <cstdint>  // fixed-width integers

int8_t   a = 127;          // tepat 8 bit signed
uint8_t  b = 255;          // tepat 8 bit unsigned
int16_t  c = 32767;        // tepat 16 bit
int32_t  d = 2147483647;   // tepat 32 bit
int64_t  e = 9223372036854775807LL;  // tepat 64 bit
```

> **Kapan pakai?** Saat ukuran tipe HARUS konsisten lintas platform (misal: protokol jaringan, file format).

---

## Scope (Ruang Lingkup) Variabel

```cpp
int globalVar = 100;  // variabel global — bisa diakses dari mana saja

void fungsi() {
    int lokal = 10;   // hanya ada di dalam fungsi ini
    
    {
        int blok = 5; // hanya ada di dalam blok {} ini
        cout << blok << "\n";  // OK
    }
    // cout << blok;  // ERROR: blok sudah habis scope-nya
    
    cout << lokal    << "\n";  // OK
    cout << globalVar << "\n"; // OK
}
```

---

## Latihan

1. Buat program yang mendeklarasikan semua tipe data dan mencetak nilainya
2. Buat program yang menghitung luas lingkaran (π × r²) menggunakan `const double PI`
3. Praktikkan konversi tipe dan amati hasil yang berbeda
4. Gunakan `sizeof` untuk mencetak ukuran semua tipe di komputer Anda

---

**[← Kembali: Pengenalan](../01-pengenalan/README.md)** | **[Berikutnya → Operator](../03-operator/README.md)**
