#!/bin/bash
# ============================================================
# build_static.sh — Membuat dan menggunakan Static Library (.a)
# ============================================================
set -e

echo ""
echo "╔══════════════════════════════════════════════╗"
echo "║  BUILD: Static Library (.a)                  ║"
echo "╚══════════════════════════════════════════════╝"
echo ""

# Pindah ke direktori script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# ── Langkah 1: Kompilasi source ke object file ──
echo "[1/4] Kompilasi matematika.cpp → matematika.o"
g++ -std=c++17 -Wall -O2 -c matematika.cpp -o matematika.o
echo "      Ukuran: $(du -h matematika.o | cut -f1)"

# ── Langkah 2: Buat static library dengan ar ──
echo ""
echo "[2/4] Buat static library dengan ar rcs"
ar rcs libmatematika.a matematika.o
echo "      Ukuran libmatematika.a: $(du -h libmatematika.a | cut -f1)"

# Lihat isi library
echo ""
echo "      Simbol dalam libmatematika.a:"
nm libmatematika.a | grep -E "^[0-9a-f]+ T " | head -12 | sed 's/^/        /'

# ── Langkah 3: Kompilasi program utama dengan library ──
echo ""
echo "[3/4] Kompilasi main_static.cpp + libmatematika.a → program_static"
g++ -std=c++17 -Wall -O2 -o program_static main_static.cpp -L. -lmatematika
echo "      Ukuran program: $(du -h program_static | cut -f1)"

# Verifikasi tidak ada dependency .so untuk matematika
echo ""
echo "      Dependency (.so) program_static:"
ldd program_static 2>/dev/null | grep -v "matematika" | sed 's/^/        /'
echo "      → libmatematika.a SUDAH disertakan ke dalam executable!"

# ── Langkah 4: Jalankan ──
echo ""
echo "[4/4] Menjalankan program_static..."
echo ""
./program_static

# Bersihkan file intermediate (opsional)
# rm -f matematika.o libmatematika.a program_static

echo ""
echo "File yang dihasilkan:"
echo "  matematika.o      — Object file"
echo "  libmatematika.a   — Static library"
echo "  program_static    — Executable (semua kode sudah di dalam)"
