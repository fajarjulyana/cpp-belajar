// ============================================================
// File  : oop_dasar.cpp
// Topik : OOP Dasar — Class, Object, Constructor, Operator
// Kompilasi: g++ -std=c++17 -Wall -o oop_dasar oop_dasar.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

// ---- BankAccount ----
class BankAccount {
private:
    string pemilik;
    double saldo;
    static int totalAkun;

public:
    BankAccount() : pemilik("Anonim"), saldo(0.0) { ++totalAkun; }
    BankAccount(const string& nama, double saldoAwal)
        : pemilik(nama), saldo(saldoAwal) { ++totalAkun; }
    ~BankAccount() { --totalAkun; }

    bool setor(double nominal) {
        if (nominal <= 0) return false;
        saldo += nominal; return true;
    }
    bool tarik(double nominal) {
        if (nominal <= 0 || nominal > saldo) return false;
        saldo -= nominal; return true;
    }
    string getPemilik() const { return pemilik; }
    double getSaldo()   const { return saldo; }
    static int getTotalAkun() { return totalAkun; }
    void info() const {
        cout << "  Pemilik : " << pemilik << "\n"
             << "  Saldo   : Rp " << saldo << "\n";
    }
};
int BankAccount::totalAkun = 0;

// ---- Pecahan (dengan operator overloading) ----
class Pecahan {
private:
    int pembilang, penyebut;
    int gcd(int a, int b) const { return b == 0 ? a : gcd(b, a % b); }
    void sederhanakan() {
        int g = gcd(abs(pembilang), abs(penyebut));
        pembilang /= g;
        penyebut  /= g;
        if (penyebut < 0) { pembilang = -pembilang; penyebut = -penyebut; }
    }
public:
    Pecahan(int p = 0, int q = 1) : pembilang(p), penyebut(q) {
        if (q == 0) throw invalid_argument("Penyebut nol!");
        sederhanakan();
    }
    Pecahan operator+(const Pecahan& o) const {
        return Pecahan(pembilang * o.penyebut + o.pembilang * penyebut,
                       penyebut * o.penyebut);
    }
    Pecahan operator-(const Pecahan& o) const {
        return Pecahan(pembilang * o.penyebut - o.pembilang * penyebut,
                       penyebut * o.penyebut);
    }
    Pecahan operator*(const Pecahan& o) const {
        return Pecahan(pembilang * o.pembilang, penyebut * o.penyebut);
    }
    bool operator==(const Pecahan& o) const {
        return pembilang == o.pembilang && penyebut == o.penyebut;
    }
    friend ostream& operator<<(ostream& os, const Pecahan& f) {
        if (f.penyebut == 1) return os << f.pembilang;
        return os << f.pembilang << "/" << f.penyebut;
    }
};

// ---- Vektor2D ----
class Vektor2D {
public:
    double x, y;
    Vektor2D(double x=0, double y=0): x(x), y(y) {}
    Vektor2D operator+(const Vektor2D& v) const { return {x+v.x, y+v.y}; }
    Vektor2D operator-(const Vektor2D& v) const { return {x-v.x, y-v.y}; }
    Vektor2D operator*(double s) const { return {x*s, y*s}; }
    double   panjang() const { return sqrt(x*x + y*y); }
    double   dotProd(const Vektor2D& v) const { return x*v.x + y*v.y; }
    friend ostream& operator<<(ostream& os, const Vektor2D& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }
};

// ---- Tumpukan (Stack) menggunakan template ----
template<typename T>
class Tumpukan {
private:
    vector<T> data;
public:
    void push(const T& val) { data.push_back(val); }
    T    pop() {
        if (kosong()) throw out_of_range("Tumpukan kosong!");
        T val = data.back(); data.pop_back(); return val;
    }
    T    peek() const {
        if (kosong()) throw out_of_range("Tumpukan kosong!");
        return data.back();
    }
    bool kosong() const { return data.empty(); }
    int  ukuran() const { return (int)data.size(); }
};

int main() {
    // ---- BankAccount ----
    cout << "=== BankAccount ===" << endl;
    BankAccount akun1("Budi Santoso", 1000000);
    BankAccount akun2("Ani Rahayu",   500000);

    cout << "Total akun: " << BankAccount::getTotalAkun() << "\n";

    akun1.setor(250000);
    akun1.tarik(100000);
    akun2.setor(750000);

    cout << "Akun 1:\n"; akun1.info();
    cout << "Akun 2:\n"; akun2.info();

    if (!akun2.tarik(2000000))
        cout << "Gagal tarik: saldo tidak cukup!\n";

    // ---- Pecahan ----
    cout << "\n=== Pecahan (Operator Overloading) ===" << endl;
    Pecahan p1(1, 2), p2(1, 3), p3(2, 4);
    cout << p1 << " + " << p2 << " = " << (p1 + p2) << "\n";
    cout << p1 << " - " << p2 << " = " << (p1 - p2) << "\n";
    cout << p1 << " * " << p2 << " = " << (p1 * p2) << "\n";
    cout << p1 << " == " << p3 << " : " << boolalpha << (p1 == p3) << "\n";

    // ---- Vektor2D ----
    cout << "\n=== Vektor 2D ===" << endl;
    Vektor2D v1(3, 4), v2(1, 2);
    cout << "v1 = " << v1 << ", panjang = " << v1.panjang() << "\n";
    cout << "v2 = " << v2 << "\n";
    cout << "v1 + v2 = " << (v1 + v2) << "\n";
    cout << "v1 - v2 = " << (v1 - v2) << "\n";
    cout << "v1 * 3  = " << (v1 * 3)  << "\n";
    cout << "v1 . v2 = " << v1.dotProd(v2) << "\n";

    // ---- Tumpukan (Template) ----
    cout << "\n=== Tumpukan (Stack Template) ===" << endl;
    Tumpukan<int> ts;
    for (int i = 1; i <= 5; i++) ts.push(i * 10);
    cout << "Ukuran: " << ts.ukuran() << ", Top: " << ts.peek() << "\n";
    cout << "Pop: ";
    while (!ts.kosong()) cout << ts.pop() << " ";
    cout << "\n";

    Tumpukan<string> strStack;
    strStack.push("pertama");
    strStack.push("kedua");
    strStack.push("ketiga");
    cout << "String stack top: " << strStack.peek() << "\n";

    return 0;
}
