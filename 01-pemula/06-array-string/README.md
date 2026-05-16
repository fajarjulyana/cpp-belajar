# Bab 6 — Array & String

## Array

Array adalah kumpulan elemen **bertipe sama** yang disimpan dalam memori yang **berurutan** (contiguous).

---

### Array Satu Dimensi

```cpp
// Deklarasi dan inisialisasi
int angka[5] = {10, 20, 30, 40, 50};
double suhu[3] = {36.5, 37.0, 36.8};
char huruf[4]  = {'A', 'B', 'C', 'D'};

// Deklarasi tanpa ukuran (otomatis dihitung)
int primes[] = {2, 3, 5, 7, 11, 13};

// Akses elemen (indeks dimulai dari 0)
cout << angka[0] << endl;  // 10
cout << angka[4] << endl;  // 50
angka[2] = 35;             // ubah elemen ke-3

// Ukuran array
int ukuran = sizeof(angka) / sizeof(angka[0]);  // 5
```

### Iterasi Array
```cpp
// Cara 1: for biasa
for (int i = 0; i < 5; i++) {
    cout << angka[i] << " ";
}

// Cara 2: range-based for (C++11)
for (int x : angka) {
    cout << x << " ";
}

// Cara 3: pointer
for (int* p = angka; p < angka + 5; p++) {
    cout << *p << " ";
}
```

---

### Array Dua Dimensi (Matriks)

```cpp
int matriks[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

// Akses
cout << matriks[1][2] << endl;  // 7 (baris 1, kolom 2)

// Iterasi
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
        cout << matriks[i][j] << "\t";
    }
    cout << "\n";
}
```

---

### Array sebagai Parameter Fungsi

```cpp
// Cara 1: pakai pointer (array "decay" ke pointer)
void cetakArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
}

// Cara 2: pakai referensi (ukuran harus tetap)
template <int N>
void cetakArray(const int (&arr)[N]) {
    for (int x : arr) cout << x << " ";
}

// Cara 3: gunakan std::array atau std::vector (direkomendasikan)
```

---

## std::array (C++11) — Array Modern

```cpp
#include <array>

std::array<int, 5> arr = {1, 2, 3, 4, 5};
cout << arr.size()  << endl;  // 5
cout << arr.front() << endl;  // 1
cout << arr.back()  << endl;  // 5
arr.fill(0);                  // isi semua dengan 0

// Iterasi
for (int x : arr) cout << x << " ";
```

---

## std::vector — Array Dinamis

```cpp
#include <vector>

vector<int> v;
v.push_back(10);   // tambah elemen di belakang
v.push_back(20);
v.push_back(30);
v.pop_back();      // hapus elemen terakhir

cout << v.size()     << endl;  // 2
cout << v[0]         << endl;  // 10
cout << v.at(1)      << endl;  // 20 (lebih aman, ada bounds check)
cout << v.empty()    << endl;  // false

vector<int> v2 = {5, 3, 8, 1, 9};
v2.insert(v2.begin() + 2, 100);  // sisipkan 100 di posisi 2
v2.erase(v2.begin());            // hapus elemen pertama
```

---

## String

### C-Style String (char array) — Hindari jika bisa

```cpp
char nama[50] = "Budi Santoso";
cout << strlen(nama)  << endl;  // 12
strcpy(nama, "Ahmad");          // salin string
strcat(nama, " Subagyo");       // gabungkan string
strcmp("abc", "abc")            // 0 = sama
```

---

### std::string — String Modern C++

```cpp
#include <string>

string s1 = "Halo";
string s2 = " Dunia";
string s3 = s1 + s2;   // "Halo Dunia"

// Properti
cout << s3.length()     << endl;  // 10
cout << s3.size()       << endl;  // sama dengan length()
cout << s3.empty()      << endl;  // false
cout << s3.front()      << endl;  // 'H'
cout << s3.back()       << endl;  // 'a'

// Akses karakter
cout << s3[0]           << endl;  // 'H'
cout << s3.at(5)        << endl;  // 'D' (safe)

// Modifikasi
s3.append("!");
s3 += " Selamat Pagi";
s3.insert(4, ",");      // "Halo, Dunia!"
s3.erase(4, 1);         // hapus karakter ke-4 sejumlah 1

// Pencarian
size_t pos = s3.find("Dunia");
if (pos != string::npos) {
    cout << "Ditemukan di posisi: " << pos << endl;
}

// Substring
string sub = s3.substr(5, 5);  // mulai pos 5, panjang 5

// Konversi
string numStr = "42";
int num = stoi(numStr);      // string → int
double dbl = stod("3.14");   // string → double
string fromInt = to_string(100);  // int → string

// Compare
if (s1 == "Halo") cout << "sama!\n";
if (s1 < s2) cout << "s1 lebih kecil secara leksikografis\n";

// Uppercase/lowercase (manual)
#include <algorithm>
#include <cctype>
string upper = s1;
transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
```

---

### String Multiline & Raw String (C++11)

```cpp
// String literal multiline
string teks = "Baris 1\n"
              "Baris 2\n"
              "Baris 3\n";

// Raw string — backslash tidak diinterpretasi
string path = R"(C:\Users\Budi\Documents\file.txt)";
string regex = R"(\d{3}-\d{4})";
```

---

### stringstream — Parsing String

```cpp
#include <sstream>

// Pisahkan kata-kata
string kalimat = "Halo Dunia C++";
stringstream ss(kalimat);
string kata;
while (ss >> kata) {
    cout << kata << "\n";
}

// Konversi ke/dari string
stringstream ss2;
ss2 << "Nilai: " << 42 << ", Pi: " << 3.14;
string hasil = ss2.str();
```

---

## Latihan

1. Buat program yang mencari nilai maksimum dan minimum dari array 10 bilangan
2. Buat program yang membalik urutan elemen array
3. Implementasikan pencarian biner (binary search) pada array terurut
4. Buat program yang menghitung frekuensi setiap huruf dalam sebuah kalimat
5. Buat program yang memeriksa apakah dua string adalah anagram

---

**[← Fungsi](../05-fungsi/README.md)** | **[Berikutnya → Pointer](../07-pointer/README.md)**
