// ============================================================
// File  : operator.cpp
// Topik : Semua Operator C++
// Kompilasi: g++ -std=c++17 -Wall -o operator operator.cpp
// ============================================================
#include <iostream>
#include <string>
using namespace std;

int main() {
    // ---- Aritmatika ----
    int a = 17, b = 5;
    cout << "=== Operator Aritmatika ===" << endl;
    cout << a << " + " << b << " = " << (a + b) << endl;
    cout << a << " - " << b << " = " << (a - b) << endl;
    cout << a << " * " << b << " = " << (a * b) << endl;
    cout << a << " / " << b << " = " << (a / b) << "  (integer division!)" << endl;
    cout << a << " % " << b << " = " << (a % b) << "  (sisa bagi)" << endl;
    double da = 17.0, db = 5.0;
    cout << da << " / " << db << " = " << (da / db) << "  (float division)" << endl;

    // ---- Penugasan ----
    int n = 100;
    cout << "\n=== Operator Penugasan ===" << endl;
    cout << "awal n = " << n << endl;
    n += 20; cout << "n += 20 → " << n << endl;
    n -= 15; cout << "n -= 15 → " << n << endl;
    n *= 2;  cout << "n *= 2  → " << n << endl;
    n /= 3;  cout << "n /= 3  → " << n << endl;
    n %= 7;  cout << "n %= 7  → " << n << endl;

    // ---- Perbandingan ----
    int x = 8, y = 5;
    cout << "\n=== Operator Perbandingan ===" << endl;
    cout << boolalpha;
    cout << x << " == " << y << " : " << (x == y) << endl;
    cout << x << " != " << y << " : " << (x != y) << endl;
    cout << x << " >  " << y << " : " << (x > y)  << endl;
    cout << x << " <  " << y << " : " << (x < y)  << endl;
    cout << x << " >= " << y << " : " << (x >= y) << endl;
    cout << x << " <= " << y << " : " << (x <= y) << endl;

    // ---- Logika ----
    bool p = true, q = false;
    cout << "\n=== Operator Logika ===" << endl;
    cout << "p=" << p << ", q=" << q << endl;
    cout << "p && q = " << (p && q) << endl;
    cout << "p || q = " << (p || q) << endl;
    cout << "!p     = " << (!p)     << endl;
    cout << "!q     = " << (!q)     << endl;

    // ---- Increment/Decrement ----
    int c = 5;
    cout << "\n=== Increment & Decrement ===" << endl;
    cout << "c = " << c << endl;
    cout << "++c = " << ++c << "  (pre-increment, c sekarang=" << c << ")" << endl;
    cout << "c++ = " << c++ << "  (post-increment, c sekarang=" << c << ")" << endl;
    cout << "--c = " << --c << "  (pre-decrement, c sekarang=" << c << ")" << endl;
    cout << "c-- = " << c-- << "  (post-decrement, c sekarang=" << c << ")" << endl;

    // ---- Bitwise ----
    int ba = 12;  // 1100
    int bb = 10;  // 1010
    cout << "\n=== Operator Bitwise ===" << endl;
    cout << "a=12 (1100), b=10 (1010)" << endl;
    cout << "a & b  = " << (ba & bb)  << "  (AND: 1000 = 8)"  << endl;
    cout << "a | b  = " << (ba | bb)  << "  (OR:  1110 = 14)" << endl;
    cout << "a ^ b  = " << (ba ^ bb)  << "  (XOR: 0110 = 6)"  << endl;
    cout << "a << 1 = " << (ba << 1)  << "  (kiri 1 = *2)"    << endl;
    cout << "a >> 1 = " << (ba >> 1)  << "  (kanan 1 = /2)"   << endl;

    // ---- Ternary ----
    int nilai = 75;
    cout << "\n=== Operator Ternary ===" << endl;
    string kelulusan = (nilai >= 70) ? "LULUS" : "TIDAK LULUS";
    cout << "Nilai " << nilai << ": " << kelulusan << endl;
    int maks = (x > y) ? x : y;
    cout << "Maks(" << x << ", " << y << ") = " << maks << endl;

    // ---- sizeof ----
    cout << "\n=== sizeof ===" << endl;
    cout << "sizeof(int)    = " << sizeof(int)    << endl;
    cout << "sizeof(double) = " << sizeof(double) << endl;
    cout << "sizeof(n)      = " << sizeof(n)      << " (variabel int)" << endl;

    return 0;
}
