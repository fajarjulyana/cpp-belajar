# Bab 18 — Filesystem & Modern I/O (C++17)

`std::filesystem` (C++17) menyediakan API modern untuk operasi file dan direktori lintas platform.

---

## Setup

```cpp
#include <filesystem>
namespace fs = std::filesystem;  // alias praktis
```

---

## Operasi Path

```cpp
fs::path p1 = "/home/budi/dokumen";
fs::path p2 = "file.txt";
fs::path gabung = p1 / p2;  // operator / untuk join path

cout << gabung.string()      << "\n";  // /home/budi/dokumen/file.txt
cout << gabung.filename()    << "\n";  // file.txt
cout << gabung.stem()        << "\n";  // file (tanpa ekstensi)
cout << gabung.extension()   << "\n";  // .txt
cout << gabung.parent_path() << "\n";  // /home/budi/dokumen
cout << gabung.root_path()   << "\n";  // /
cout << gabung.is_absolute() << "\n";  // true
cout << gabung.is_relative() << "\n";  // false

// Current directory
fs::path cwd = fs::current_path();
cout << "CWD: " << cwd << "\n";
```

---

## Cek Keberadaan & Tipe

```cpp
fs::path p = "file.txt";

// Cek keberadaan
if (fs::exists(p))          cout << "Ada\n";
if (fs::is_regular_file(p)) cout << "Ini file\n";
if (fs::is_directory(p))    cout << "Ini direktori\n";
if (fs::is_symlink(p))      cout << "Ini symlink\n";

// Ukuran file
auto ukuran = fs::file_size(p);  // dalam byte

// Waktu modifikasi
auto waktu = fs::last_write_time(p);
```

---

## Manipulasi File & Direktori

```cpp
// Buat direktori
fs::create_directory("direktori_baru");
fs::create_directories("a/b/c/d");  // buat bersama parent

// Salin
fs::copy_file("sumber.txt", "tujuan.txt");
fs::copy_file("sumber.txt", "tujuan.txt", fs::copy_options::overwrite_existing);
fs::copy("direktori_asal", "direktori_tujuan", fs::copy_options::recursive);

// Pindah/Rename
fs::rename("lama.txt", "baru.txt");
fs::rename("dir_lama", "dir_baru");

// Hapus
fs::remove("file.txt");             // hapus satu file
fs::remove_all("direktori");        // hapus rekursif

// Ubah ukuran file
fs::resize_file("file.txt", 1024);

// Absolute path
fs::path abs = fs::absolute("relatif/path");
fs::path kanonik = fs::canonical("./../../file.txt");  // resolve ..
```

---

## Iterasi Direktori

```cpp
// Iterasi satu level
for (const auto& entry : fs::directory_iterator(".")) {
    cout << entry.path().filename().string();
    if (entry.is_directory()) cout << "/";
    cout << "\n";
}

// Iterasi rekursif
for (const auto& entry : fs::recursive_directory_iterator(".")) {
    // Hitung indentasi berdasarkan kedalaman
    auto depth = distance(entry.path().begin(), entry.path().end());
    cout << string(depth * 2, ' ') << entry.path().filename().string();
    if (entry.is_directory()) cout << "/";
    else cout << " (" << entry.file_size() << " byte)";
    cout << "\n";
}

// Filter berdasarkan ekstensi
for (const auto& entry : fs::recursive_directory_iterator(".")) {
    if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
        cout << entry.path().string() << "\n";
    }
}
```

---

## `std::filesystem::space` — Informasi Disk

```cpp
auto info = fs::space("/");
cout << "Total : " << info.capacity / (1024*1024*1024) << " GB\n";
cout << "Bebas : " << info.available / (1024*1024*1024) << " GB\n";
cout << "Terpakai: " << (info.capacity - info.free) / (1024*1024*1024) << " GB\n";
```

---

## Error Handling

```cpp
// Cara 1: exception
try {
    fs::copy_file("tidak_ada.txt", "tujuan.txt");
} catch (const fs::filesystem_error& e) {
    cerr << "Error: " << e.what() << "\n";
    cerr << "Path1: " << e.path1() << "\n";
    cerr << "Path2: " << e.path2() << "\n";
}

// Cara 2: error_code (tidak melempar exception)
error_code ec;
fs::copy_file("tidak_ada.txt", "tujuan.txt", ec);
if (ec) cerr << "Error: " << ec.message() << "\n";
```

---

## Compile

```bash
# C++17 diperlukan
g++ -std=c++17 -Wall -o filesystem_demo filesystem_demo.cpp

# Windows MSVC
cl /std:c++17 filesystem_demo.cpp
```

---

## Latihan

1. Buat program yang menghitung total ukuran semua file .cpp di direktori saat ini
2. Buat program backup yang menyalin semua file dengan ekstensi tertentu ke direktori backup
3. Buat program yang mencari duplikat file berdasarkan ukuran
4. Implementasikan `tree` command sederhana (tampilkan struktur direktori)

---

**[← Multithreading](../07-multithreading/README.md)** | **[→ Lanjut ke Ahli: Design Patterns](../../03-ahli/01-design-patterns/README.md)**
