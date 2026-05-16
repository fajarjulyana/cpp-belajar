# Bab 8 — Struct

`struct` (structure) adalah tipe data yang mengelompokkan beberapa variabel dengan tipe berbeda di bawah satu nama.

---

## Deklarasi Struct

```cpp
// Deklarasi struct
struct Mahasiswa {
    string nama;
    int    nim;
    double ipk;
    bool   aktif;
};

// Membuat instance
Mahasiswa mhs1;
mhs1.nama  = "Budi Santoso";
mhs1.nim   = 20210001;
mhs1.ipk   = 3.75;
mhs1.aktif = true;

// Inisialisasi langsung (C++11)
Mahasiswa mhs2 = {"Ani Rahayu", 20210002, 3.90, true};
Mahasiswa mhs3 {"Candra", 20210003, 3.50, true};  // tanpa =
```

---

## Mengakses Anggota

```cpp
// Operator titik (.) untuk variabel
cout << mhs1.nama << " - IPK: " << mhs1.ipk << endl;

// Operator panah (->) untuk pointer ke struct
Mahasiswa* ptr = &mhs1;
cout << ptr->nama << " - IPK: " << ptr->ipk << endl;
// ptr->nama setara dengan (*ptr).nama
```

---

## Struct di dalam Fungsi

```cpp
// Parameter struct (pass by value — membuat salinan)
void tampilkan(Mahasiswa m) {
    cout << "Nama: " << m.nama << ", NIM: " << m.nim
         << ", IPK: " << m.ipk << endl;
}

// Parameter const reference (efisien, tidak menyalin)
void tampilkanRef(const Mahasiswa& m) {
    cout << "Nama: " << m.nama << ", IPK: " << m.ipk << endl;
}

// Return struct
Mahasiswa buatMahasiswa(string nama, int nim, double ipk) {
    Mahasiswa m;
    m.nama = nama;
    m.nim  = nim;
    m.ipk  = ipk;
    m.aktif = true;
    return m;
}
```

---

## Struct Bersarang

```cpp
struct Tanggal {
    int hari, bulan, tahun;
};

struct Pegawai {
    string  nama;
    int     id;
    Tanggal tanggalMasuk;   // struct bersarang
    double  gaji;
};

Pegawai peg;
peg.nama = "Dewi Lestari";
peg.tanggalMasuk.hari  = 15;
peg.tanggalMasuk.bulan = 6;
peg.tanggalMasuk.tahun = 2020;
```

---

## Array of Struct

```cpp
const int MAKS = 5;
Mahasiswa kelas[MAKS] = {
    {"Alfa",   1001, 3.8, true},
    {"Beta",   1002, 3.5, true},
    {"Gamma",  1003, 3.9, false},
    {"Delta",  1004, 3.2, true},
    {"Epsilon",1005, 3.7, true}
};

// Cari IPK tertinggi
Mahasiswa* terbaik = &kelas[0];
for (int i = 1; i < MAKS; i++) {
    if (kelas[i].ipk > terbaik->ipk)
        terbaik = &kelas[i];
}
cout << "Terbaik: " << terbaik->nama << " - " << terbaik->ipk << endl;
```

---

## Struct dengan Method (Menuju Class)

```cpp
struct Lingkaran {
    double radius;

    // Method di dalam struct
    double luas() const {
        return 3.14159 * radius * radius;
    }

    double keliling() const {
        return 2 * 3.14159 * radius;
    }

    void tampilkan() const {
        cout << "r=" << radius
             << " luas=" << luas()
             << " keliling=" << keliling() << endl;
    }
};

Lingkaran l;
l.radius = 7.0;
l.tampilkan();
```

---

## `typedef` dan `using` dengan Struct

```cpp
// Cara lama dengan typedef
typedef struct { int x, y; } Titik;

// Cara modern dengan using (C++11)
using Koordinat = struct { int x, y; };

Titik p1 = {3, 4};
```

---

## Struct vs Class

| Fitur | struct | class |
|-------|--------|-------|
| Default access | `public` | `private` |
| Inheritance default | `public` | `private` |
| Konvensi penggunaan | Data sederhana | Objek kompleks |

> Di C++, `struct` dan `class` hampir identik — perbedaannya hanya default access level.

---

## Latihan

1. Buat struct `Buku` dengan field: judul, pengarang, tahun, harga. Buat array 5 buku dan urutkan berdasarkan harga
2. Buat struct `Titik` dengan x,y. Buat fungsi untuk menghitung jarak antara dua titik
3. Buat struct `Tumpukan` (stack) sederhana dengan array dan operasi push/pop

---

**[← Array & String](../06-array-string/README.md)** | **[Berikutnya → File I/O](../09-file-io/README.md)**
