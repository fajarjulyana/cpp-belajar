// ============================================================
// File  : variabel.cpp
// Topik : Variabel dan Tipe Data dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o variabel variabel.cpp
// ============================================================
#include <iostream>
#include <string>
#include <climits>
#include <cfloat>
#include <limits>
using namespace std;

int main() {
    // ---- Tipe Integer ----
    short       s  = 30000;
    int         i  = 2000000;
    long        l  = 123456789L;
    long long   ll = 9000000000LL;
    unsigned int ui = 4000000000U;

    cout << "=== Tipe Integer ===" << endl;
    cout << "short:     " << s  << endl;
    cout << "int:       " << i  << endl;
    cout << "long:      " << l  << endl;
    cout << "long long: " << ll << endl;
    cout << "unsigned:  " << ui << endl;

    // ---- Tipe Floating Point ----
    float       f  = 3.14f;
    double      d  = 3.14159265358979;
    long double ld = 3.141592653589793238L;

    cout << "\n=== Tipe Floating Point ===" << endl;
    cout.precision(15);
    cout << "float:       " << f  << endl;
    cout << "double:      " << d  << endl;
    cout << "long double: " << ld << endl;

    // ---- Tipe Karakter ----
    char c1 = 'A';
    char c2 = 66;  // 'B' dalam ASCII

    cout << "\n=== Tipe Karakter ===" << endl;
    cout << "char c1: " << c1 << " (kode ASCII: " << (int)c1 << ")" << endl;
    cout << "char c2: " << c2 << " (kode ASCII: " << (int)c2 << ")" << endl;

    // ---- Tipe Boolean ----
    bool benar = true;
    bool salah = false;
    bool hasil = (10 > 5);

    cout << "\n=== Tipe Boolean ===" << endl;
    cout << boolalpha;  // tampilkan true/false, bukan 1/0
    cout << "benar: " << benar << endl;
    cout << "salah: " << salah << endl;
    cout << "10 > 5: " << hasil << endl;

    // ---- Tipe String ----
    string nama     = "Budi Santoso";
    string kota     = "Jakarta";
    string gabungan = nama + " dari " + kota;

    cout << "\n=== Tipe String ===" << endl;
    cout << "nama:     " << nama     << endl;
    cout << "kota:     " << kota     << endl;
    cout << "gabungan: " << gabungan << endl;
    cout << "panjang nama: " << nama.length() << " karakter" << endl;

    // ---- Auto (Type Inference) ----
    auto angka  = 42;
    auto desimal = 9.81;
    auto teks   = string("Halo Auto");

    cout << "\n=== Auto Type Deduction ===" << endl;
    cout << "auto int:    " << angka   << endl;
    cout << "auto double: " << desimal << endl;
    cout << "auto string: " << teks    << endl;

    // ---- Konstanta ----
    const double PI = 3.14159265358979;
    constexpr int MAKS_NILAI = 1000;
    double radius = 7.0;
    double luas = PI * radius * radius;

    cout << "\n=== Konstanta ===" << endl;
    cout << "PI = " << PI << endl;
    cout << "MAKS = " << MAKS_NILAI << endl;
    cout << "Luas lingkaran r=" << radius << ": " << luas << endl;

    // ---- Konversi Tipe ----
    int bilBulat = 7;
    double hasilBagi = static_cast<double>(bilBulat) / 2;
    int terpotong = static_cast<int>(9.99);

    cout << "\n=== Konversi Tipe ===" << endl;
    cout << "7 / 2 (int)    = " << bilBulat / 2    << endl;
    cout << "7.0 / 2 (dbl)  = " << hasilBagi       << endl;
    cout << "static_cast<int>(9.99) = " << terpotong << endl;

    // ---- Ukuran Tipe ----
    cout << "\n=== sizeof Tipe Data ===" << endl;
    cout << "sizeof(char):      " << sizeof(char)      << " byte" << endl;
    cout << "sizeof(short):     " << sizeof(short)     << " byte" << endl;
    cout << "sizeof(int):       " << sizeof(int)       << " byte" << endl;
    cout << "sizeof(long):      " << sizeof(long)      << " byte" << endl;
    cout << "sizeof(long long): " << sizeof(long long) << " byte" << endl;
    cout << "sizeof(float):     " << sizeof(float)     << " byte" << endl;
    cout << "sizeof(double):    " << sizeof(double)    << " byte" << endl;
    cout << "sizeof(bool):      " << sizeof(bool)      << " byte" << endl;

    // ---- Batas Nilai ----
    cout << "\n=== Batas Nilai ===" << endl;
    cout << "INT_MAX:  " << numeric_limits<int>::max()    << endl;
    cout << "INT_MIN:  " << numeric_limits<int>::min()    << endl;
    cout << "DBL_MAX:  " << numeric_limits<double>::max() << endl;

    return 0;
}
