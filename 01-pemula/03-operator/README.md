# Bab 3 — Operator

Operator adalah simbol yang melakukan operasi pada satu atau lebih **operand** (nilai/variabel).

---

## 1. Operator Aritmatika

| Operator | Nama | Contoh | Hasil |
|----------|------|--------|-------|
| `+` | Penjumlahan | `5 + 3` | `8` |
| `-` | Pengurangan | `5 - 3` | `2` |
| `*` | Perkalian | `5 * 3` | `15` |
| `/` | Pembagian | `10 / 3` | `3` (integer!) |
| `%` | Modulo/Sisa | `10 % 3` | `1` |

```cpp
int a = 10, b = 3;
cout << a + b << "\n";  // 13
cout << a - b << "\n";  // 7
cout << a * b << "\n";  // 30
cout << a / b << "\n";  // 3 (pembagian integer, bukan 3.33)
cout << a % b << "\n";  // 1 (sisa 10/3)

double x = 10.0, y = 3.0;
cout << x / y << "\n";  // 3.33333 (pembagian float)
```

> ⚠️ **Hati-hati:** `int / int` selalu menghasilkan `int` (terpotong, bukan dibulatkan)!

---

## 2. Operator Penugasan (Assignment)

| Operator | Contoh | Ekuivalen |
|----------|--------|-----------|
| `=` | `a = 5` | `a = 5` |
| `+=` | `a += 3` | `a = a + 3` |
| `-=` | `a -= 3` | `a = a - 3` |
| `*=` | `a *= 3` | `a = a * 3` |
| `/=` | `a /= 3` | `a = a / 3` |
| `%=` | `a %= 3` | `a = a % 3` |

```cpp
int n = 10;
n += 5;   // n = 15
n -= 3;   // n = 12
n *= 2;   // n = 24
n /= 4;   // n = 6
n %= 4;   // n = 2
```

---

## 3. Operator Perbandingan (Relasional)

Selalu menghasilkan `bool` (true/false).

| Operator | Arti | Contoh |
|----------|------|--------|
| `==` | Sama dengan | `5 == 5` → `true` |
| `!=` | Tidak sama | `5 != 3` → `true` |
| `<` | Kurang dari | `3 < 5` → `true` |
| `>` | Lebih dari | `5 > 3` → `true` |
| `<=` | Kurang dari/sama | `5 <= 5` → `true` |
| `>=` | Lebih dari/sama | `3 >= 5` → `false` |

```cpp
int x = 5, y = 3;
cout << boolalpha;
cout << (x == y) << "\n";  // false
cout << (x != y) << "\n";  // true
cout << (x > y)  << "\n";  // true
cout << (x <= y) << "\n";  // false
```

---

## 4. Operator Logika

| Operator | Nama | Contoh | Arti |
|----------|------|--------|------|
| `&&` | AND | `a && b` | true jika keduanya true |
| `\|\|` | OR | `a \|\| b` | true jika salah satu true |
| `!` | NOT | `!a` | kebalikan |

```cpp
bool hujan = true;
bool panas = false;

cout << (hujan && panas) << "\n";   // false
cout << (hujan || panas) << "\n";   // true
cout << (!hujan)         << "\n";   // false

// Contoh praktis
int umur = 20;
bool punyaSIM = true;
bool bolehMenyetir = (umur >= 17) && punyaSIM;  // true
```

**Tabel Kebenaran AND:**
| A | B | A && B |
|---|---|--------|
| T | T | T |
| T | F | F |
| F | T | F |
| F | F | F |

**Tabel Kebenaran OR:**
| A | B | A \|\| B |
|---|---|--------|
| T | T | T |
| T | F | T |
| F | T | T |
| F | F | F |

---

## 5. Operator Increment & Decrement

```cpp
int n = 5;

// Pre-increment: tambah dulu, lalu gunakan
cout << ++n << "\n";  // 6, n=6

// Post-increment: gunakan dulu, lalu tambah
cout << n++ << "\n";  // 6, n=7 setelah ini
cout << n   << "\n";  // 7

// Pre-decrement
cout << --n << "\n";  // 6

// Post-decrement
cout << n-- << "\n";  // 6, n=5 setelah ini
```

---

## 6. Operator Bitwise

Beroperasi langsung pada bit biner.

| Operator | Nama | Contoh |
|----------|------|--------|
| `&` | Bitwise AND | `5 & 3` → `1` |
| `\|` | Bitwise OR | `5 \| 3` → `7` |
| `^` | Bitwise XOR | `5 ^ 3` → `6` |
| `~` | Bitwise NOT | `~5` → `-6` |
| `<<` | Left shift | `5 << 1` → `10` |
| `>>` | Right shift | `5 >> 1` → `2` |

```cpp
int a = 5;   // biner: 0101
int b = 3;   // biner: 0011

cout << (a & b) << "\n";   // 0001 = 1
cout << (a | b) << "\n";   // 0111 = 7
cout << (a ^ b) << "\n";   // 0110 = 6
cout << (a << 1) << "\n";  // 1010 = 10 (kali 2)
cout << (a >> 1) << "\n";  // 0010 = 2  (bagi 2)
```

---

## 7. Operator Ternary (? :)

Bentuk ringkas dari `if-else`.

```cpp
// Sintaks: kondisi ? nilai_jika_true : nilai_jika_false
int a = 10, b = 5;
int maks = (a > b) ? a : b;  // 10
cout << "Maks: " << maks << "\n";

string status = (maks >= 10) ? "Lulus" : "Tidak Lulus";
cout << status << "\n";  // Lulus
```

---

## 8. Operator sizeof & typeid

```cpp
#include <typeinfo>

int x = 42;
cout << sizeof(x) << "\n";        // 4 (ukuran dalam byte)
cout << typeid(x).name() << "\n"; // "i" (int) — platform-dependent
```

---

## 9. Prioritas Operator (Precedence)

Dari tertinggi ke terendah (sebagian):

| Prioritas | Operator |
|-----------|----------|
| 1 (tertinggi) | `()`, `[]`, `->`, `.` |
| 2 | `++`, `--` (post), `~`, `!`, `sizeof` |
| 3 | `*`, `/`, `%` |
| 4 | `+`, `-` |
| 5 | `<<`, `>>` |
| 6 | `<`, `<=`, `>`, `>=` |
| 7 | `==`, `!=` |
| 8 | `&` |
| 9 | `^` |
| 10 | `\|` |
| 11 | `&&` |
| 12 | `\|\|` |
| 13 | `?:` |
| 14 (terendah) | `=`, `+=`, dll |

```cpp
int hasil = 2 + 3 * 4;    // 14 (bukan 20), * lebih tinggi
int hasil2 = (2 + 3) * 4; // 20 (pakai kurung)
bool cek = 5 > 3 && 2 < 4; // true, > dan < lebih tinggi dari &&
```

> **Tips:** Jika ragu soal prioritas, gunakan tanda kurung `()` untuk memperjelas!

---

## Latihan

1. Hitung luas dan keliling persegi panjang dengan panjang=12 dan lebar=7
2. Buat program yang menentukan bilangan genap/ganjil menggunakan operator `%`
3. Gunakan operator bitwise untuk mengecek apakah sebuah angka adalah kelipatan 2
4. Buat kalkulator sederhana dengan operator `+`, `-`, `*`, `/`

---

**[← Variabel & Tipe Data](../02-variabel-tipe-data/README.md)** | **[Berikutnya → Kontrol Alur](../04-kontrol-alur/README.md)**
