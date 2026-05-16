// ============================================================
// File  : kalkulator.cpp
// Proyek: Kalkulator Ilmiah CLI
//
// Kompilasi:
//   g++ -std=c++17 -Wall -o kalkulator kalkulator.cpp -lm
// Jalankan:
//   ./kalkulator
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <functional>

// ============================================================
// Struct untuk riwayat kalkulasi
// ============================================================
struct Riwayat {
    std::string ekspresi;
    double hasil;
    std::string timestamp;

    std::string format() const {
        std::ostringstream ss;
        ss << std::setw(30) << std::left << ekspresi
           << " = " << std::setw(15) << std::right
           << std::setprecision(10) << hasil;
        return ss.str();
    }
};

// ============================================================
// Kelas Kalkulator
// ============================================================
class Kalkulator {
public:
    // Fungsi yang tersedia
    std::map<std::string, std::function<double(double)>> fungsiSatu = {
        {"sin",  [](double x){ return std::sin(x * M_PI / 180.0); }},  // input derajat
        {"cos",  [](double x){ return std::cos(x * M_PI / 180.0); }},
        {"tan",  [](double x){ return std::tan(x * M_PI / 180.0); }},
        {"asin", [](double x){ return std::asin(x) * 180.0 / M_PI; }},
        {"acos", [](double x){ return std::acos(x) * 180.0 / M_PI; }},
        {"atan", [](double x){ return std::atan(x) * 180.0 / M_PI; }},
        {"sqrt", [](double x){ return std::sqrt(x); }},
        {"cbrt", [](double x){ return std::cbrt(x); }},
        {"log",  [](double x){ return std::log10(x); }},
        {"ln",   [](double x){ return std::log(x); }},
        {"exp",  [](double x){ return std::exp(x); }},
        {"abs",  [](double x){ return std::abs(x); }},
        {"ceil", [](double x){ return std::ceil(x); }},
        {"floor",[](double x){ return std::floor(x); }},
        {"round",[](double x){ return std::round(x); }},
        {"deg",  [](double x){ return x * 180.0 / M_PI; }},
        {"rad",  [](double x){ return x * M_PI / 180.0; }},
    };

    // Konstanta
    std::map<std::string, double> konstanta = {
        {"pi", M_PI},
        {"e",  M_E},
        {"phi", 1.6180339887},
        {"ans", 0.0},
    };

    double hitungDua(double a, char op, double b) {
        switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0) throw std::runtime_error("Pembagian dengan nol!");
            return a / b;
        case '%':
            if (b == 0) throw std::runtime_error("Modulo dengan nol!");
            return std::fmod(a, b);
        case '^': return std::pow(a, b);
        default:  throw std::runtime_error("Operator tidak dikenal");
        }
    }

    // Parser ekspresi sederhana: "num op num" atau "fungsi(num)"
    double evaluasi(const std::string& input) {
        std::string expr = input;
        // Ganti konstanta
        for (auto& [nama, nilai] : konstanta) {
            size_t pos;
            while ((pos = expr.find(nama)) != std::string::npos)
                expr.replace(pos, nama.size(), std::to_string(nilai));
        }

        // Cek fungsi: sin(45), sqrt(2), dll
        for (auto& [nama, fn] : fungsiSatu) {
            if (expr.substr(0, nama.size()) == nama &&
                expr.size() > nama.size() && expr[nama.size()] == '(') {
                // Ambil isi kurung
                size_t close = expr.rfind(')');
                if (close == std::string::npos)
                    throw std::runtime_error("Kurung tidak tertutup");
                std::string inner = expr.substr(nama.size() + 1,
                                                close - nama.size() - 1);
                double arg = evaluasi(inner);
                return fn(arg);
            }
        }

        // Cek operasi dua angka dengan operator +, -, *, /, ^
        // Cari operator dari kanan (supaya +/- dievaluasi setelah */^)
        for (char op : {'+', '-'}) {
            for (int i = (int)expr.size() - 1; i > 0; i--) {
                if (expr[i] == op && expr[i-1] != 'e' && expr[i-1] != 'E') {
                    double a = evaluasi(expr.substr(0, i));
                    double b = evaluasi(expr.substr(i + 1));
                    return hitungDua(a, op, b);
                }
            }
        }
        for (char op : {'*', '/', '%'}) {
            for (int i = (int)expr.size() - 1; i > 0; i--) {
                if (expr[i] == op) {
                    double a = evaluasi(expr.substr(0, i));
                    double b = evaluasi(expr.substr(i + 1));
                    return hitungDuo(a, op, b);
                }
            }
        }
        for (char op : {'^'}) {
            for (int i = (int)expr.size() - 1; i > 0; i--) {
                if (expr[i] == op) {
                    double a = evaluasi(expr.substr(0, i));
                    double b = evaluasi(expr.substr(i + 1));
                    return hitungDua(a, op, b);
                }
            }
        }

        // Angka tunggal
        try {
            return std::stod(expr);
        } catch (...) {
            throw std::runtime_error("Ekspresi tidak valid: '" + input + "'");
        }
    }

    double hitungDuo(double a, char op, double b) { return hitungDua(a, op, b); }

    void tambahRiwayat(const std::string& expr, double hasil) {
        riwayat.push_back({ expr, hasil, "" });
        konstanta["ans"] = hasil;
        if (riwayat.size() > 20) riwayat.erase(riwayat.begin());
    }

    void tampilRiwayat() const {
        if (riwayat.empty()) {
            std::cout << "  (belum ada riwayat)\n";
            return;
        }
        std::cout << "\n  Riwayat (" << riwayat.size() << " entri):\n";
        for (int i = 0; i < (int)riwayat.size(); i++)
            std::cout << "  " << std::setw(3) << (i+1) << ". " << riwayat[i].format() << "\n";
    }

    const std::vector<Riwayat>& getRiwayat() const { return riwayat; }

private:
    std::vector<Riwayat> riwayat;
};

// ============================================================
// Bantuan penggunaan
// ============================================================
void tampilBantuan() {
    std::cout << R"(
  ════════════════════════════════════════════════
   KALKULATOR ILMIAH C++ — Panduan Penggunaan
  ════════════════════════════════════════════════

  Operasi Dasar:
    3 + 4      → 7
    10 - 3     → 7
    6 * 7      → 42
    15 / 4     → 3.75
    17 % 5     → 2      (modulo)
    2 ^ 10     → 1024   (pangkat)

  Fungsi Matematika:
    sqrt(144)  → 12
    cbrt(27)   → 3
    sin(30)    → 0.5    (input dalam derajat)
    cos(60)    → 0.5
    tan(45)    → 1
    log(100)   → 2      (log base 10)
    ln(e)      → 1      (log natural)
    abs(-5)    → 5
    ceil(3.2)  → 4
    floor(3.8) → 3
    round(3.5) → 4

  Konstanta:
    pi         → 3.14159...
    e          → 2.71828...
    phi        → 1.61803... (golden ratio)
    ans        → hasil kalkulasi sebelumnya

  Perintah:
    riwayat    → tampilkan riwayat kalkulasi
    hapus      → hapus riwayat
    bantuan    → tampilkan panduan ini
    keluar     → tutup kalkulator

  Contoh lanjutan:
    2 ^ 3 + sqrt(16)    → 12
    sin(ans)            → sin dari hasil sebelumnya
  ════════════════════════════════════════════════
)";
}

// ============================================================
// Banner header
// ============================================================
void tampilHeader() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║    KALKULATOR ILMIAH C++  v1.0       ║\n";
    std::cout << "  ║  Ketik 'bantuan' untuk panduan       ║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n\n";
}

// ============================================================
// Hapus spasi di depan dan belakang
// ============================================================
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// Hapus semua spasi dari string
std::string hapusSpasi(const std::string& s) {
    std::string result;
    for (char c : s) if (c != ' ' && c != '\t') result += c;
    return result;
}

// ============================================================
// Main
// ============================================================
int main() {
    tampilHeader();

    Kalkulator kalk;
    std::string input;

    while (true) {
        std::cout << "  calc> ";
        if (!std::getline(std::cin, input)) break;
        input = trim(input);

        if (input.empty()) continue;

        // Perintah khusus
        if (input == "keluar" || input == "exit" || input == "quit") {
            std::cout << "  Sampai jumpa!\n\n";
            break;
        }
        if (input == "bantuan" || input == "help") {
            tampilBantuan();
            continue;
        }
        if (input == "riwayat" || input == "history") {
            kalk.tampilRiwayat();
            continue;
        }
        if (input == "hapus" || input == "clear") {
            std::cout << "\033[2J\033[H";  // clear terminal
            tampilHeader();
            continue;
        }

        // Hitung ekspresi
        try {
            std::string expr = hapusSpasi(input);
            double hasil = kalk.evaluasi(expr);
            kalk.tambahRiwayat(input, hasil);

            // Format output: jika hasil bulat, tampilkan tanpa desimal
            std::cout << "  → ";
            if (hasil == std::floor(hasil) && std::abs(hasil) < 1e15)
                std::cout << std::fixed << std::setprecision(0) << hasil << "\n";
            else
                std::cout << std::setprecision(10) << hasil << "\n";
        } catch (const std::exception& e) {
            std::cout << "  ✗ Error: " << e.what() << "\n";
        }
    }

    return 0;
}
