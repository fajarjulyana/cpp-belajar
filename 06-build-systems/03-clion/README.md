# Build Systems: CLion IDE

CLion adalah IDE C++ profesional dari JetBrains yang menggunakan CMake sebagai sistem build utamanya. Memberikan pengalaman pengembangan lengkap: code completion, debugging, refactoring, dan analisis kode.

---

## Instalasi

```bash
# Linux — snap
sudo snap install clion --classic

# Linux — tarball
# Download dari https://www.jetbrains.com/clion/download/
tar xfz CLion-*.tar.gz
./clion-*/bin/clion.sh

# macOS — Homebrew Cask
brew install --cask clion

# Windows
# Download installer dari https://www.jetbrains.com/clion/download/
```

> **Lisensi**: Berbayar (30 hari trial gratis). Mahasiswa/dosen mendapat lisensi gratis di jetbrains.com/student/

---

## Membuat Project Baru

```
File → New Project → C++ Executable
     ├── Language Standard: C++17 atau C++20
     ├── Toolchain: System (GCC/Clang)
     └── CMake: sesuaikan CMakeLists.txt
```

CLion otomatis membuat `CMakeLists.txt` dan struktur folder dasar.

---

## Antarmuka CLion

```
┌─────────────────────────────────────────────────────────┐
│  Menu Bar: File Edit View Build Run Tools VCS Help      │
├────────────┬────────────────────────────────────────────┤
│  Project   │                                            │
│  Explorer  │         Code Editor (Central)             │
│  (kiri)    │                                            │
│  ─────     │  - Syntax highlighting                    │
│  CMake     │  - Smart completion (Ctrl+Space)           │
│  Targets   │  - Live error detection                   │
│            │  - Parameter hints                        │
├────────────┴────────────────────────────────────────────┤
│  Build / Run / Debug Console (bawah)                   │
│  CMake / Terminal / Version Control                     │
└─────────────────────────────────────────────────────────┘
```

---

## Shortcut Penting

| Shortcut (Linux/Win) | Shortcut (macOS) | Fungsi |
|----------------------|------------------|--------|
| `Ctrl+Space`         | `⌃Space`         | Code completion |
| `Ctrl+Shift+Space`   | `⌃⇧Space`        | Smart completion |
| `Ctrl+B`             | `⌘B`             | Go to definition |
| `Ctrl+Alt+B`         | `⌘⌥B`            | Go to implementation |
| `Shift+F6`           | `⇧F6`            | Rename refactor |
| `Ctrl+Alt+L`         | `⌘⌥L`            | Format code |
| `Ctrl+/`             | `⌘/`             | Comment/uncomment |
| `Ctrl+D`             | `⌘D`             | Duplicate line |
| `Alt+Enter`          | `⌥Enter`         | Quick fix / intention |
| `Ctrl+Shift+F`       | `⌘⇧F`            | Find in all files |
| `Ctrl+Shift+R`       | `⌘⇧R`            | Replace in all files |
| `F9`                 | `F9`             | Run |
| `Shift+F9`           | `⇧F9`            | Debug |
| `F8`                 | `F8`             | Step over (debug) |
| `F7`                 | `F7`             | Step into (debug) |
| `Ctrl+F8`            | `⌘F8`            | Toggle breakpoint |
| `Alt+Insert`         | `⌘N`             | Generate code |
| `Ctrl+E`             | `⌘E`             | Recent files |
| `Ctrl+G`             | `⌘G`             | Go to line |
| `Ctrl+W`             | `⌥↑`             | Extend selection |

---

## Konfigurasi CMake di CLion

```
Settings → Build, Execution, Deployment → CMake
```

```
┌─────────────────────────────────────────────────┐
│ CMake Profile: Debug                             │
│  Build type    : Debug                           │
│  CMake options : -DCMAKE_VERBOSE_MAKEFILE=ON     │
│  Build dir     : cmake-build-debug               │
│  Environment   : MY_VAR=nilai                    │
├─────────────────────────────────────────────────┤
│ CMake Profile: Release                           │
│  Build type    : Release                         │
│  CMake options : -DCMAKE_BUILD_TYPE=Release      │
│  Build dir     : cmake-build-release             │
└─────────────────────────────────────────────────┘
```

---

## Debugging di CLion

### Breakpoint
```cpp
void fungsiContoh() {
    int x = 10;
    // Klik pada nomor baris → breakpoint merah
    int y = x * 2;    // ← program berhenti di sini
    int z = x + y;
}
```

### Debug Panel
```
Variables:
  x = 10          ← nilai variabel saat ini
  y = 20
  z = <not computed>

Call Stack:
  fungsiContoh()  ← frame aktif
  main()

Watches:
  x + y = 30      ← ekspresi kustom
  x * x = 100
```

### Conditional Breakpoint
```
Klik kanan breakpoint → Edit Breakpoint
Condition: i == 500 && value > 0.9
```

---

## Run/Debug Configuration

```
Run → Edit Configurations...
┌──────────────────────────────────────────────┐
│ Type: CMake Application                      │
│ Target: MyApp                                │
│ Executable: MyApp                            │
│ Program arguments: --file data.txt --verbose │
│ Working directory: $ProjectFileDir$          │
│ Environment: DEBUG=1;LOG_LEVEL=verbose       │
└──────────────────────────────────────────────┘
```

---

## Analisis Kode (Code Analysis)

CLion mendeteksi masalah secara real-time:

```cpp
int* p = nullptr;
*p = 5;          // ← CLion: "Null dereference"

char buf[10];
buf[10] = 'x';   // ← CLion: "Array index out of bounds"

int x;
return x;        // ← CLion: "Uninitialized variable"

if (a = b)       // ← CLion: "Assignment in condition"
```

---

## Refactoring

```
Rename (Shift+F6)       : ganti nama fungsi/variabel di seluruh project
Extract Function         : pindahkan blok kode ke fungsi baru
Inline                   : ganti pemanggilan fungsi dengan isinya
Change Signature         : ubah parameter fungsi
Move                     : pindah kode ke file lain
Extract Variable         : bungkus ekspresi dalam variabel baru
```

---

## Integrasi Version Control

```
VCS menu → Git
  ├── Commit (Ctrl+K)     → commit perubahan
  ├── Update Project (Ctrl+T) → git pull
  ├── Push (Ctrl+Shift+K) → push ke remote
  └── Branches            → buat/ganti branch

Log View → lihat semua commit dengan grafik
Annotate → tampilkan siapa yang menulis tiap baris (git blame)
```

---

## Plugin Berguna untuk CLion

| Plugin | Kegunaan |
|--------|----------|
| `.env files support` | Highlight file .env |
| `IdeaVim` | Vim key binding di CLion |
| `Rainbow Brackets` | Warna kurung berbeda |
| `Indent Rainbow` | Visualisasi indentasi |
| `GitHub Copilot` | AI code completion |
| `GLSL Support` | Shader language support |
| `CMake Plus` | Fitur CMake tambahan |

---

## Konfigurasi Clang-Tidy & Clang-Format

### .clang-format
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: None
BreakBeforeBraces: Allman
```

### .clang-tidy
```yaml
Checks: >
  -*, clang-analyzer-*, modernize-*, readability-*,
  performance-*, bugprone-*, cppcoreguidelines-*
WarningsAsErrors: ''
```

```
Settings → Editor → Code Style → C/C++
       → Scheme: Project (baca dari .clang-format)

Settings → Editor → Inspections → Clang-Tidy
       → aktifkan sesuai kebutuhan
```

---

## Profiling (Valgrind / Perf)

```
Run → Profile with Valgrind
  - Memory: deteksi memory leak, invalid access
  - Callgrind: profil waktu per fungsi

Run → CPU Profiler (built-in)
  - Flame graph
  - Call tree
  - Timeline
```

---

## Tips Produktivitas

```cpp
// 1. Live Templates (snippet) — ketik "for" lalu Tab
for (int i = 0; i < n; ++i) { /*cursor*/ }

// 2. Postfix Completion — ketik ekspresi lalu .
vec.for_      → for (auto& elem : vec) {}
x.if_         → if (x) {}
ptr.null_     → if (ptr == nullptr) {}

// 3. Structural Search — cari pattern kode
Ctrl+Shift+S → ketik: $x$ == null → temukan semua perbandingan null

// 4. Documentation Hover — hover fungsi untuk lihat docs
// 5. Parameter Name Hints — tampilkan nama param di call site
```

---

## Latihan

1. Buat project CMake baru di CLion dengan 2 source file
2. Set breakpoint di loop dan amati nilai variabel tiap iterasi
3. Gunakan Rename refactoring untuk mengganti nama fungsi
4. Aktifkan Clang-Tidy dan perbaiki semua warning yang muncul

---

**[← CMake](../02-cmake/README.md)** | **[Berikutnya → JUCE](../../07-audio-juce/01-pengenalan/README.md)**
