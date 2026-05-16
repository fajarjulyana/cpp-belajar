# Bab 10 — OOP Dasar (Class & Object)

**OOP (Object-Oriented Programming)** adalah paradigma pemrograman yang mengorganisasi kode ke dalam **objek** yang menggabungkan data (atribut) dan perilaku (method).

---

## Konsep Dasar OOP

| Konsep | Penjelasan |
|--------|-----------|
| **Encapsulation** | Menyembunyikan implementasi, mengekspos interface |
| **Inheritance** | Class anak mewarisi sifat class induk |
| **Polymorphism** | Satu interface, banyak implementasi |
| **Abstraction** | Menyembunyikan kompleksitas, tampilkan yang penting |

---

## Class dan Object

```cpp
// Definisi class
class Mobil {
// Data members (atribut)
private:               // hanya bisa diakses dari dalam class
    string merk;
    int    tahun;
    double kecepatan;

public:                // bisa diakses dari luar class
    // Constructor
    Mobil(string m, int t) : merk(m), tahun(t), kecepatan(0) {}

    // Method (member function)
    void tambahKecepatan(double v) { kecepatan += v; }
    void rem(double v) { kecepatan = max(0.0, kecepatan - v); }

    // Getter
    string getMerk()      const { return merk; }
    int    getTahun()     const { return tahun; }
    double getKecepatan() const { return kecepatan; }

    // Method tampilkan info
    void info() const {
        cout << merk << " (" << tahun << ") — "
             << kecepatan << " km/h\n";
    }
};

// Membuat objek
Mobil m1("Toyota", 2020);
Mobil m2("Honda",  2022);

m1.tambahKecepatan(60);
m1.tambahKecepatan(20);
m1.info();  // Toyota (2020) — 80 km/h
```

---

## Access Specifier

| Specifier | Akses dari dalam class | Akses dari luar class | Akses dari class turunan |
|-----------|----------------------|----------------------|--------------------------|
| `public` | ✓ | ✓ | ✓ |
| `protected` | ✓ | ✗ | ✓ |
| `private` | ✓ | ✗ | ✗ |

---

## Constructor & Destructor

```cpp
class BankAccount {
private:
    string pemilik;
    double saldo;

public:
    // Constructor default
    BankAccount() : pemilik("Anonim"), saldo(0) {
        cout << "Akun dibuat (default)\n";
    }

    // Constructor dengan parameter
    BankAccount(string nama, double saldoAwal)
        : pemilik(nama), saldo(saldoAwal) {
        cout << "Akun dibuat: " << pemilik << "\n";
    }

    // Copy constructor
    BankAccount(const BankAccount& lain)
        : pemilik(lain.pemilik + " (copy)"), saldo(lain.saldo) {}

    // Destructor
    ~BankAccount() {
        cout << "Akun dihapus: " << pemilik << "\n";
    }

    void setor(double nominal) {
        if (nominal > 0) saldo += nominal;
    }

    bool tarik(double nominal) {
        if (nominal > saldo) return false;
        saldo -= nominal;
        return true;
    }

    void tampilkan() const {
        cout << pemilik << ": Rp " << saldo << "\n";
    }
};
```

### Member Initializer List

```cpp
// Cara 1 (dalam body constructor)
Kelas(int a, int b) {
    x = a;  // ini adalah PENUGASAN, bukan inisialisasi
    y = b;
}

// Cara 2: Initializer list (DIREKOMENDASIKAN — lebih efisien)
Kelas(int a, int b) : x(a), y(b) {}
// Wajib dipakai untuk: const members, reference members, base class
```

---

## Getter & Setter

```cpp
class Mahasiswa {
private:
    string nama;
    double ipk;

public:
    // Getter (const karena tidak mengubah state)
    string getNama() const { return nama; }
    double getIpk()  const { return ipk; }

    // Setter (dengan validasi)
    void setNama(const string& n) {
        if (!n.empty()) nama = n;
    }

    void setIpk(double i) {
        if (i >= 0.0 && i <= 4.0) ipk = i;
        else cerr << "IPK tidak valid!\n";
    }
};
```

---

## Static Member

```cpp
class Counter {
private:
    static int jumlah;   // satu untuk semua instance
    int id;

public:
    Counter() : id(++jumlah) {}
    ~Counter() { --jumlah; }

    static int getJumlah() { return jumlah; }
    int getId() const { return id; }
};

int Counter::jumlah = 0;  // wajib didefinisikan di luar class

Counter c1, c2, c3;
cout << Counter::getJumlah() << "\n";  // 3
```

---

## `const` Method

```cpp
class Titik {
    double x, y;
public:
    Titik(double x, double y) : x(x), y(y) {}

    // const method — tidak boleh mengubah data member
    double getX() const { return x; }
    double getY() const { return y; }

    // Contoh const objek
    const Titik asal(0, 0);
    // asal.x = 1;      // ERROR: objek const
    double xAsal = asal.getX();  // OK: getX() adalah const method
};
```

---

## Operator Overloading

```cpp
class Vektor2D {
public:
    double x, y;
    Vektor2D(double x=0, double y=0) : x(x), y(y) {}

    Vektor2D operator+(const Vektor2D& v) const {
        return {x + v.x, y + v.y};
    }

    Vektor2D operator*(double skalar) const {
        return {x * skalar, y * skalar};
    }

    bool operator==(const Vektor2D& v) const {
        return x == v.x && y == v.y;
    }

    // Friend function untuk output
    friend ostream& operator<<(ostream& os, const Vektor2D& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};

Vektor2D v1(1, 2), v2(3, 4);
Vektor2D v3 = v1 + v2;
cout << v3 << endl;   // (4, 6)
cout << v3 * 2 << endl;  // (8, 12)
```

---

## Latihan

1. Buat class `Persegi` dengan method luas, keliling, dan tampilkan
2. Buat class `Tumpukan` (stack) dengan push, pop, peek, isEmpty
3. Buat class `KalkMatriks` yang mendukung penjumlahan dua matriks 2x2
4. Tambahkan operator overloading `+` dan `-` untuk class `Pecahan` (pembilang/penyebut)

---

**[← File I/O](../09-file-io/README.md)** | **[→ Lanjut ke Menengah: OOP Lanjut](../../02-menengah/01-oop-lanjut/README.md)**
