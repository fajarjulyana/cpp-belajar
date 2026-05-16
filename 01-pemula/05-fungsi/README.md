# Bab 5 — Fungsi

Fungsi adalah blok kode yang dapat dipanggil berulang kali, membantu kode lebih terstruktur dan mudah dipelihara (prinsip **DRY: Don't Repeat Yourself**).

---

## Anatomi Fungsi

```cpp
// Deklarasi (prototype) — opsional jika definisi sebelum main()
int tambah(int a, int b);

// Definisi fungsi
int tambah(int a, int b) {
//  ^       ^    ^  ^
// tipe    nama  param1 param2
    return a + b;
}

int main() {
    int hasil = tambah(3, 5);  // pemanggilan
    cout << hasil << endl;     // 8
    return 0;
}
```

---

## Jenis Fungsi

### Fungsi Tanpa Return Value (void)
```cpp
void sapa(string nama) {
    cout << "Halo, " << nama << "!" << endl;
    // tidak perlu return
}
```

### Fungsi dengan Return Value
```cpp
double luasLingkaran(double r) {
    const double PI = 3.14159265;
    return PI * r * r;
}
```

### Fungsi Tanpa Parameter
```cpp
void tampilkanMenu() {
    cout << "1. Tambah Data\n";
    cout << "2. Hapus Data\n";
    cout << "3. Keluar\n";
}
```

### Fungsi dengan Nilai Default Parameter
```cpp
void cetak(string teks, int ulang = 1, char pemisah = '\n') {
    for (int i = 0; i < ulang; i++) {
        cout << teks << pemisah;
    }
}

int main() {
    cetak("Halo");           // ulang=1, pemisah='\n'
    cetak("Halo", 3);        // ulang=3, pemisah='\n'
    cetak("Halo", 3, ' ');   // ulang=3, pemisah=' '
}
```

> **Aturan:** Parameter dengan nilai default harus di paling kanan!

---

## Parameter: Pass by Value vs Pass by Reference

### Pass by Value (salinan)
```cpp
void tambahSatu(int x) {
    x++;  // HANYA mengubah salinan lokal
}

int n = 5;
tambahSatu(n);
cout << n << endl;  // tetap 5!
```

### Pass by Reference (referensi, tidak menyalin)
```cpp
void tambahSatu(int& x) {
    x++;  // mengubah variabel ASLI
}

int n = 5;
tambahSatu(n);
cout << n << endl;  // 6 ✓

// Return multiple values dengan referensi
void hitungLingkaran(double r, double& luas, double& keliling) {
    const double PI = 3.14159265;
    luas      = PI * r * r;
    keliling  = 2 * PI * r;
}
```

### Pass by Const Reference (efisien, tidak berubah)
```cpp
void cetakPanjang(const string& s) {
    // s tidak bisa diubah, tapi tidak disalin (efisien)
    cout << "Panjang: " << s.length() << endl;
}
```

### Pass by Pointer
```cpp
void ganda(int* ptr) {
    *ptr = (*ptr) * 2;
}

int n = 10;
ganda(&n);
cout << n << endl;  // 20
```

---

## Function Overloading

Fungsi dengan nama sama tapi parameter berbeda.

```cpp
int    maks(int a, int b)       { return a > b ? a : b; }
double maks(double a, double b) { return a > b ? a : b; }
int    maks(int a, int b, int c){ return maks(maks(a, b), c); }

cout << maks(3, 7)        << endl;  // 7 (int versi)
cout << maks(3.5, 2.1)    << endl;  // 3.5 (double versi)
cout << maks(1, 5, 3)     << endl;  // 5 (3 parameter)
```

---

## Rekursi

Fungsi yang memanggil dirinya sendiri.

```cpp
// Faktorial: n! = n * (n-1)!
long long faktorial(int n) {
    if (n <= 1) return 1;        // base case
    return n * faktorial(n - 1); // recursive case
}

// Fibonacci
long long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

// Contoh rekursi yang bagus: Menara Hanoi
void hanoi(int n, char asal, char tujuan, char bantu) {
    if (n == 1) {
        cout << "Pindahkan disk 1 dari " << asal << " ke " << tujuan << "\n";
        return;
    }
    hanoi(n-1, asal, bantu, tujuan);
    cout << "Pindahkan disk " << n << " dari " << asal << " ke " << tujuan << "\n";
    hanoi(n-1, bantu, tujuan, asal);
}
```

> ⚠️ **Rekursi harus memiliki base case**, jika tidak → infinite recursion → stack overflow!

---

## Fungsi Inline

Saran ke compiler untuk menyalin kode fungsi ke tempat pemanggilan (menghindari overhead pemanggilan).

```cpp
inline int kuadrat(int x) { return x * x; }

// Cocok untuk fungsi sangat pendek yang dipanggil banyak
cout << kuadrat(5) << endl;  // 25
```

---

## Lambda (Fungsi Anonim) — C++11

```cpp
// Sintaks: [capture](parameter) -> return_type { body }
auto tambah = [](int a, int b) -> int { return a + b; };
cout << tambah(3, 5) << endl;  // 8

// Tanpa return type (auto-deduced)
auto sapa = [](string nama) { cout << "Halo " << nama << "\n"; };
sapa("Budi");

// Capture variabel luar
int x = 10;
auto tambahX = [x](int a) { return a + x; };  // [x] = capture by value
cout << tambahX(5) << endl;  // 15
```

---

## Pointer ke Fungsi

```cpp
int tambah(int a, int b) { return a + b; }
int kurang(int a, int b) { return a - b; }

// Pointer ke fungsi
int (*operasi)(int, int);

operasi = tambah;
cout << operasi(10, 3) << endl;  // 13

operasi = kurang;
cout << operasi(10, 3) << endl;  // 7
```

---

## Latihan

1. Buat fungsi `isPalindrome(string s)` yang memeriksa apakah sebuah string adalah palindrom
2. Buat fungsi rekursif `pangkat(int basis, int eksponen)` untuk menghitung basis^eksponen
3. Implementasikan bubble sort sebagai fungsi `void urutkan(int arr[], int n)`
4. Buat kalkulator dengan fungsi terpisah untuk setiap operasi

---

**[← Kontrol Alur](../04-kontrol-alur/README.md)** | **[Berikutnya → Array & String](../06-array-string/README.md)**
