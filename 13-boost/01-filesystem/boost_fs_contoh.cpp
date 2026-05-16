// ============================================================
// File  : boost_fs_contoh.cpp
// Topik : Boost.Filesystem — Setara std::filesystem (C++17)
//
// Menunjukkan API Boost.Filesystem DAN setara std::filesystem.
// File ini dikompilasi dengan std::filesystem (C++17).
//
// Boost nyata:
//   #include <boost/filesystem.hpp>
//   namespace fs = boost::filesystem;
//   Kompilasi: g++ -std=c++17 -lboost_filesystem -lboost_system -o prog main.cpp
//
// std::filesystem (C++17):
//   #include <filesystem>
//   namespace fs = std::filesystem;
//   Kompilasi: g++ -std=c++17 -o prog main.cpp
//
// Kompilasi file ini:
//   g++ -std=c++17 -o boost_fs boost_fs_contoh.cpp
// ============================================================

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

// ============================================================
// Utilitas
// ============================================================
void judulBagian(const std::string& j) {
    std::cout << "\n\033[1;34m── " << j << " ──\033[0m\n";
}

std::string ukuranRapi(uintmax_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024*1024) return std::to_string(bytes/1024) + " KB";
    return std::to_string(bytes/(1024*1024)) + " MB";
}

// ============================================================
// Demo 1: Path Operations
// ============================================================
void demoPath() {
    judulBagian("Path Operations");

    // Boost: boost::filesystem::path p("dir/sub/file.txt");
    // Std:   std::filesystem::path  p("dir/sub/file.txt");
    fs::path p("/home/pengguna/proyek/src/main.cpp");

    std::cout << "  path         : " << p << "\n";
    std::cout << "  filename     : " << p.filename() << "\n";
    std::cout << "  stem         : " << p.stem() << "\n";
    std::cout << "  extension    : " << p.extension() << "\n";
    std::cout << "  parent_path  : " << p.parent_path() << "\n";
    std::cout << "  root_path    : " << p.root_path() << "\n";
    std::cout << "  relative_path: " << p.relative_path() << "\n";

    // Gabung path
    fs::path dir = "/home/pengguna";
    fs::path file = dir / "dokumen" / "laporan.pdf";
    std::cout << "\n  Gabung: " << dir << " / dokumen / laporan.pdf\n";
    std::cout << "  Hasil : " << file << "\n";

    // Cek ekstensi
    std::vector<std::string> files = {
        "main.cpp", "readme.md", "data.json", "foto.png", "CMakeLists.txt"
    };
    std::cout << "\n  Filter file C++:\n";
    for (auto& f : files) {
        fs::path fp(f);
        if (fp.extension() == ".cpp" || fp.extension() == ".h")
            std::cout << "    ✓ " << f << "\n";
    }
}

// ============================================================
// Demo 2: Operasi File & Direktori
// ============================================================
void demoFileOps() {
    judulBagian("Operasi File & Direktori (menggunakan /tmp)");

    fs::path tmpDir = fs::temp_directory_path() / "cpp_belajar_demo";

    // Buat direktori
    // Boost: boost::filesystem::create_directories(tmpDir);
    fs::create_directories(tmpDir);
    std::cout << "  Buat direktori: " << tmpDir << "\n";

    // Buat beberapa file
    std::vector<std::string> namaFile = {
        "catatan.txt", "data.csv", "config.json", "laporan.md"
    };
    for (auto& nama : namaFile) {
        std::ofstream ofs(tmpDir / nama);
        ofs << "Isi file " << nama << "\n";
        ofs << "Dibuat untuk demo Boost.Filesystem\n";
    }
    std::cout << "  Buat " << namaFile.size() << " file contoh\n";

    // Cek keberadaan
    fs::path cek = tmpDir / "data.csv";
    std::cout << "\n  exists(data.csv)    : "
              << (fs::exists(cek) ? "ya" : "tidak") << "\n";
    std::cout << "  is_regular_file     : "
              << (fs::is_regular_file(cek) ? "ya" : "tidak") << "\n";
    std::cout << "  is_directory(tmpDir): "
              << (fs::is_directory(tmpDir) ? "ya" : "tidak") << "\n";
    std::cout << "  file_size(data.csv) : "
              << ukuranRapi(fs::file_size(cek)) << "\n";

    // Salin file
    fs::copy_file(tmpDir / "catatan.txt", tmpDir / "catatan_backup.txt",
                  fs::copy_options::overwrite_existing);
    std::cout << "\n  Salin catatan.txt → catatan_backup.txt\n";

    // Ganti nama
    fs::rename(tmpDir / "catatan_backup.txt", tmpDir / "catatan_v2.txt");
    std::cout << "  Rename → catatan_v2.txt\n";

    // Ukuran direktori
    uintmax_t totalUkuran = 0;
    for (auto& entry : fs::directory_iterator(tmpDir))
        if (fs::is_regular_file(entry))
            totalUkuran += fs::file_size(entry);
    std::cout << "  Total ukuran direktori: " << ukuranRapi(totalUkuran) << "\n";

    // Hapus direktori beserta isinya
    fs::remove_all(tmpDir);
    std::cout << "  Hapus direktori (remove_all): selesai\n";
}

// ============================================================
// Demo 3: Iterasi Direktori
// ============================================================
void demoIterasi() {
    judulBagian("Iterasi Direktori");

    fs::path dir = "/tmp";
    std::cout << "  Isi /tmp (maks 10 file):\n";

    int no = 1;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (no > 10) { std::cout << "  ...\n"; break; }
        std::string tipe = entry.is_directory() ? "[DIR] "
                         : entry.is_regular_file() ? "[FILE]"
                         : "[?]   ";
        std::cout << "  " << no++ << ". " << tipe << " "
                  << entry.path().filename().string();
        if (entry.is_regular_file()) {
            try {
                std::cout << " (" << ukuranRapi(fs::file_size(entry)) << ")";
            } catch (...) {}
        }
        std::cout << "\n";
    }

    // Recursive iteration — hitung file berdasarkan ekstensi
    judulBagian("Recursive Iterator — Statistik Ekstensi");
    fs::path srcDir = "/home/runner/workspace/cpp-belajar";
    if (!fs::exists(srcDir)) srcDir = "/tmp";

    std::map<std::string, int> ekst;
    int totalFile = 0;
    try {
        for (auto& entry : fs::recursive_directory_iterator(
                srcDir, fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                ++totalFile;
                ++ekst[entry.path().extension().string()];
            }
        }
    } catch (...) {}

    std::cout << "  Direktori: " << srcDir << "\n";
    std::cout << "  Total file: " << totalFile << "\n";
    std::cout << "  Per ekstensi:\n";
    // Urutkan berdasarkan jumlah
    std::vector<std::pair<int,std::string>> sorted;
    for (auto& [e, c] : ekst) sorted.push_back(std::make_pair(c, e.empty() ? std::string("(tanpa)") : e));
    std::sort(sorted.rbegin(), sorted.rend());
    for (auto& [c, e] : sorted)
        std::cout << "    " << std::setw(8) << e << " : " << c << " file\n";
}

// ============================================================
// Demo 4: Path Relative & Absolute
// ============================================================
void demoPathConvert() {
    judulBagian("Path Relative ↔ Absolute");

    fs::path abs = fs::current_path();
    std::cout << "  current_path    : " << abs << "\n";
    std::cout << "  absolute(\".\")   : " << fs::absolute(".") << "\n";

    fs::path rel = fs::relative(abs / "src" / "main.cpp", abs);
    std::cout << "  relative path   : " << rel << "\n";

    // Boost: boost::filesystem::canonical(p) — resolves symlinks
    try {
        fs::path canon = fs::canonical("/tmp");
        std::cout << "  canonical(/tmp) : " << canon << "\n";
    } catch (...) {}
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n\033[1m"
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Boost.Filesystem ≈ std::filesystem (C++17) ║\n"
              << "║  API hampir identik — mudah migrasi          ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << "\033[0m";

    demoPath();
    demoFileOps();
    demoIterasi();
    demoPathConvert();

    std::cout << "\n\033[1;33mBoost vs std::filesystem:\033[0m\n";
    std::cout << "  boost::filesystem::path  ≈  std::filesystem::path\n";
    std::cout << "  boost::filesystem::exists ≈  std::filesystem::exists\n";
    std::cout << "  API hampir 1:1 — ganti namespace saja!\n\n";
    return 0;
}
