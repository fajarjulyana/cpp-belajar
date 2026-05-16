// ============================================================
// File  : konverter.cpp
// Proyek: Konverter Unit Lengkap
//
// Kompilasi:
//   g++ -std=c++17 -Wall -o konverter konverter.cpp
// Jalankan:
//   ./konverter
// ============================================================

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <sstream>

// ============================================================
// Sistem Konversi Universal
// Semua satuan dikonversi melalui satuan SI sebagai basis
// ============================================================

struct KategoriKonversi {
    std::string nama;
    std::string satuanSI;
    // Nama satuan → faktor ke SI (nilai 1 satuan ini = ? SI)
    std::map<std::string, double> faktor;
    // Nama satuan → pasangan (dari SI ke satuan ini)
    std::map<std::string, std::function<double(double)>> fromSI;
    std::map<std::string, std::function<double(double)>> toSI;
};

// ============================================================
// Definisi Konversi
// ============================================================

KategoriKonversi buatSuhu() {
    KategoriKonversi k;
    k.nama    = "Suhu";
    k.satuanSI = "Kelvin (K)";

    // Suhu tidak bisa pakai faktor linear biasa → pakai fungsi
    k.toSI["C"]  = [](double v){ return v + 273.15; };
    k.toSI["F"]  = [](double v){ return (v - 32.0) * 5.0 / 9.0 + 273.15; };
    k.toSI["K"]  = [](double v){ return v; };
    k.toSI["R"]  = [](double v){ return v * 5.0 / 9.0; };       // Rankine

    k.fromSI["C"] = [](double k){ return k - 273.15; };
    k.fromSI["F"] = [](double k){ return (k - 273.15) * 9.0 / 5.0 + 32.0; };
    k.fromSI["K"] = [](double k){ return k; };
    k.fromSI["R"] = [](double k){ return k * 9.0 / 5.0; };

    k.faktor = {{"C",1},{"F",1},{"K",1},{"R",1}};
    return k;
}

KategoriKonversi buatPanjang() {
    KategoriKonversi k;
    k.nama     = "Panjang";
    k.satuanSI  = "Meter (m)";

    k.faktor = {
        {"km",   1000.0},
        {"m",    1.0},
        {"cm",   0.01},
        {"mm",   0.001},
        {"um",   1e-6},      // mikrometer
        {"nm",   1e-9},      // nanometer
        {"mile", 1609.344},
        {"yard", 0.9144},
        {"feet", 0.3048},
        {"inch", 0.0254},
        {"nmi",  1852.0},    // nautical mile
        {"ly",   9.461e15},  // light year
        {"au",   1.496e11},  // astronomical unit
    };

    for (auto& [satuan, f] : k.faktor) {
        double faktor = f;
        k.toSI[satuan]   = [faktor](double v){ return v * faktor; };
        k.fromSI[satuan] = [faktor](double si){ return si / faktor; };
    }
    return k;
}

KategoriKonversi buatBerat() {
    KategoriKonversi k;
    k.nama    = "Berat / Massa";
    k.satuanSI = "Kilogram (kg)";

    k.faktor = {
        {"kg",   1.0},
        {"g",    0.001},
        {"mg",   1e-6},
        {"ton",  1000.0},
        {"lb",   0.453592},   // pound
        {"oz",   0.0283495},  // ounce
        {"grain",6.35e-5},
        {"carat",0.0002},
    };

    for (auto& [satuan, f] : k.faktor) {
        double faktor = f;
        k.toSI[satuan]   = [faktor](double v){ return v * faktor; };
        k.fromSI[satuan] = [faktor](double si){ return si / faktor; };
    }
    return k;
}

KategoriKonversi buatVolume() {
    KategoriKonversi k;
    k.nama    = "Volume";
    k.satuanSI = "Liter (L)";

    k.faktor = {
        {"L",    1.0},
        {"mL",   0.001},
        {"m3",   1000.0},
        {"cm3",  0.001},       // cc
        {"galUS",3.78541},     // US gallon
        {"galUK",4.54609},     // UK gallon
        {"qt",   0.946353},    // quart
        {"pt",   0.473176},    // pint
        {"cup",  0.236588},
        {"floz", 0.0295735},   // fluid ounce
        {"tbsp", 0.0147868},   // tablespoon
        {"tsp",  0.00492892},  // teaspoon
    };

    for (auto& [satuan, f] : k.faktor) {
        double faktor = f;
        k.toSI[satuan]   = [faktor](double v){ return v * faktor; };
        k.fromSI[satuan] = [faktor](double si){ return si / faktor; };
    }
    return k;
}

KategoriKonversi buatKecepatan() {
    KategoriKonversi k;
    k.nama    = "Kecepatan";
    k.satuanSI = "Meter per detik (m/s)";

    k.faktor = {
        {"m/s",  1.0},
        {"km/h", 1.0/3.6},
        {"mph",  0.44704},
        {"knot", 0.514444},
        {"fps",  0.3048},      // feet per second
        {"mach", 340.29},      // kecepatan suara (15°C, laut)
        {"c",    299792458.0}, // kecepatan cahaya
    };

    for (auto& [satuan, f] : k.faktor) {
        double faktor = f;
        k.toSI[satuan]   = [faktor](double v){ return v * faktor; };
        k.fromSI[satuan] = [faktor](double si){ return si / faktor; };
    }
    return k;
}

KategoriKonversi buatData() {
    KategoriKonversi k;
    k.nama    = "Data Digital";
    k.satuanSI = "Bit (b)";

    k.faktor = {
        {"b",   1.0},
        {"B",   8.0},          // Byte
        {"KB",  8.0*1024},
        {"MB",  8.0*1024*1024},
        {"GB",  8.0*1024*1024*1024},
        {"TB",  8.0*1024*1024*1024*1024},
        {"KiB", 8.0*1024},
        {"Kbps",1000.0},
        {"Mbps",1e6},
    };

    for (auto& [satuan, f] : k.faktor) {
        double faktor = f;
        k.toSI[satuan]   = [faktor](double v){ return v * faktor; };
        k.fromSI[satuan] = [faktor](double si){ return si / faktor; };
    }
    return k;
}

// ============================================================
// Engine Konversi
// ============================================================
class KonverterEngine {
    std::map<std::string, KategoriKonversi> kategori;
public:
    KonverterEngine() {
        auto suhu  = buatSuhu();
        auto panjang = buatPanjang();
        auto berat = buatBerat();
        auto vol   = buatVolume();
        auto kec   = buatKecepatan();
        auto data  = buatData();

        kategori[suhu.nama]     = suhu;
        kategori[panjang.nama]  = panjang;
        kategori[berat.nama]    = berat;
        kategori[vol.nama]      = vol;
        kategori[kec.nama]      = kec;
        kategori[data.nama]     = data;
    }

    double konversi(const std::string& katNama,
                    const std::string& dari,
                    const std::string& ke,
                    double nilai) {
        auto it = kategori.find(katNama);
        if (it == kategori.end()) throw std::runtime_error("Kategori tidak ditemukan");
        auto& k = it->second;

        if (k.toSI.find(dari) == k.toSI.end())
            throw std::runtime_error("Satuan asal '" + dari + "' tidak dikenal");
        if (k.fromSI.find(ke) == k.fromSI.end())
            throw std::runtime_error("Satuan tujuan '" + ke + "' tidak dikenal");

        double si = k.toSI.at(dari)(nilai);
        return k.fromSI.at(ke)(si);
    }

    // Tampilkan semua satuan untuk kategori
    void tampilSatuan(const std::string& katNama) const {
        auto it = kategori.find(katNama);
        if (it == kategori.end()) return;
        std::cout << "\n  Satuan " << katNama << " (" << it->second.satuanSI << "):\n  ";
        int i = 0;
        for (const auto& [s, _] : it->second.faktor) {
            std::cout << std::setw(8) << s;
            if (++i % 8 == 0) std::cout << "\n  ";
        }
        std::cout << "\n";
    }

    // Konversi ke semua satuan sekaligus
    void tampilSemua(const std::string& katNama,
                     const std::string& dari, double nilai) {
        auto it = kategori.find(katNama);
        if (it == kategori.end()) return;
        auto& k = it->second;

        if (k.toSI.find(dari) == k.toSI.end()) {
            std::cout << "  ✗ Satuan '" << dari << "' tidak dikenal\n";
            return;
        }

        double si = k.toSI.at(dari)(nilai);
        std::cout << "\n  " << nilai << " " << dari << " = :\n";
        for (const auto& [satuan, _] : k.faktor) {
            if (satuan == dari) continue;
            double hasil = k.fromSI.at(satuan)(si);
            std::cout << "  " << std::setw(14) << std::right
                      << std::setprecision(6) << hasil << " " << satuan << "\n";
        }
    }

    const std::map<std::string, KategoriKonversi>& getKategori() const { return kategori; }
};

// ============================================================
// UI
// ============================================================
void tampilHeader() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║   KONVERTER UNIT LENGKAP  v1.0       ║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
}

std::string inputStr(const std::string& p) {
    std::cout << p;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

double inputDouble(const std::string& p) {
    while (true) {
        std::cout << p;
        std::string s;
        std::getline(std::cin, s);
        try { return std::stod(s); }
        catch(...) { std::cout << "  Input tidak valid. Masukkan angka.\n"; }
    }
}

int main() {
    tampilHeader();
    KonverterEngine engine;

    while (true) {
        // Menu kategori
        std::cout << "\n  Pilih kategori:\n";
        int i = 1;
        std::vector<std::string> namaKat;
        for (const auto& [nama, _] : engine.getKategori()) {
            std::cout << "  [" << i++ << "] " << nama << "\n";
            namaKat.push_back(nama);
        }
        std::cout << "  [0] Keluar\n";
        std::string pil = inputStr("  Pilih: ");
        if (pil == "0") { std::cout << "  Sampai jumpa!\n\n"; break; }

        int pilIdx = -1;
        try { pilIdx = std::stoi(pil) - 1; } catch(...) {}
        if (pilIdx < 0 || pilIdx >= (int)namaKat.size()) {
            std::cout << "  Pilihan tidak valid\n"; continue;
        }

        std::string katNama = namaKat[pilIdx];
        engine.tampilSatuan(katNama);

        std::string dari = inputStr("  Dari satuan: ");
        double nilai = inputDouble("  Nilai: ");
        std::string ke = inputStr("  Ke satuan (kosong = tampilkan semua): ");

        if (ke.empty()) {
            engine.tampilSemua(katNama, dari, nilai);
        } else {
            try {
                double hasil = engine.konversi(katNama, dari, ke, nilai);
                std::cout << "\n  " << std::setprecision(10) << nilai
                          << " " << dari << " = " << hasil << " " << ke << "\n";
            } catch (const std::exception& e) {
                std::cout << "  ✗ " << e.what() << "\n";
            }
        }
    }

    return 0;
}
