# Bab 1 — Pengenalan C++ & Setup Lingkungan

## Apa itu C++?

C++ adalah bahasa pemrograman **compiled**, **strongly-typed**, dan **multi-paradigma** yang dibuat oleh **Bjarne Stroustrup** pada tahun 1985 sebagai perluasan dari bahasa C. C++ mendukung:

- **Pemrograman Prosedural** — seperti C
- **Pemrograman Berorientasi Objek (OOP)** — class, inheritance, polymorphism
- **Pemrograman Generic** — template
- **Pemrograman Fungsional** — lambda, function objects

---

## Mengapa Belajar C++?

| Alasan | Penjelasan |
|--------|-----------|
| Performa tinggi | Kode dikompilasi langsung ke mesin |
| Kontrol memori | Manajemen memori manual maupun otomatis |
| Dipakai industri | Game, sistem operasi, embedded, audio/video |
| Fondasi bahasa lain | Rust, Java, C# terinspirasi dari C++ |
| Komunitas besar | Banyak library dan resource |

---

## Setup Lingkungan

### 🐧 Linux (Ubuntu/Debian/Mint)

```bash
# Update paket
sudo apt update && sudo apt upgrade -y

# Install compiler dan tools
sudo apt install -y build-essential g++ gdb cmake make git

# Verifikasi
g++ --version
cmake --version
gdb --version
```

**Hasil yang diharapkan:**
```
g++ (Ubuntu 12.3.0-1ubuntu1~23.04) 12.3.0
```

### 🐧 Linux (Arch/Manjaro)

```bash
sudo pacman -Syu base-devel g++ gdb cmake
```

### 🍎 macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (jika belum ada)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools tambahan
brew install cmake llvm

# Verifikasi
g++ --version   # mengarah ke clang di macOS
clang++ --version
cmake --version
```

> **Catatan macOS:** Perintah `g++` di macOS sebenarnya adalah alias untuk `clang++` (LLVM). Keduanya kompatibel dengan standar C++17/20.

### 🪟 Windows

#### Opsi 1: MinGW-w64 (Gratis, Ringan)
1. Download **MSYS2** dari https://www.msys2.org/
2. Jalankan installer, buka terminal MSYS2
3. Jalankan:
```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-gdb
```
4. Tambahkan `C:\msys64\mingw64\bin` ke PATH

#### Opsi 2: Visual Studio (MSVC)
1. Download **Visual Studio 2022 Community** (gratis) dari https://visualstudio.microsoft.com/
2. Pilih workload: **"Desktop development with C++"**
3. Compiler `cl.exe` otomatis tersedia

#### Opsi 3: WSL2 (Windows Subsystem for Linux)
```powershell
# Di PowerShell (Admin)
wsl --install
# Restart, lalu ikuti setup Linux di atas
```

---

## Editor & IDE yang Direkomendasikan

| Editor/IDE | Platform | Harga | Keterangan |
|-----------|----------|-------|-----------|
| **VS Code** + C++ Extension | Semua | Gratis | Ringan, populer |
| **CLion** | Semua | Berbayar (gratis untuk pelajar) | IDE khusus C++ terbaik |
| **Visual Studio** | Windows | Gratis (Community) | IDE Microsoft |
| **Code::Blocks** | Semua | Gratis | Sederhana, bagus untuk pemula |
| **Qt Creator** | Semua | Gratis | Bagus untuk Qt dan C++ umum |
| **Vim/Neovim** + clangd | Semua | Gratis | Untuk pengguna advanced |

### Setup VS Code untuk C++

1. Install **VS Code**: https://code.visualstudio.com/
2. Install ekstensi **C/C++** (Microsoft) dari Marketplace
3. Install ekstensi **CMake Tools**
4. Buat file `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "Linux",
            "compilerPath": "/usr/bin/g++",
            "cppStandard": "c++17",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```

---

## Program Pertama: Hello, World!

Buat file `hello.cpp`:

```cpp
#include <iostream>   // library input/output standar

int main() {
    // Mencetak teks ke layar, endl = akhiri baris
    std::cout << "Halo, Dunia!" << std::endl;
    return 0;  // 0 = program berjalan sukses
}
```

### Kompilasi dan Jalankan

**Linux / macOS:**
```bash
g++ -std=c++17 -o hello hello.cpp
./hello
```

**Windows (MinGW):**
```bash
g++ -std=c++17 -o hello.exe hello.cpp
hello.exe
```

**Windows (MSVC - di Developer Command Prompt):**
```bash
cl /std:c++17 hello.cpp
hello.exe
```

**Output:**
```
Halo, Dunia!
```

---

## Anatomi Program C++

```cpp
// [1] Preprocessor directive — menyertakan library
#include <iostream>
#include <string>

// [2] Namespace — menghindari konflik nama
using namespace std;

// [3] Fungsi main — titik masuk program
int main() {
    // [4] Statement — instruksi yang dieksekusi
    string nama = "Budi";
    
    // [5] Output ke layar
    cout << "Halo, " << nama << "!" << endl;
    
    // [6] Return value — 0 = sukses
    return 0;
}
```

| Elemen | Penjelasan |
|--------|-----------|
| `#include` | Menyertakan file header library |
| `using namespace std` | Tidak perlu menulis `std::` setiap saat |
| `int main()` | Fungsi utama, wajib ada di setiap program |
| `cout` | Console output — cetak ke layar |
| `cin` | Console input — baca dari keyboard |
| `endl` | End line — pindah baris (sama dengan `\n`) |
| `return 0` | Memberi tahu OS program selesai sukses |

---

## Proses Kompilasi

```
Kode Sumber (.cpp)
       │
       ▼
  Preprocessor  ──→  Mengganti #include, #define, dll
       │
       ▼
   Compiler     ──→  Mengubah ke Assembly (.s)
       │
       ▼
   Assembler    ──→  Mengubah ke Object Code (.o / .obj)
       │
       ▼
    Linker       ──→  Menggabungkan semua .o + library
       │
       ▼
  Executable    ──→  ./program atau program.exe
```

### Flag Kompilasi Penting

```bash
g++ -std=c++17      # Gunakan standar C++17
g++ -std=c++20      # Gunakan standar C++20
g++ -Wall           # Tampilkan semua peringatan
g++ -Wextra         # Peringatan tambahan
g++ -g              # Sertakan info debug (untuk gdb)
g++ -O0             # Tanpa optimasi (default debug)
g++ -O2             # Optimasi level 2 (rekomendasi produksi)
g++ -O3             # Optimasi agresif
g++ -o namafile     # Tentukan nama output
```

**Contoh lengkap:**
```bash
g++ -std=c++17 -Wall -Wextra -g -o hello hello.cpp
```

---

## Komentar dalam C++

```cpp
// Ini adalah komentar satu baris

/*
   Ini adalah komentar
   multi-baris
*/

/**
 * Komentar dokumentasi (Doxygen style)
 * @param nama Parameter nama pengguna
 * @return Tidak mengembalikan nilai
 */
void sapa(std::string nama) {
    std::cout << "Halo " << nama << std::endl;
}
```

---

## Latihan

1. Buat program yang mencetak nama Anda
2. Buat program yang mencetak 5 baris teks berbeda
3. Coba kompilasi dengan flag `-Wall` dan amati peringatan

---

**[Berikutnya → Variabel & Tipe Data](../02-variabel-tipe-data/README.md)**
