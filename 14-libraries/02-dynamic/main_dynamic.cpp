// ============================================================
// File  : main_dynamic.cpp
// Topik : Program yang menggunakan shared library libutilitas.so
//
// Cara build (lihat juga build_dynamic.sh):
//   1. Kompilasi library ke object dengan -fPIC:
//      g++ -std=c++17 -fPIC -c utilitas.cpp -o utilitas.o
//   2. Buat shared library:
//      g++ -shared -o libutilitas.so utilitas.o
//   3. Kompilasi program:
//      g++ -std=c++17 -o program_dyn main_dynamic.cpp -L. -lutilitas
//   4. Jalankan (pastikan .so bisa ditemukan):
//      LD_LIBRARY_PATH=. ./program_dyn
//
// Catatan: program_dyn TIDAK menyertakan kode library.
//          .so diload saat runtime oleh dynamic linker.
// ============================================================

#include <iostream>
#include <iomanip>
#include <vector>
#include "utilitas.h"

void judulBagian(const std::string& j) {
    std::cout << "\n\033[1;35m── " << j << " ──\033[0m\n";
}

void ok(const std::string& label, const std::string& nilai) {
    std::cout << "  " << std::left << std::setw(30) << label
              << "→ " << nilai << "\n";
}

void okBool(const std::string& label, bool nilai) {
    std::cout << "  " << std::left << std::setw(30) << label
              << "→ " << (nilai ? "\033[32m✓ valid\033[0m"
                                : "\033[31m✗ tidak valid\033[0m") << "\n";
}

int main() {
    std::cout << "\n\033[1m"
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Menggunakan Dynamic Library (.so)           ║\n"
              << "║  " << Utilitas::versi()
              << std::string(44 - Utilitas::versi().size(), ' ') << "║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << "\033[0m";

    // ── String ──
    judulBagian("String Utilities");
    std::string s = "Halo Dunia, Belajar C++!";
    std::cout << "  Input: \"" << s << "\"\n\n";
    ok("hurufBesar",       Utilitas::hurufBesar(s));
    ok("hurufKecil",       Utilitas::hurufKecil(s));
    ok("balik",            Utilitas::balik(s));
    ok("hitungKata",       std::to_string(Utilitas::hitungKata(s)));
    ok("formatRibu(1e9)",  Utilitas::formatRibu(1'000'000'000LL));
    ok("formatRibu(-5000)",Utilitas::formatRibu(-5000));

    // palindrom
    std::vector<std::string> tes = {"katak", "A man a plan a canal Panama", "hello"};
    for (auto& t : tes) {
        std::cout << "  palindrom(\"" << t << "\") → "
                  << (Utilitas::palindrom(t) ? "\033[32mya\033[0m"
                                             : "\033[31mtidak\033[0m") << "\n";
    }

    // split + join
    judulBagian("Split & Join");
    std::string csv = "Budi,Sari,Eko,Dewi,Rina";
    auto parts = Utilitas::pisah(csv, ',');
    std::cout << "  pisah(\"" << csv << "\", ',')\n    → ";
    for (auto& p : parts) std::cout << "[" << p << "] ";
    std::cout << "\n";
    ok("gabung(\" | \")", Utilitas::gabung(parts, " | "));

    // ── Waktu ──
    judulBagian("Waktu & Tanggal");
    ok("waktuSekarang",   Utilitas::waktuSekarang());
    ok("tanggalSekarang", Utilitas::tanggalSekarang());
    ok("timestamp (ms)",  std::to_string(Utilitas::timestampSekarang()));

    // ── Validasi ──
    judulBagian("Validasi");
    okBool("validEmail(budi@mail.com)",  Utilitas::validEmail("budi@mail.com"));
    okBool("validEmail(bukan-email)",    Utilitas::validEmail("bukan-email"));
    okBool("validAngka(\"3.14\")",       Utilitas::validAngka("3.14"));
    okBool("validAngka(\"-42\")",        Utilitas::validAngka("-42"));
    okBool("validAngka(\"abc\")",        Utilitas::validAngka("abc"));
    okBool("validURL(https://...)",      Utilitas::validURL("https://google.com"));
    okBool("validURL(bukan-url)",        Utilitas::validURL("bukan-url"));

    // ── Hash ──
    judulBagian("FNV-1a Hash 32-bit");
    std::vector<std::string> inputs = {"halo", "dunia", "C++17", ""};
    for (auto& inp : inputs)
        ok("hash32(\"" + inp + "\")", Utilitas::hash32(inp));

    std::cout << "\n\033[1;32m✓ Dynamic library bekerja dengan baik!\033[0m\n\n";
    return 0;
}
