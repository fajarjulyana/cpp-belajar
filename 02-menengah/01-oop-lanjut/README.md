# Bab 11 — OOP Lanjut: Inheritance & Polymorphism

## Inheritance (Pewarisan)

Class anak (derived) mewarisi atribut dan method dari class induk (base).

```cpp
class Hewan {          // Base class
protected:
    string nama;
    int    umur;
public:
    Hewan(string n, int u) : nama(n), umur(u) {}
    virtual void bersuara() const {
        cout << nama << " mengeluarkan suara\n";
    }
    string getNama() const { return nama; }
};

class Anjing : public Hewan {   // Derived class
    string ras;
public:
    Anjing(string n, int u, string r) : Hewan(n, u), ras(r) {}
    void bersuara() const override {   // Override method
        cout << nama << " (Anjing/" << ras << "): Guk guk!\n";
    }
};

class Kucing : public Hewan {
public:
    Kucing(string n, int u) : Hewan(n, u) {}
    void bersuara() const override {
        cout << nama << " (Kucing): Meong!\n";
    }
};
```

---

## Polymorphism (Polimorfisme)

Satu interface, banyak implementasi. Dengan `virtual` dan pointer/referensi.

```cpp
// Runtime polymorphism menggunakan pointer ke base class
vector<Hewan*> kebun = {
    new Anjing("Rex", 3, "Labrador"),
    new Kucing("Mimi", 2),
    new Anjing("Buddy", 4, "Golden"),
};

for (Hewan* h : kebun) {
    h->bersuara();   // memanggil versi yang tepat!
}

// Pembersihan memori
for (Hewan* h : kebun) delete h;
```

**Output:**
```
Rex (Anjing/Labrador): Guk guk!
Mimi (Kucing): Meong!
Buddy (Anjing/Golden): Guk guk!
```

---

## Virtual Function & vTable

```cpp
class Bentuk {
public:
    virtual double luas()     const = 0;  // pure virtual
    virtual double keliling() const = 0;  // pure virtual
    virtual void tampilkan()  const {
        cout << "Luas=" << luas() << " Keliling=" << keliling() << "\n";
    }
    virtual ~Bentuk() {}  // PENTING: virtual destructor!
};

class Lingkaran : public Bentuk {
    double r;
public:
    Lingkaran(double r) : r(r) {}
    double luas()     const override { return 3.14159 * r * r; }
    double keliling() const override { return 2 * 3.14159 * r; }
};

class Persegi : public Bentuk {
    double s;
public:
    Persegi(double s) : s(s) {}
    double luas()     const override { return s * s; }
    double keliling() const override { return 4 * s; }
};
```

---

## Abstract Class (Kelas Abstrak)

Class yang memiliki minimal satu **pure virtual function** (`= 0`) tidak bisa di-instansiasi langsung.

```cpp
Bentuk* b = new Lingkaran(5.0);  // OK
// Bentuk* b = new Bentuk();     // ERROR: abstract class!
b->tampilkan();
delete b;
```

---

## Multiple Inheritance

```cpp
class Berenang {
public:
    void berenang() { cout << "Sedang berenang\n"; }
};

class Terbang {
public:
    void terbang() { cout << "Sedang terbang\n"; }
};

class Bebek : public Hewan, public Berenang, public Terbang {
public:
    Bebek(string n) : Hewan(n, 1) {}
    void bersuara() const override {
        cout << nama << ": Kwek kwek!\n";
    }
};

Bebek donald("Donald");
donald.bersuara();
donald.berenang();
donald.terbang();
```

### Diamond Problem & Virtual Inheritance

```cpp
class A { public: int x; };
class B : virtual public A {};  // virtual inheritance
class C : virtual public A {};
class D : public B, public C {};  // D hanya punya satu A::x

D obj;
obj.x = 10;  // OK, tidak ambigu
```

---

## `override` dan `final` (C++11)

```cpp
class Base {
public:
    virtual void foo() {}
    virtual void bar() final {}  // tidak bisa di-override lagi
};

class Derived : public Base {
public:
    void foo() override {}   // OK
    // void bar() override {}  // ERROR: bar() adalah final
};

class Leaf final : public Derived {  // class ini tidak bisa diturunkan lagi
};
// class Sub : public Leaf {};  // ERROR
```

---

## dynamic_cast & RTTI

```cpp
#include <typeinfo>

Hewan* h = new Anjing("Rex", 3, "Lab");

// dynamic_cast — aman, mengembalikan nullptr jika gagal
Anjing* a = dynamic_cast<Anjing*>(h);
if (a) {
    cout << "Ini adalah Anjing!\n";
} else {
    cout << "Bukan Anjing\n";
}

// typeid — informasi tipe runtime
cout << typeid(*h).name() << "\n";

delete h;
```

---

## Copy Constructor & Assignment Operator (Rule of 3/5)

```cpp
class Buffer {
    int* data;
    int  ukuran;
public:
    Buffer(int n) : ukuran(n), data(new int[n]) {}

    // Copy constructor (Rule of 3)
    Buffer(const Buffer& lain) : ukuran(lain.ukuran), data(new int[lain.ukuran]) {
        copy(lain.data, lain.data + ukuran, data);
    }

    // Copy assignment operator
    Buffer& operator=(const Buffer& lain) {
        if (this != &lain) {
            delete[] data;
            ukuran = lain.ukuran;
            data   = new int[ukuran];
            copy(lain.data, lain.data + ukuran, data);
        }
        return *this;
    }

    // Move constructor (Rule of 5, C++11)
    Buffer(Buffer&& lain) noexcept : data(lain.data), ukuran(lain.ukuran) {
        lain.data   = nullptr;
        lain.ukuran = 0;
    }

    ~Buffer() { delete[] data; }
};
```

---

## Latihan

1. Buat hirarki class `Karyawan` → `Manager` → `Direktur` dengan method `hitungGaji()`
2. Buat abstract class `Instrumen` dengan method `mainkan()` dan turunkan `Gitar`, `Piano`, `Drum`
3. Implementasikan class `Smart pointer` sederhana menggunakan destructor dan operator overloading

---

**[← OOP Dasar](../../01-pemula/10-oop-dasar/README.md)** | **[Berikutnya → Template](../02-template/README.md)**
