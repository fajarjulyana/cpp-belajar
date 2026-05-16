// ============================================================
// File  : main_static.cpp
// Topik : Program yang menggunakan static library libmatematika.a
//
// Cara build (lihat juga build_static.sh):
//   1. Kompilasi library ke object file:
//      g++ -std=c++17 -c matematika.cpp -o matematika.o
//   2. Kemas menjadi static library:
//      ar rcs libmatematika.a matematika.o
//   3. Kompilasi program dengan library:
//      g++ -std=c++17 -o program main_static.cpp -L. -lmatematika
//   4. Jalankan:
//      ./program
//
// Perhatikan: libmatematika.a disertakan LANGSUNG ke executable.
// Tidak perlu file .a saat menjalankan — cukup ./program
// ============================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include "matematika.h"

void judulBagian(const std::string& j) {
    std::cout << "\n\033[1;34m── " << j << " ──\033[0m\n";
}

int main() {
    std::cout << "\n\033[1m"
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Menggunakan Static Library (.a)             ║\n"
              << "║  " << Matematika::versi()
              << std::string(44 - Matematika::versi().size(), ' ') << "║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << "\033[0m";

    // ── Operasi dasar ──
    judulBagian("Operasi Dasar");
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  tambah(3.5, 2.1)  = " << Matematika::tambah(3.5, 2.1) << "\n";
    std::cout << "  kurang(10, 4.5)   = " << Matematika::kurang(10, 4.5) << "\n";
    std::cout << "  kali(6, 7)        = " << Matematika::kali(6, 7) << "\n";
    std::cout << "  bagi(22, 7)       = " << Matematika::bagi(22, 7) << "\n";

    // Exception handling
    try {
        Matematika::bagi(5, 0);
    } catch (const std::invalid_argument& e) {
        std::cout << "  bagi(5,0) → Exception: " << e.what() << "\n";
    }

    // ── Statistik ──
    judulBagian("Statistik");
    std::vector<double> data = {12, 7, 3, 14, 6, 11, 5, 8, 9, 10};
    std::cout << "  Data: ";
    for (double x : data) std::cout << x << " ";
    std::cout << "\n";
    std::cout << "  Rata-rata       = " << Matematika::rata2(data) << "\n";
    std::cout << "  Median          = " << Matematika::median(data) << "\n";
    std::cout << "  Standar Deviasi = " << Matematika::standarDeviasi(data) << "\n";
    std::cout << "  Min / Maks      = " << Matematika::min(data)
              << " / " << Matematika::max(data) << "\n";

    // ── Bilangan ──
    judulBagian("Bilangan");
    std::cout << "  Prima 1-30:  ";
    for (int i = 2; i <= 30; ++i)
        if (Matematika::adalahPrima(i)) std::cout << i << " ";
    std::cout << "\n";

    std::cout << "  Faktorial:   ";
    for (int i = 0; i <= 10; ++i)
        std::cout << i << "!=" << Matematika::faktorial(i) << " ";
    std::cout << "\n";

    std::cout << "  Fibonacci:   ";
    for (int i = 0; i <= 10; ++i)
        std::cout << "F(" << i << ")=" << Matematika::fibonacci(i) << " ";
    std::cout << "\n";

    std::cout << "  GCD(48, 18)  = " << Matematika::gcd(48, 18) << "\n";
    std::cout << "  LCM(12, 8)   = " << Matematika::lcm(12, 8) << "\n";

    // ── Konversi ──
    judulBagian("Konversi Sudut");
    std::cout << "  90°  → radian : " << Matematika::derajatKeRadian(90) << "\n";
    std::cout << "  π    → derajat: " << Matematika::radianKeDerjat(3.14159265358979) << "\n";

    std::cout << "\n\033[1;32m✓ Semua fungsi library berjalan dengan baik!\033[0m\n\n";
    return 0;
}
