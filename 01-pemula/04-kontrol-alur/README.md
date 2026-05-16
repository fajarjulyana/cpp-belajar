# Bab 4 — Kontrol Alur (Control Flow)

Kontrol alur mengatur **urutan eksekusi** kode berdasarkan kondisi tertentu.

---

## 1. Pernyataan `if`, `else if`, `else`

```cpp
int nilai = 78;

if (nilai >= 90) {
    cout << "Grade A" << endl;
} else if (nilai >= 80) {
    cout << "Grade B" << endl;
} else if (nilai >= 70) {
    cout << "Grade C" << endl;
} else if (nilai >= 60) {
    cout << "Grade D" << endl;
} else {
    cout << "Grade F — Tidak Lulus" << endl;
}
// Output: Grade C
```

### if Satu Baris (tanpa kurung kurawal)
```cpp
if (x > 0)
    cout << "positif" << endl;  // OK, tapi kurang aman
```
> ⚠️ Selalu pakai `{}` kurung kurawal untuk menghindari bug!

---

## 2. Pernyataan `switch`

Cocok untuk membandingkan satu variabel dengan banyak nilai.

```cpp
int hari = 3;
switch (hari) {
    case 1:
        cout << "Senin" << endl;
        break;
    case 2:
        cout << "Selasa" << endl;
        break;
    case 3:
        cout << "Rabu" << endl;
        break;
    case 4:
        cout << "Kamis" << endl;
        break;
    case 5:
        cout << "Jumat" << endl;
        break;
    case 6:
    case 7:
        cout << "Akhir Pekan" << endl;
        break;
    default:
        cout << "Hari tidak valid" << endl;
}
// Output: Rabu
```

> ⚠️ Jangan lupa `break`! Tanpa `break`, eksekusi akan "jatuh" ke case berikutnya (fall-through).

### switch dengan char dan string (C++17)
```cpp
char grade = 'B';
switch (grade) {
    case 'A': cout << "Sempurna!";  break;
    case 'B': cout << "Bagus!";     break;
    case 'C': cout << "Cukup";      break;
    default:  cout << "Perlu usaha lebih";
}
```

---

## 3. Perulangan `for`

```cpp
// Sintaks: for (inisialisasi; kondisi; update)
for (int i = 0; i < 5; i++) {
    cout << "Iterasi ke-" << i << endl;
}

// Hitung mundur
for (int i = 10; i >= 1; i--) {
    cout << i << " ";
}
cout << endl;  // Output: 10 9 8 7 6 5 4 3 2 1

// Langkah 2
for (int i = 0; i <= 20; i += 2) {
    cout << i << " ";
}  // Output: 0 2 4 6 8 10 12 14 16 18 20
```

### Range-based for (C++11)
```cpp
#include <vector>
vector<int> angka = {10, 20, 30, 40, 50};

for (int x : angka) {
    cout << x << " ";
}

// Dengan referensi (lebih efisien untuk objek besar)
for (const int& x : angka) {
    cout << x << " ";
}

// Dengan auto
for (auto x : angka) {
    cout << x << " ";
}
```

---

## 4. Perulangan `while`

```cpp
// Mengeksekusi SELAMA kondisi true
int i = 1;
while (i <= 5) {
    cout << "i = " << i << endl;
    i++;  // JANGAN LUPA! Tanpa ini → infinite loop
}

// Membaca input sampai kondisi terpenuhi
int angka;
cout << "Masukkan angka (0 untuk berhenti): ";
cin >> angka;
while (angka != 0) {
    cout << "Anda memasukkan: " << angka << endl;
    cout << "Masukkan angka berikutnya (0 untuk berhenti): ";
    cin >> angka;
}
cout << "Program selesai." << endl;
```

---

## 5. Perulangan `do-while`

Perbedaan dari `while`: kode dieksekusi **minimal satu kali** sebelum kondisi dicek.

```cpp
int i = 0;
do {
    cout << "Dieksekusi ke-" << (i+1) << endl;
    i++;
} while (i < 3);
// Output: Dieksekusi ke-1, ke-2, ke-3

// Contoh: validasi input
int pilihan;
do {
    cout << "Pilih menu (1-3): ";
    cin >> pilihan;
    if (pilihan < 1 || pilihan > 3)
        cout << "Pilihan tidak valid!\n";
} while (pilihan < 1 || pilihan > 3);
cout << "Anda memilih: " << pilihan << endl;
```

---

## 6. `break` dan `continue`

### `break` — keluar dari loop/switch
```cpp
for (int i = 0; i < 10; i++) {
    if (i == 5) break;  // keluar saat i==5
    cout << i << " ";
}
// Output: 0 1 2 3 4
```

### `continue` — lewati iterasi saat ini
```cpp
for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) continue;  // lewati bilangan genap
    cout << i << " ";
}
// Output: 1 3 5 7 9
```

---

## 7. Loop Bersarang (Nested Loop)

```cpp
// Tabel perkalian
for (int i = 1; i <= 5; i++) {
    for (int j = 1; j <= 5; j++) {
        cout << i * j << "\t";
    }
    cout << endl;
}

// Pola bintang
for (int i = 1; i <= 5; i++) {
    for (int j = 0; j < i; j++) {
        cout << "* ";
    }
    cout << endl;
}
// Output:
// *
// * *
// * * *
// * * * *
// * * * * *
```

---

## 8. `goto` (Jarang Dipakai)

```cpp
// goto tersedia di C++ tapi sangat tidak disarankan
// Penggunaan satu-satunya yang diterima: keluar dari nested loop

for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
        if (i == 2 && j == 2) goto selesai;
        cout << "(" << i << "," << j << ") ";
    }
}
selesai:
cout << "\nKeluar dari nested loop" << endl;
```

---

## 9. if-constexpr (C++17)

Evaluasi kondisi pada waktu kompilasi:

```cpp
template<typename T>
void cetak(T nilai) {
    if constexpr (std::is_integral_v<T>) {
        cout << "Integer: " << nilai << endl;
    } else if constexpr (std::is_floating_point_v<T>) {
        cout << "Float: " << nilai << endl;
    } else {
        cout << "Lainnya" << endl;
    }
}
```

---

## Latihan

1. Buat program yang mencetak bilangan prima antara 1 sampai 100
2. Buat program yang menghitung faktorial `n!` menggunakan loop
3. Buat pola segitiga bintang menggunakan nested loop
4. Buat kalkulator sederhana dengan menu menggunakan `switch`
5. Buat program yang menebak angka (1-100) dengan petunjuk "terlalu besar/kecil"

---

**[← Operator](../03-operator/README.md)** | **[Berikutnya → Fungsi](../05-fungsi/README.md)**
