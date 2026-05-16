// ============================================================
// File  : oop_lanjut.cpp
// Topik : OOP Lanjut — Inheritance, Polymorphism, Abstract Class
// Kompilasi: g++ -std=c++17 -Wall -o oop_lanjut oop_lanjut.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
using namespace std;

// ---- Abstract Base Class ----
class Bentuk {
protected:
    string warna;
public:
    Bentuk(const string& w = "putih") : warna(w) {}
    virtual double luas()     const = 0;
    virtual double keliling() const = 0;
    virtual string nama()     const = 0;
    virtual void tampilkan()  const {
        cout << nama() << " [" << warna << "]"
             << " — Luas=" << luas()
             << " Keliling=" << keliling() << "\n";
    }
    virtual ~Bentuk() = default;
};

class Lingkaran : public Bentuk {
    double r;
public:
    Lingkaran(double r, const string& w="merah") : Bentuk(w), r(r) {}
    double luas()     const override { return M_PI * r * r; }
    double keliling() const override { return 2 * M_PI * r; }
    string nama()     const override { return "Lingkaran(r=" + to_string(r) + ")"; }
};

class Persegi : public Bentuk {
    double s;
public:
    Persegi(double s, const string& w="biru") : Bentuk(w), s(s) {}
    double luas()     const override { return s * s; }
    double keliling() const override { return 4 * s; }
    string nama()     const override { return "Persegi(s=" + to_string(s) + ")"; }
};

class SegitigaSS : public Bentuk {
    double a, b, c;  // ketiga sisi
public:
    SegitigaSS(double a, double b, double c, const string& w="hijau")
        : Bentuk(w), a(a), b(b), c(c) {}
    double luas() const override {
        double s = (a+b+c)/2.0;
        return sqrt(s*(s-a)*(s-b)*(s-c));  // Heron
    }
    double keliling() const override { return a + b + c; }
    string nama()     const override {
        return "Segitiga(" + to_string(a) + "," + to_string(b) + "," + to_string(c) + ")";
    }
};

// ---- Hirarki Karyawan ----
class Karyawan {
protected:
    string nama;
    int    id;
    double gajiBasis;
public:
    Karyawan(const string& n, int i, double g) : nama(n), id(i), gajiBasis(g) {}
    virtual double hitungGaji() const { return gajiBasis; }
    virtual string jabatan()    const { return "Karyawan"; }
    virtual void info() const {
        cout << jabatan() << ": " << nama
             << " (ID=" << id << ") Gaji=Rp " << hitungGaji() << "\n";
    }
    virtual ~Karyawan() = default;
};

class Manager : public Karyawan {
    double tunjangan;
public:
    Manager(const string& n, int i, double g, double t)
        : Karyawan(n, i, g), tunjangan(t) {}
    double hitungGaji() const override { return gajiBasis + tunjangan; }
    string jabatan()    const override { return "Manager"; }
};

class Direktur : public Manager {
    double bonus;
public:
    Direktur(const string& n, int i, double g, double t, double b)
        : Manager(n, i, g, t), bonus(b) {}
    double hitungGaji() const override { return Manager::hitungGaji() + bonus; }
    string jabatan()    const override { return "Direktur"; }
};

// ---- Multiple Inheritance ----
class Berenang {
public:
    virtual void berenang() { cout << "Berenang...\n"; }
};

class Terbang {
public:
    virtual void terbang() { cout << "Terbang...\n"; }
};

class Bebek : public Karyawan, public Berenang, public Terbang {
public:
    Bebek(const string& n) : Karyawan(n, 0, 0) {}
    string jabatan() const override { return "Bebek"; }
    void bersuara() const { cout << nama << ": Kwek kwek!\n"; }
};

int main() {
    // ---- Polymorphism dengan Smart Pointer ----
    cout << "=== Polymorphism (Bentuk Geometri) ===" << endl;
    vector<unique_ptr<Bentuk>> bentuk;
    bentuk.push_back(make_unique<Lingkaran>(5.0, "merah"));
    bentuk.push_back(make_unique<Persegi>(4.0, "biru"));
    bentuk.push_back(make_unique<SegitigaSS>(3.0, 4.0, 5.0, "hijau"));
    bentuk.push_back(make_unique<Lingkaran>(2.5, "kuning"));

    double totalLuas = 0;
    for (const auto& b : bentuk) {
        b->tampilkan();
        totalLuas += b->luas();
    }
    cout << "Total Luas: " << totalLuas << "\n";

    // ---- Hirarki Karyawan ----
    cout << "\n=== Hirarki Karyawan ===" << endl;
    vector<unique_ptr<Karyawan>> pegawai;
    pegawai.push_back(make_unique<Karyawan>("Ani",   101, 4000000));
    pegawai.push_back(make_unique<Karyawan>("Budi",  102, 4500000));
    pegawai.push_back(make_unique<Manager>("Candra",  201, 6000000, 2000000));
    pegawai.push_back(make_unique<Manager>("Dewi",    202, 6500000, 2500000));
    pegawai.push_back(make_unique<Direktur>("Eko",    301, 10000000, 5000000, 3000000));

    double totalGaji = 0;
    for (const auto& p : pegawai) {
        p->info();
        totalGaji += p->hitungGaji();
    }
    cout << "Total Gaji/Bulan: Rp " << totalGaji << "\n";

    // ---- dynamic_cast ----
    cout << "\n=== dynamic_cast ===" << endl;
    for (const auto& p : pegawai) {
        Manager* m = dynamic_cast<Manager*>(p.get());
        if (m) cout << m->jabatan() << ": " << p.get()->info(), (void)0;
    }

    // ---- Multiple Inheritance ----
    cout << "\n=== Multiple Inheritance (Bebek) ===" << endl;
    Bebek donald("Donald");
    donald.bersuara();
    donald.berenang();
    donald.terbang();

    // ---- Urutkan berdasarkan luas ----
    cout << "\n=== Bentuk Diurutkan (Luas Terkecil) ===" << endl;
    sort(bentuk.begin(), bentuk.end(), [](const auto& a, const auto& b){
        return a->luas() < b->luas();
    });
    for (const auto& b : bentuk) b->tampilkan();

    return 0;
}
