# Bab 19 — Design Patterns

Design Patterns adalah solusi yang sudah terbukti untuk masalah desain perangkat lunak yang sering muncul.

---

## Kategori Pattern

| Kategori | Pattern |
|----------|---------|
| **Creational** | Singleton, Factory, Abstract Factory, Builder, Prototype |
| **Structural** | Adapter, Bridge, Composite, Decorator, Facade, Proxy |
| **Behavioral** | Observer, Strategy, Command, Iterator, State, Template Method |

---

## 1. Singleton — Satu Instance

```cpp
class Konfigurasi {
    static Konfigurasi* instance;
    map<string, string> data;
    Konfigurasi() {}                              // private constructor
    Konfigurasi(const Konfigurasi&) = delete;     // no copy
    Konfigurasi& operator=(const Konfigurasi&) = delete;
public:
    static Konfigurasi& getInstance() {
        static Konfigurasi inst;  // thread-safe sejak C++11
        return inst;
    }
    void set(const string& k, const string& v) { data[k] = v; }
    string get(const string& k) const {
        auto it = data.find(k);
        return it != data.end() ? it->second : "";
    }
};

auto& cfg = Konfigurasi::getInstance();
cfg.set("db_host", "localhost");
cout << cfg.get("db_host") << "\n";
```

---

## 2. Factory Method — Pembuatan Objek via Interface

```cpp
class Tombol {
public:
    virtual void render() = 0;
    virtual void onClick() = 0;
    virtual ~Tombol() = default;
};

class TombolWindows : public Tombol {
public:
    void render()  override { cout << "[Render Windows Button]\n"; }
    void onClick() override { cout << "[Windows Click]\n"; }
};

class TombolLinux : public Tombol {
public:
    void render()  override { cout << "[Render Linux Button]\n"; }
    void onClick() override { cout << "[Linux Click]\n"; }
};

class UIFactory {
public:
    virtual unique_ptr<Tombol> buatTombol() = 0;
    virtual ~UIFactory() = default;
};

class WindowsFactory : public UIFactory {
public:
    unique_ptr<Tombol> buatTombol() override {
        return make_unique<TombolWindows>();
    }
};

class LinuxFactory : public UIFactory {
public:
    unique_ptr<Tombol> buatTombol() override {
        return make_unique<TombolLinux>();
    }
};
```

---

## 3. Observer — Event/Notification System

```cpp
class Pengamat {
public:
    virtual void update(const string& acara, const string& data) = 0;
    virtual ~Pengamat() = default;
};

class Subjek {
    vector<shared_ptr<Pengamat>> pengamat;
protected:
    void notifikasi(const string& acara, const string& data) {
        for (auto& p : pengamat) p->update(acara, data);
    }
public:
    void daftar(shared_ptr<Pengamat> p)   { pengamat.push_back(p); }
    void hapusDaftar(shared_ptr<Pengamat> p) {
        pengamat.erase(remove(pengamat.begin(), pengamat.end(), p), pengamat.end());
    }
};

class StoreHarga : public Subjek {
    double harga;
public:
    void setHarga(double h) {
        harga = h;
        notifikasi("harga_berubah", to_string(h));
    }
};

class AlertHarga : public Pengamat {
    string nama;
    double batasAtas, batasBawah;
public:
    AlertHarga(string n, double atas, double bawah)
        : nama(n), batasAtas(atas), batasBawah(bawah) {}
    void update(const string&, const string& data) override {
        double harga = stod(data);
        if (harga > batasAtas)  cout << nama << ": JUAL! Harga=" << harga << "\n";
        if (harga < batasBawah) cout << nama << ": BELI! Harga=" << harga << "\n";
    }
};
```

---

## 4. Strategy — Algoritma yang Dapat Ditukar

```cpp
class StrategiSorting {
public:
    virtual void urutkan(vector<int>& data) = 0;
    virtual string nama() const = 0;
    virtual ~StrategiSorting() = default;
};

class BubbleSort : public StrategiSorting {
public:
    void urutkan(vector<int>& d) override {
        for (int i = 0; i < (int)d.size()-1; i++)
            for (int j = 0; j < (int)d.size()-i-1; j++)
                if (d[j] > d[j+1]) swap(d[j], d[j+1]);
    }
    string nama() const override { return "BubbleSort"; }
};

class QuickSort : public StrategiSorting {
    void qs(vector<int>& d, int lo, int hi) {
        if (lo >= hi) return;
        int pivot = d[hi], i = lo-1;
        for (int j = lo; j < hi; j++)
            if (d[j] <= pivot) swap(d[++i], d[j]);
        swap(d[i+1], d[hi]);
        qs(d, lo, i); qs(d, i+2, hi);
    }
public:
    void urutkan(vector<int>& d) override { qs(d, 0, (int)d.size()-1); }
    string nama() const override { return "QuickSort"; }
};

class Sorter {
    unique_ptr<StrategiSorting> strategi;
public:
    void setStrategi(unique_ptr<StrategiSorting> s) { strategi = move(s); }
    void urutkan(vector<int>& data) {
        cout << "Menggunakan " << strategi->nama() << "\n";
        strategi->urutkan(data);
    }
};
```

---

## 5. Command — Enkapsulasi Perintah + Undo

```cpp
class Perintah {
public:
    virtual void lakukan()   = 0;
    virtual void batalkan()  = 0;
    virtual ~Perintah() = default;
};

class Editor {
    string teks;
public:
    void tambah(const string& s) { teks += s; }
    void hapusAkhir(int n)       { if (n <= (int)teks.size()) teks.erase(teks.size()-n); }
    string getTeks() const       { return teks; }
};

class PerintahTambah : public Perintah {
    Editor& editor;
    string teks;
public:
    PerintahTambah(Editor& e, string t) : editor(e), teks(t) {}
    void lakukan()  override { editor.tambah(teks); }
    void batalkan() override { editor.hapusAkhir((int)teks.size()); }
};

class Invoker {
    vector<unique_ptr<Perintah>> riwayat;
public:
    void jalankan(unique_ptr<Perintah> p) {
        p->lakukan();
        riwayat.push_back(move(p));
    }
    void undo() {
        if (!riwayat.empty()) {
            riwayat.back()->batalkan();
            riwayat.pop_back();
        }
    }
};
```

---

## 6. Decorator — Tambah Perilaku Dinamis

```cpp
class Kopi {
public:
    virtual double harga() const = 0;
    virtual string deskripsi() const = 0;
    virtual ~Kopi() = default;
};

class KopiPolos : public Kopi {
public:
    double harga()     const override { return 15000; }
    string deskripsi() const override { return "Kopi Polos"; }
};

class DekoratorKopi : public Kopi {
protected:
    unique_ptr<Kopi> kopi;
public:
    DekoratorKopi(unique_ptr<Kopi> k) : kopi(move(k)) {}
};

class TambahSusu : public DekoratorKopi {
public:
    using DekoratorKopi::DekoratorKopi;
    double harga()     const override { return kopi->harga() + 5000; }
    string deskripsi() const override { return kopi->deskripsi() + " + Susu"; }
};

class TambahGula : public DekoratorKopi {
public:
    using DekoratorKopi::DekoratorKopi;
    double harga()     const override { return kopi->harga() + 2000; }
    string deskripsi() const override { return kopi->deskripsi() + " + Gula"; }
};
```

---

## Latihan

1. Implementasikan **Builder Pattern** untuk membangun objek `Mobil` (merek, warna, tahun, fitur)
2. Implementasikan **Proxy Pattern** untuk lazy loading gambar
3. Buat **State Machine** sederhana untuk traffic light menggunakan State Pattern
4. Implementasikan **Iterator Pattern** untuk custom linked list

---

**[← Filesystem](../../02-menengah/08-filesystem/README.md)** | **[Berikutnya → Metaprogramming](../02-metaprogramming/README.md)**
