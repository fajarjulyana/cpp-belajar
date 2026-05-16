# Bab 15 — Smart Pointer

Smart pointer adalah wrapper RAII di sekitar raw pointer yang **otomatis mengelola memori** — tidak perlu `delete` manual, tidak ada memory leak.

---

## `unique_ptr` — Kepemilikan Eksklusif

```cpp
#include <memory>

// Buat unique_ptr
unique_ptr<int> p1 = make_unique<int>(42);   // C++14, direkomendasikan
unique_ptr<int> p2(new int(10));              // C++11

cout << *p1 << "\n";   // 42
*p1 = 100;
cout << *p1 << "\n";   // 100

// Tidak bisa disalin — kepemilikan eksklusif
// unique_ptr<int> p3 = p1;   // ERROR: tidak bisa copy

// Bisa dipindahkan (move)
unique_ptr<int> p3 = move(p1);  // p1 sekarang null
cout << (p1 == nullptr) << "\n";  // true
cout << *p3 << "\n";               // 100

// Otomatis dibebaskan saat keluar scope — tidak perlu delete!
```

### unique_ptr untuk Array
```cpp
unique_ptr<int[]> arr = make_unique<int[]>(5);
for (int i = 0; i < 5; i++) arr[i] = (i+1) * 10;
```

### unique_ptr untuk Object
```cpp
struct Hewan {
    string nama;
    Hewan(string n) : nama(n) { cout << "Hewan(" << nama << ") dibuat\n"; }
    ~Hewan() { cout << "Hewan(" << nama << ") dihapus\n"; }
    virtual void bersuara() = 0;
};
struct Anjing : Hewan {
    Anjing(string n) : Hewan(n) {}
    void bersuara() override { cout << nama << ": Guk!\n"; }
};

{
    auto anjing = make_unique<Anjing>("Rex");
    anjing->bersuara();
}  // otomatis dihapus di sini
```

---

## `shared_ptr` — Kepemilikan Bersama

Beberapa shared_ptr bisa menunjuk ke objek yang sama. Objek dihapus saat **reference count = 0**.

```cpp
shared_ptr<int> sp1 = make_shared<int>(42);
cout << sp1.use_count() << "\n";  // 1

shared_ptr<int> sp2 = sp1;        // salin — keduanya menunjuk objek sama
cout << sp1.use_count() << "\n";  // 2
cout << sp2.use_count() << "\n";  // 2

{
    shared_ptr<int> sp3 = sp1;
    cout << sp1.use_count() << "\n";  // 3
}
// sp3 keluar scope — count turun jadi 2
cout << sp1.use_count() << "\n";  // 2

sp1.reset();  // sp1 melepas kepemilikan
cout << sp2.use_count() << "\n";  // 1
// Saat sp2 keluar scope, objek dihapus
```

---

## `weak_ptr` — Referensi Lemah

Tidak memengaruhi reference count. Digunakan untuk memutus **circular reference**.

```cpp
// Masalah: circular reference (memory leak)
struct Node {
    int nilai;
    shared_ptr<Node> berikut;    // strong reference
    // shared_ptr<Node> sebelum; // ini menyebabkan circular!
    weak_ptr<Node> sebelum;      // solusi: weak reference
    Node(int n) : nilai(n) {}
};

auto n1 = make_shared<Node>(1);
auto n2 = make_shared<Node>(2);
n1->berikut = n2;
n2->sebelum = n1;  // weak_ptr — tidak menambah ref count

// Gunakan weak_ptr
if (auto locked = n2->sebelum.lock()) {  // lock() → shared_ptr
    cout << "Node sebelum: " << locked->nilai << "\n";
}
```

---

## Perbandingan Smart Pointer

| | `unique_ptr` | `shared_ptr` | `weak_ptr` |
|---|---|---|---|
| Kepemilikan | Eksklusif | Bersama | Tidak punya |
| Reference count | Tidak | Ya | Tidak |
| Bisa disalin | Tidak | Ya | Ya |
| Bisa dipindah | Ya | Ya | Ya |
| Overhead | Minimal | Kecil | Kecil |
| Penggunaan | Default | Shared ownership | Break cycles |

---

## Custom Deleter

```cpp
// Deleter untuk file
auto fileDeleter = [](FILE* f) {
    if (f) { fclose(f); cout << "File ditutup\n"; }
};
unique_ptr<FILE, decltype(fileDeleter)> file(fopen("test.txt","w"), fileDeleter);

// Deleter untuk array C
auto arrDeleter = [](int* p) { delete[] p; };
unique_ptr<int, decltype(arrDeleter)> arr(new int[10], arrDeleter);
```

---

## `enable_shared_from_this`

```cpp
class Widget : public enable_shared_from_this<Widget> {
public:
    shared_ptr<Widget> getShared() {
        return shared_from_this();  // buat shared_ptr dari this
    }
};

auto w = make_shared<Widget>();
auto w2 = w->getShared();
cout << w.use_count() << "\n";  // 2
```

---

## Latihan

1. Buat linked list menggunakan `unique_ptr` untuk manajemen memori otomatis
2. Implementasikan pohon biner dengan `shared_ptr` untuk node
3. Demonstrasikan circular reference dan solusinya dengan `weak_ptr`
4. Buat factory function yang mengembalikan `unique_ptr`

---

**[← Exception Handling](../04-exception/README.md)** | **[Berikutnya → Lambda](../06-lambda/README.md)**
