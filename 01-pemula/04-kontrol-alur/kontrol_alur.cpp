// ============================================================
// File  : kontrol_alur.cpp
// Topik : Kontrol Alur — if, switch, for, while, do-while
// Kompilasi: g++ -std=c++17 -Wall -o kontrol_alur kontrol_alur.cpp
// ============================================================
#include <iostream>
#include <vector>
using namespace std;

int main() {
    // ---- if / else if / else ----
    cout << "=== Contoh if-else ===" << endl;
    int nilai = 78;
    if      (nilai >= 90) cout << "Grade A\n";
    else if (nilai >= 80) cout << "Grade B\n";
    else if (nilai >= 70) cout << "Grade C\n";
    else if (nilai >= 60) cout << "Grade D\n";
    else                  cout << "Grade F\n";

    // ---- switch ----
    cout << "\n=== Contoh switch ===" << endl;
    int hari = 3;
    switch (hari) {
        case 1: cout << "Senin\n";  break;
        case 2: cout << "Selasa\n"; break;
        case 3: cout << "Rabu\n";   break;
        case 4: cout << "Kamis\n";  break;
        case 5: cout << "Jumat\n";  break;
        case 6:
        case 7: cout << "Akhir Pekan\n"; break;
        default: cout << "Tidak valid\n";
    }

    // ---- for loop ----
    cout << "\n=== For Loop ===" << endl;
    for (int i = 1; i <= 10; i++) {
        cout << i << " ";
    }
    cout << "\n";

    // ---- range-based for ----
    cout << "\n=== Range-based For ===" << endl;
    vector<string> buah = {"Apel", "Mangga", "Pisang", "Jeruk"};
    for (const auto& b : buah) {
        cout << "- " << b << "\n";
    }

    // ---- while ----
    cout << "\n=== While Loop ===" << endl;
    int angka = 1;
    while (angka <= 5) {
        cout << angka * angka << " ";  // kuadrat
        angka++;
    }
    cout << "\n";

    // ---- do-while ----
    cout << "\n=== Do-While Loop ===" << endl;
    int hitungan = 3;
    do {
        cout << "Hitung mundur: " << hitungan << "\n";
        hitungan--;
    } while (hitungan > 0);
    cout << "MULAI!\n";

    // ---- break dan continue ----
    cout << "\n=== Break & Continue ===" << endl;
    cout << "Bilangan 1-20 yang bukan kelipatan 3, berhenti di 15: ";
    for (int i = 1; i <= 20; i++) {
        if (i == 15) break;
        if (i % 3 == 0) continue;
        cout << i << " ";
    }
    cout << "\n";

    // ---- Nested loop: tabel perkalian ----
    cout << "\n=== Tabel Perkalian (1-5) ===" << endl;
    for (int i = 1; i <= 5; i++) {
        for (int j = 1; j <= 5; j++) {
            cout << i * j << "\t";
        }
        cout << "\n";
    }

    // ---- Contoh: Bilangan Prima ----
    cout << "\n=== Bilangan Prima 2-50 ===" << endl;
    for (int n = 2; n <= 50; n++) {
        bool prima = true;
        for (int d = 2; d * d <= n; d++) {
            if (n % d == 0) { prima = false; break; }
        }
        if (prima) cout << n << " ";
    }
    cout << "\n";

    // ---- Faktorial ----
    cout << "\n=== Faktorial ===" << endl;
    long long fakto = 1;
    for (int i = 1; i <= 12; i++) {
        fakto *= i;
        cout << i << "! = " << fakto << "\n";
    }

    return 0;
}
