# Bab 14 — Exception Handling

Exception handling memungkinkan program menangani **kondisi error secara elegan** tanpa menghentikan program secara tiba-tiba.

---

## Mekanisme `try`, `throw`, `catch`

```cpp
try {
    // Kode yang mungkin melempar exception
    int a = 10, b = 0;
    if (b == 0) throw runtime_error("Pembagian dengan nol!");
    cout << a / b << "\n";
} catch (const runtime_error& e) {
    cerr << "Error: " << e.what() << "\n";
} catch (...) {
    cerr << "Error tidak dikenal!\n";
}
```

---

## Hierarki Exception Standar

```
std::exception
├── std::logic_error
│   ├── std::invalid_argument
│   ├── std::domain_error
│   ├── std::length_error
│   └── std::out_of_range
└── std::runtime_error
    ├── std::range_error
    ├── std::overflow_error
    └── std::underflow_error
```

```cpp
#include <stdexcept>

try {
    vector<int> v = {1, 2, 3};
    cout << v.at(10);  // melempar std::out_of_range
} catch (const out_of_range& e) {
    cerr << "Out of range: " << e.what() << "\n";
} catch (const logic_error& e) {
    cerr << "Logic error: " << e.what() << "\n";
} catch (const exception& e) {
    cerr << "Exception: " << e.what() << "\n";
}
```

---

## Custom Exception

```cpp
// Custom exception class
class DivisiNolException : public runtime_error {
    int pembilang;
public:
    DivisiNolException(int p)
        : runtime_error("Pembagian dengan nol"),
          pembilang(p) {}

    int getPembilang() const { return pembilang; }
    const char* what() const noexcept override {
        return "DivisiNolException: tidak boleh membagi dengan nol";
    }
};

class SaldoTidakCukup : public runtime_error {
    double saldo, nominal;
public:
    SaldoTidakCukup(double s, double n)
        : runtime_error("Saldo tidak cukup"),
          saldo(s), nominal(n) {}

    string pesan() const {
        return "Saldo: " + to_string(saldo) +
               ", Dibutuhkan: " + to_string(nominal);
    }
};
```

---

## `noexcept`

```cpp
// Deklarasi fungsi tidak melempar exception
double akarKuadrat(double x) noexcept {
    if (x < 0) return -1.0;  // tidak throw
    return sqrt(x);
}

// noexcept kondisional
template<typename T>
void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T>) {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}
```

---

## RAII & Exception Safety

```cpp
// RAII — Resource Acquisition Is Initialization
// Pastikan resource dibebaskan meski terjadi exception

class FilePenjaga {
    FILE* fp;
public:
    FilePenjaga(const char* nama) : fp(fopen(nama, "r")) {
        if (!fp) throw runtime_error("Gagal buka file");
    }
    ~FilePenjaga() {
        if (fp) fclose(fp);  // otomatis ditutup
    }
    FILE* get() { return fp; }
};

void bacaFile(const char* nama) {
    FilePenjaga penjaga(nama);  // otomatis tutup saat keluar scope
    // ... baca file ...
}
// Tidak perlu khawatir lupa fclose()!
```

---

## Re-throw Exception

```cpp
void prosesData(const string& data) {
    try {
        if (data.empty()) throw invalid_argument("Data kosong");
        cout << "Memproses: " << data << "\n";
    } catch (const invalid_argument& e) {
        cerr << "Peringatan: " << e.what() << "\n";
        throw;  // lempar ulang exception yang sama
    }
}

try {
    prosesData("");
} catch (const invalid_argument& e) {
    cerr << "Ditangkap di level atas: " << e.what() << "\n";
}
```

---

## Exception dalam Constructor

```cpp
class Koneksi {
    string host;
    int port;
public:
    Koneksi(const string& h, int p) : host(h), port(p) {
        if (h.empty()) throw invalid_argument("Host tidak boleh kosong");
        if (p <= 0 || p > 65535) throw out_of_range("Port tidak valid");
        cout << "Terhubung ke " << h << ":" << p << "\n";
    }
};

try {
    Koneksi c("localhost", 8080);   // OK
    Koneksi c2("", 80);             // throws invalid_argument
} catch (const exception& e) {
    cerr << e.what() << "\n";
}
```

---

## Latihan

1. Buat class `Antrian` yang melempar exception saat queue penuh atau kosong
2. Buat fungsi `bagi(a, b)` dengan exception untuk pembagian nol
3. Implementasikan kalkulator yang menangani semua kemungkinan error input
4. Buat hierarki exception untuk sistem perbankan (saldo kurang, akun terkunci, dll)

---

**[← STL](../03-stl/README.md)** | **[Berikutnya → Smart Pointer](../05-smart-pointer/README.md)**
