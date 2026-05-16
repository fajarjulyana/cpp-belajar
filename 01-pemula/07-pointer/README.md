# Bab 7 — Pointer & Referensi

Pointer adalah salah satu fitur paling kuat sekaligus paling membingungkan dalam C++. Memahami pointer adalah kunci untuk memahami manajemen memori.

---

## Memori dan Alamat

Setiap variabel di komputer disimpan di **lokasi memori** yang memiliki **alamat** (address). Pointer adalah variabel yang menyimpan **alamat memori** variabel lain.

```
Memori:
Alamat   Nilai
0x1000   42      ← variabel int x
0x1004   0x1000  ← pointer p yang menunjuk ke x
```

---

## Deklarasi & Penggunaan Pointer

```cpp
int x = 42;

int* p = &x;  // p menyimpan alamat x
//  ^    ^
// pointer  operator address-of

cout << x    << endl;  // 42  (nilai x)
cout << &x   << endl;  // 0x7fff... (alamat x)
cout << p    << endl;  // 0x7fff... (nilai pointer = alamat x)
cout << *p   << endl;  // 42  (dereferensi: nilai yang ditunjuk p)
//       ^
//   operator dereference

*p = 100;              // ubah nilai x via pointer
cout << x << endl;     // 100
```

---

## Pointer Null

```cpp
int* p1 = nullptr;   // C++11 — direkomendasikan
int* p2 = NULL;      // gaya lama
int* p3 = 0;         // juga valid, tapi hindari

// Selalu cek sebelum dereference!
if (p1 != nullptr) {
    cout << *p1 << endl;
}
```

> ⚠️ Mendereferensi pointer null = **undefined behavior** = crash!

---

## Pointer dan Array

Array C-style "meluruh" (decay) menjadi pointer ke elemen pertama.

```cpp
int arr[] = {10, 20, 30, 40, 50};
int* p = arr;     // p menunjuk ke arr[0]

cout << *p     << endl;  // 10
cout << *(p+1) << endl;  // 20 (pointer arithmetic)
cout << *(p+4) << endl;  // 50
cout << p[2]   << endl;  // 30 (notasi array pada pointer)

// Iterasi dengan pointer
for (int* ptr = arr; ptr < arr + 5; ptr++) {
    cout << *ptr << " ";
}
```

### Pointer Arithmetic
```cpp
int* p = arr;
p++;        // p sekarang menunjuk ke arr[1] (geser 4 byte = sizeof(int))
p += 2;     // p sekarang menunjuk ke arr[3]
int* q = arr + 4;
int jarak = q - p;  // 1 (selisih elemen, bukan byte)
```

---

## Pointer ke Pointer

```cpp
int x  = 10;
int*  p  = &x;     // pointer ke int
int** pp = &p;     // pointer ke pointer ke int

cout << x    << endl;  // 10
cout << *p   << endl;  // 10
cout << **pp << endl;  // 10

**pp = 99;
cout << x    << endl;  // 99
```

---

## Referensi (Reference)

Referensi adalah **alias** untuk variabel lain. Berbeda dari pointer:
- Tidak bisa null
- Tidak bisa diarahkan ulang setelah inisialisasi
- Sintaks lebih bersih

```cpp
int x = 10;
int& ref = x;  // ref adalah alias dari x

cout << x   << endl;  // 10
cout << ref << endl;  // 10

ref = 50;              // mengubah x!
cout << x   << endl;  // 50

// Keduanya menunjuk ke alamat yang sama
cout << (&x == &ref) << endl;  // true (boolalpha)
```

### Referensi vs Pointer

| Fitur | Pointer | Referensi |
|-------|---------|-----------|
| Nilai null | Bisa (`nullptr`) | Tidak bisa |
| Diarahkan ulang | Bisa | Tidak bisa |
| Sintaks dereferensi | `*p` | Langsung (`ref`) |
| Alamat elemen | Bisa (`p + 1`) | Tidak bisa |

---

## Manajemen Memori Dinamis

### `new` dan `delete`

```cpp
// Alokasi satu elemen
int* p = new int;        // alokasi di heap
*p = 42;
cout << *p << endl;       // 42
delete p;                 // WAJIB dibebaskan!
p = nullptr;              // hindari dangling pointer

// Alokasi dengan nilai awal
double* d = new double(3.14);
delete d;

// Alokasi array
int* arr = new int[10];  // array 10 int di heap
for (int i = 0; i < 10; i++) arr[i] = i * i;
delete[] arr;             // GUNAKAN delete[] untuk array!
arr = nullptr;
```

### Masalah Umum Memori

```cpp
// 1. Memory Leak — lupa delete
int* p = new int(10);
// ... lupa delete p → memori bocor!

// 2. Dangling Pointer — menggunakan pointer setelah delete
int* p2 = new int(5);
delete p2;
// cout << *p2;  // UNDEFINED BEHAVIOR!

// 3. Double Delete
delete p2;
// delete p2;  // UNDEFINED BEHAVIOR!

// 4. Buffer Overflow
int* arr2 = new int[5];
arr2[10] = 99;  // UNDEFINED BEHAVIOR — di luar batas!
delete[] arr2;
```

> 💡 **Solusi modern:** Gunakan **Smart Pointer** (`unique_ptr`, `shared_ptr`) — lihat Bab Menengah.

---

## Pointer ke Fungsi

```cpp
int tambah(int a, int b) { return a + b; }
int kurang(int a, int b) { return a - b; }

// Tipe pointer ke fungsi: int (*)(int, int)
int (*operasi)(int, int) = tambah;
cout << operasi(10, 3) << endl;  // 13

operasi = kurang;
cout << operasi(10, 3) << endl;  // 7

// Array of function pointers
int (*ops[])(int, int) = {tambah, kurang};
for (auto& op : ops) cout << op(6, 2) << " ";  // 8 4
```

---

## `const` dengan Pointer

```cpp
int x = 10, y = 20;

// Pointer ke const int (nilai tidak bisa diubah via pointer)
const int* p1 = &x;
// *p1 = 5;    // ERROR
p1 = &y;       // OK (pointer bisa diubah)

// Const pointer ke int (pointer tidak bisa diarahkan ulang)
int* const p2 = &x;
*p2 = 5;        // OK (nilai bisa diubah)
// p2 = &y;     // ERROR

// Const pointer ke const int (keduanya tidak bisa diubah)
const int* const p3 = &x;
// *p3 = 5;     // ERROR
// p3 = &y;     // ERROR
```

**Cara membaca:** Baca dari kanan ke kiri!
- `const int* p` → `p` adalah pointer ke `int` yang `const`
- `int* const p` → `p` adalah `const` pointer ke `int`

---

## `void*` Pointer Generik

```cpp
void* ptr;
int i = 42;
double d = 3.14;
string s = "halo";

ptr = &i;  // OK
ptr = &d;  // OK
ptr = &s;  // OK

// Harus di-cast sebelum dereference
int* ip = static_cast<int*>(ptr);
```

---

## Latihan

1. Buat program yang menukar dua variabel menggunakan pointer
2. Buat fungsi yang mengembalikan pointer ke elemen terbesar dalam array
3. Alokasikan matriks 2D secara dinamis menggunakan `new` dan bebaskan dengan `delete`
4. Buat program yang menyalin string menggunakan pointer

---

**[← Array & String](../06-array-string/README.md)** | **[Berikutnya → Struct](../08-struct/README.md)**
