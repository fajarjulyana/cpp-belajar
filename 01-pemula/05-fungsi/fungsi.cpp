// ============================================================
// File  : fungsi.cpp
// Topik : Fungsi dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o fungsi fungsi.cpp
// ============================================================
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// ---- Prototipe fungsi ----
int tambah(int a, int b);
double luasLingkaran(double r);
bool isPalindrome(const string& s);
long long faktorial(int n);
long long fibonacci(int n);
void tukar(int& a, int& b);
int maks(int a, int b);
int maks(int a, int b, int c);  // overload

// ---- Definisi fungsi ----

int tambah(int a, int b) {
    return a + b;
}

double luasLingkaran(double r) {
    return 3.14159265 * r * r;
}

bool isPalindrome(const string& s) {
    int kiri  = 0;
    int kanan = (int)s.length() - 1;
    while (kiri < kanan) {
        if (s[kiri] != s[kanan]) return false;
        kiri++;
        kanan--;
    }
    return true;
}

long long faktorial(int n) {
    if (n <= 1) return 1;
    return n * faktorial(n - 1);
}

long long fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

void tukar(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int maks(int a, int b) {
    return a > b ? a : b;
}

int maks(int a, int b, int c) {
    return maks(maks(a, b), c);
}

// Fungsi dengan nilai default
void cetak(const string& s, int ulang = 1, char pemisah = '\n') {
    for (int i = 0; i < ulang; i++) cout << s << pemisah;
}

// Inline
inline int kuadrat(int x) { return x * x; }

// Hitung lingkaran via reference
void hitungLingkaran(double r, double& luas, double& keliling) {
    const double PI = 3.14159265;
    luas     = PI * r * r;
    keliling = 2.0 * PI * r;
}

// Pangkat rekursif
double pangkat(double basis, int exp) {
    if (exp == 0) return 1.0;
    if (exp < 0)  return 1.0 / pangkat(basis, -exp);
    return basis * pangkat(basis, exp - 1);
}

int main() {
    // ---- Fungsi dasar ----
    cout << "=== Fungsi Dasar ===" << endl;
    cout << "3 + 5 = " << tambah(3, 5) << endl;
    cout << "Luas lingkaran r=7: " << luasLingkaran(7.0) << endl;

    // ---- Palindrome ----
    cout << "\n=== isPalindrome ===" << endl;
    vector<string> kata = {"radar", "level", "hello", "madam", "world"};
    for (const auto& k : kata) {
        cout << k << ": " << boolalpha << isPalindrome(k) << endl;
    }

    // ---- Rekursi: Faktorial ----
    cout << "\n=== Faktorial ===" << endl;
    for (int i = 0; i <= 10; i++) {
        cout << i << "! = " << faktorial(i) << endl;
    }

    // ---- Rekursi: Fibonacci ----
    cout << "\n=== Fibonacci ===" << endl;
    cout << "10 deret fibonacci: ";
    for (int i = 0; i < 10; i++) cout << fibonacci(i) << " ";
    cout << endl;

    // ---- Tukar (pass by reference) ----
    cout << "\n=== Pass by Reference ===" << endl;
    int x = 10, y = 20;
    cout << "Sebelum: x=" << x << ", y=" << y << endl;
    tukar(x, y);
    cout << "Sesudah: x=" << x << ", y=" << y << endl;

    // ---- Overloading ----
    cout << "\n=== Overloading ===" << endl;
    cout << "maks(3, 7)       = " << maks(3, 7)    << endl;
    cout << "maks(1, 5, 3)    = " << maks(1, 5, 3) << endl;

    // ---- Default parameter ----
    cout << "\n=== Default Parameter ===" << endl;
    cetak("*", 5, ' ');
    cout << endl;
    cetak("Halo");

    // ---- Inline ----
    cout << "\n=== Inline ===" << endl;
    for (int i = 1; i <= 5; i++)
        cout << i << "^2 = " << kuadrat(i) << endl;

    // ---- Multiple return via reference ----
    cout << "\n=== Multiple Return via Reference ===" << endl;
    double luas, keliling;
    hitungLingkaran(5.0, luas, keliling);
    cout << "r=5 → luas=" << luas << ", keliling=" << keliling << endl;

    // ---- Pangkat rekursif ----
    cout << "\n=== Pangkat Rekursif ===" << endl;
    cout << "2^10 = " << pangkat(2, 10) << endl;
    cout << "3^5  = " << pangkat(3, 5)  << endl;
    cout << "2^-3 = " << pangkat(2, -3) << endl;

    // ---- Lambda ----
    cout << "\n=== Lambda ===" << endl;
    auto kurang = [](int a, int b) { return a - b; };
    cout << "10 - 4 = " << kurang(10, 4) << endl;

    int faktor = 3;
    auto kaliFaktor = [faktor](int n) { return n * faktor; };
    cout << "5 * 3 = " << kaliFaktor(5) << endl;

    return 0;
}
