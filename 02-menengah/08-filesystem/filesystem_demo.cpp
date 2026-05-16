// ============================================================
// File  : filesystem_demo.cpp
// Topik : std::filesystem C++17
// Kompilasi: g++ -std=c++17 -Wall -o fs_demo filesystem_demo.cpp
// ============================================================
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;
namespace fs = std::filesystem;

void buatFileContoh(const fs::path& dir) {
    fs::create_directories(dir / "sub1");
    fs::create_directories(dir / "sub2" / "deep");
    for (auto& nama : {"a.cpp","b.cpp","c.txt"}) {
        ofstream f(dir / nama);
        f << "// Konten " << nama << "\n";
        for (int i = 0; i < 10; i++) f << "baris " << i << "\n";
    }
    ofstream f1(dir / "sub1" / "util.cpp");  f1 << "// util\n";
    ofstream f2(dir / "sub2" / "data.txt");  f2 << "data\n";
    ofstream f3(dir / "sub2" / "deep" / "nested.cpp"); f3 << "// nested\n";
}

void tampilkanTree(const fs::path& dir, int indent = 0) {
    string pad(indent * 2, ' ');
    for (const auto& entry : fs::directory_iterator(dir)) {
        cout << pad << "├─ " << entry.path().filename().string();
        if (entry.is_directory()) {
            cout << "/\n";
            tampilkanTree(entry.path(), indent + 1);
        } else {
            cout << " (" << entry.file_size() << "B)\n";
        }
    }
}

int main() {
    const fs::path sandbox = "fs_sandbox";

    // ---- Buat struktur direktori contoh ----
    cout << "=== Membuat Struktur Direktori ===" << endl;
    fs::remove_all(sandbox);  // bersihkan dulu
    buatFileContoh(sandbox);
    cout << "Dibuat: " << sandbox << "\n";

    // ---- Path Operations ----
    cout << "\n=== Path Operations ===" << endl;
    fs::path p = sandbox / "sub1" / "util.cpp";
    cout << "full path  : " << p.string()      << "\n";
    cout << "filename   : " << p.filename()    << "\n";
    cout << "stem       : " << p.stem()        << "\n";
    cout << "extension  : " << p.extension()   << "\n";
    cout << "parent     : " << p.parent_path() << "\n";
    cout << "is_absolute: " << boolalpha << p.is_absolute() << "\n";
    cout << "CWD        : " << fs::current_path() << "\n";

    // ---- Cek Keberadaan ----
    cout << "\n=== Cek Keberadaan ===" << endl;
    for (const auto& path : {sandbox / "a.cpp", fs::path("tidak_ada.xyz")}) {
        cout << path.filename() << ": ";
        cout << (fs::exists(path) ? "ADA" : "TIDAK ADA");
        if (fs::exists(path)) {
            cout << ", " << (fs::is_regular_file(path) ? "file" : "direktori");
            if (fs::is_regular_file(path))
                cout << ", " << fs::file_size(path) << " byte";
        }
        cout << "\n";
    }

    // ---- Tree View ----
    cout << "\n=== Struktur Direktori ===" << endl;
    cout << sandbox.string() << "/\n";
    tampilkanTree(sandbox);

    // ---- Iterasi Rekursif ----
    cout << "\n=== Semua File .cpp ===" << endl;
    uintmax_t totalCpp = 0;
    int jumlahCpp = 0;
    for (const auto& entry : fs::recursive_directory_iterator(sandbox)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
            cout << "  " << entry.path().string()
                 << " (" << entry.file_size() << "B)\n";
            totalCpp += entry.file_size();
            jumlahCpp++;
        }
    }
    cout << "Total: " << jumlahCpp << " file .cpp, " << totalCpp << " byte\n";

    // ---- Statistik per Ekstensi ----
    cout << "\n=== Statistik per Ekstensi ===" << endl;
    map<string, pair<int, uintmax_t>> stat;
    for (const auto& entry : fs::recursive_directory_iterator(sandbox)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext.empty()) ext = "(tanpa ekstensi)";
            stat[ext].first++;
            stat[ext].second += entry.file_size();
        }
    }
    for (const auto& [ext, info] : stat) {
        cout << "  " << ext << ": " << info.first
             << " file, " << info.second << " byte total\n";
    }

    // ---- Salin, Rename, Hapus ----
    cout << "\n=== Salin, Rename, Hapus ===" << endl;
    fs::path src = sandbox / "a.cpp";
    fs::path dst = sandbox / "a_backup.cpp";
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
    cout << "Disalin: " << src << " → " << dst << "\n";

    fs::path renamed = sandbox / "a_renamed.cpp";
    fs::rename(dst, renamed);
    cout << "Diubah nama: " << dst.filename() << " → " << renamed.filename() << "\n";

    fs::remove(renamed);
    cout << "Dihapus: " << renamed.filename() << "\n";

    // ---- Error Handling ----
    cout << "\n=== Error Handling ===" << endl;
    error_code ec;
    fs::copy_file("tidak_ada.txt", "tujuan.txt", ec);
    if (ec) cout << "Error (error_code): " << ec.message() << "\n";

    try {
        fs::remove_all("tidak_ada_sama_sekali_xyz");  // tidak error jika tidak ada
        fs::file_size("tidak_ada_123.txt");  // ini baru error!
    } catch (const fs::filesystem_error& e) {
        cout << "filesystem_error: " << e.what() << "\n";
        cout << "path1: " << e.path1() << "\n";
    }

    // ---- Disk Space ----
    cout << "\n=== Informasi Disk ===" << endl;
    auto space = fs::space(fs::current_path());
    cout << "Kapasitas : " << space.capacity  / (1024*1024) << " MB\n";
    cout << "Tersedia  : " << space.available / (1024*1024) << " MB\n";

    // ---- Cleanup ----
    fs::remove_all(sandbox);
    cout << "\nSandbox dibersihkan.\n";

    return 0;
}
