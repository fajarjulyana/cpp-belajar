// ============================================================
// File  : design_patterns.cpp
// Topik : Design Patterns dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o dp_demo design_patterns.cpp
// ============================================================
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
using namespace std;

// ============================================================
// 1. SINGLETON
// ============================================================
class Logger {
    vector<string> log;
    Logger() {}
public:
    static Logger& get() { static Logger inst; return inst; }
    void tulis(const string& msg) { log.push_back(msg); cout << "[LOG] " << msg << "\n"; }
    void cetakSemua() {
        cout << "=== Riwayat Log ===\n";
        for (const auto& m : log) cout << "  " << m << "\n";
    }
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

// ============================================================
// 2. FACTORY METHOD
// ============================================================
struct Tombol {
    virtual void render() = 0;
    virtual ~Tombol() = default;
};
struct TombolWeb     : Tombol { void render() override { cout << "[HTML Button]\n"; }    };
struct TombolDesktop : Tombol { void render() override { cout << "[Native Button]\n"; }  };
struct TombolMobile  : Tombol { void render() override { cout << "[Mobile Button]\n"; }  };

struct UIFactory {
    virtual unique_ptr<Tombol> buatTombol() = 0;
    virtual ~UIFactory() = default;
};
struct WebFactory     : UIFactory { unique_ptr<Tombol> buatTombol() override { return make_unique<TombolWeb>(); } };
struct DesktopFactory : UIFactory { unique_ptr<Tombol> buatTombol() override { return make_unique<TombolDesktop>(); } };
struct MobileFactory  : UIFactory { unique_ptr<Tombol> buatTombol() override { return make_unique<TombolMobile>(); } };

// ============================================================
// 3. BUILDER
// ============================================================
struct Mobil {
    string merek, warna, transmisi;
    int tahun, pintus;
    bool sunroof, gps;
    void info() const {
        cout << merek << " " << tahun << " [" << warna << "] "
             << pintus << " pintu, " << transmisi
             << (sunroof ? " +sunroof" : "")
             << (gps     ? " +gps"     : "") << "\n";
    }
};

class MobilBuilder {
    Mobil m;
public:
    MobilBuilder& merek(const string& s) { m.merek = s; return *this; }
    MobilBuilder& warna(const string& s) { m.warna = s; return *this; }
    MobilBuilder& tahun(int t)           { m.tahun = t; return *this; }
    MobilBuilder& pintus(int p)          { m.pintus = p; return *this; }
    MobilBuilder& transmisi(const string& s) { m.transmisi = s; return *this; }
    MobilBuilder& sunroof(bool b)        { m.sunroof = b; return *this; }
    MobilBuilder& gps(bool b)            { m.gps = b; return *this; }
    Mobil build() { return m; }
};

// ============================================================
// 4. OBSERVER
// ============================================================
struct Pengamat {
    virtual void update(const string& acara, double nilai) = 0;
    virtual ~Pengamat() = default;
};

class SahamStore {
    string kode;
    double harga;
    vector<weak_ptr<Pengamat>> obs;
public:
    SahamStore(const string& k, double h) : kode(k), harga(h) {}
    void daftar(shared_ptr<Pengamat> p) { obs.push_back(p); }
    void setHarga(double h) {
        harga = h;
        for (auto it = obs.begin(); it != obs.end(); ) {
            if (auto sp = it->lock()) { sp->update(kode, harga); ++it; }
            else it = obs.erase(it);
        }
    }
};

struct AlertInvestor : Pengamat {
    string nama; double beli, jual;
    AlertInvestor(string n, double b, double j) : nama(n), beli(b), jual(j) {}
    void update(const string& kode, double h) override {
        if (h <= beli)  cout << nama << ": BELI " << kode << " @ " << h << "\n";
        if (h >= jual)  cout << nama << ": JUAL " << kode << " @ " << h << "\n";
    }
};

// ============================================================
// 5. STRATEGY
// ============================================================
struct Kompresor {
    virtual string kompresi(const string& data) const = 0;
    virtual string nama() const = 0;
    virtual ~Kompresor() = default;
};
struct KompresiNone : Kompresor {
    string kompresi(const string& d) const override { return d; }
    string nama() const override { return "None"; }
};
struct KompresiRLE : Kompresor {
    string kompresi(const string& d) const override {
        string h; int i = 0;
        while (i < (int)d.size()) {
            char c = d[i]; int cnt = 1;
            while (i+cnt < (int)d.size() && d[i+cnt] == c) cnt++;
            h += to_string(cnt) + c; i += cnt;
        }
        return h;
    }
    string nama() const override { return "RLE"; }
};
struct KompresiReverse : Kompresor {
    string kompresi(const string& d) const override { return string(d.rbegin(), d.rend()); }
    string nama() const override { return "Reverse"; }
};

class FileManager {
    unique_ptr<Kompresor> kompresor;
public:
    void setKompresor(unique_ptr<Kompresor> k) { kompresor = move(k); }
    void simpan(const string& data) {
        string hasil = kompresor->kompresi(data);
        cout << "[" << kompresor->nama() << "] '" << data << "' → '" << hasil << "'\n";
    }
};

// ============================================================
// 6. COMMAND (dengan Undo)
// ============================================================
struct Perintah {
    virtual void lakukan()  = 0;
    virtual void batalkan() = 0;
    virtual ~Perintah() = default;
};

class EditorTeks {
    string teks;
public:
    void tambah(const string& s)    { teks += s; }
    void hapusN(int n)              { if(n<=(int)teks.size()) teks.erase(teks.size()-n); }
    string get() const              { return teks; }
};

struct PerintahKetik : Perintah {
    EditorTeks& ed; string s;
    PerintahKetik(EditorTeks& e, string t) : ed(e), s(t) {}
    void lakukan()  override { ed.tambah(s); }
    void batalkan() override { ed.hapusN((int)s.size()); }
};

class Invoker {
    vector<unique_ptr<Perintah>> hist;
public:
    void jalankan(unique_ptr<Perintah> p) { p->lakukan(); hist.push_back(move(p)); }
    void undo() { if (!hist.empty()) { hist.back()->batalkan(); hist.pop_back(); } }
};

// ============================================================
// 7. DECORATOR
// ============================================================
struct Kopi {
    virtual double harga()     const = 0;
    virtual string deskripsi() const = 0;
    virtual ~Kopi() = default;
};
struct KopiPolos : Kopi {
    double harga()     const override { return 15000; }
    string deskripsi() const override { return "Kopi"; }
};
struct Dekorasi : Kopi {
protected: unique_ptr<Kopi> k;
public:   Dekorasi(unique_ptr<Kopi> k) : k(move(k)) {}
};
struct Susu : Dekorasi {
    using Dekorasi::Dekorasi;
    double harga()     const override { return k->harga() + 5000; }
    string deskripsi() const override { return k->deskripsi() + "+Susu"; }
};
struct Gula : Dekorasi {
    using Dekorasi::Dekorasi;
    double harga()     const override { return k->harga() + 2000; }
    string deskripsi() const override { return k->deskripsi() + "+Gula"; }
};
struct Karamel : Dekorasi {
    using Dekorasi::Dekorasi;
    double harga()     const override { return k->harga() + 8000; }
    string deskripsi() const override { return k->deskripsi() + "+Karamel"; }
};

int main() {
    // Singleton
    cout << "=== Singleton Logger ===" << endl;
    Logger::get().tulis("Aplikasi mulai");
    Logger::get().tulis("User login: Budi");
    Logger::get().cetakSemua();

    // Factory
    cout << "\n=== Factory Method ===" << endl;
    vector<unique_ptr<UIFactory>> factories;
    factories.push_back(make_unique<WebFactory>());
    factories.push_back(make_unique<DesktopFactory>());
    factories.push_back(make_unique<MobileFactory>());
    for (auto& f : factories) f->buatTombol()->render();

    // Builder
    cout << "\n=== Builder Pattern ===" << endl;
    Mobil m1 = MobilBuilder().merek("Toyota").warna("Putih").tahun(2023)
                              .pintus(5).transmisi("AT").sunroof(true).gps(true).build();
    Mobil m2 = MobilBuilder().merek("Honda").warna("Merah").tahun(2022)
                              .pintus(4).transmisi("MT").sunroof(false).gps(false).build();
    m1.info(); m2.info();

    // Observer
    cout << "\n=== Observer Pattern ===" << endl;
    SahamStore saham("BBCA", 9000);
    auto inv1 = make_shared<AlertInvestor>("Budi",  8800, 9500);
    auto inv2 = make_shared<AlertInvestor>("Ani",   8500, 10000);
    saham.daftar(inv1); saham.daftar(inv2);
    saham.setHarga(8700);  // Budi beli
    saham.setHarga(9600);  // Budi jual

    // Strategy
    cout << "\n=== Strategy Pattern ===" << endl;
    FileManager fm;
    fm.setKompresor(make_unique<KompresiNone>()); fm.simpan("AAABBBCCCC");
    fm.setKompresor(make_unique<KompresiRLE>());  fm.simpan("AAABBBCCCC");
    fm.setKompresor(make_unique<KompresiReverse>());fm.simpan("AAABBBCCCC");

    // Command + Undo
    cout << "\n=== Command + Undo ===" << endl;
    EditorTeks ed; Invoker inv;
    inv.jalankan(make_unique<PerintahKetik>(ed, "Halo"));
    inv.jalankan(make_unique<PerintahKetik>(ed, " Dunia"));
    cout << "Teks: " << ed.get() << "\n";
    inv.undo();
    cout << "Setelah undo: " << ed.get() << "\n";
    inv.undo();
    cout << "Setelah undo 2: '" << ed.get() << "'\n";

    // Decorator
    cout << "\n=== Decorator Pattern (Kopi) ===" << endl;
    auto k1 = make_unique<KopiPolos>();
    cout << k1->deskripsi() << ": Rp " << k1->harga() << "\n";
    auto k2 = make_unique<Susu>(make_unique<KopiPolos>());
    cout << k2->deskripsi() << ": Rp " << k2->harga() << "\n";
    auto k3 = make_unique<Karamel>(make_unique<Gula>(make_unique<Susu>(make_unique<KopiPolos>())));
    cout << k3->deskripsi() << ": Rp " << k3->harga() << "\n";

    return 0;
}
