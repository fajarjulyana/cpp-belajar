#!/bin/bash
# ============================================================
# build_dynamic.sh — Membuat dan menggunakan Shared Library (.so)
# ============================================================
set -e

echo ""
echo "╔══════════════════════════════════════════════╗"
echo "║  BUILD: Shared Library (.so)                 ║"
echo "╚══════════════════════════════════════════════╝"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# ── Langkah 1: Kompilasi dengan -fPIC ──
echo "[1/5] Kompilasi utilitas.cpp → utilitas.o (dengan -fPIC)"
echo "      -fPIC = Position Independent Code, wajib untuk .so"
g++ -std=c++17 -Wall -O2 -fPIC -c utilitas.cpp -o utilitas.o
echo "      Ukuran: $(du -h utilitas.o | cut -f1)"

# ── Langkah 2: Buat shared library ──
echo ""
echo "[2/5] Buat shared library libutilitas.so"
g++ -shared -o libutilitas.so utilitas.o
echo "      Ukuran: $(du -h libutilitas.so | cut -f1)"

# Tampilkan simbol yang terekspos
echo ""
echo "      Simbol yang diekspor (T = kode, D = data):"
nm -D libutilitas.so | grep " T " | head -12 | sed 's/^/        /'

# ── Langkah 3: Kompilasi program ──
echo ""
echo "[3/5] Kompilasi main_dynamic.cpp → program_dyn"
g++ -std=c++17 -Wall -O2 -o program_dyn main_dynamic.cpp -L. -lutilitas
echo "      Ukuran program: $(du -h program_dyn | cut -f1)"

# ── Langkah 4: Verifikasi dependency ──
echo ""
echo "[4/5] Dependency runtime (ldd):"
ldd program_dyn 2>/dev/null | sed 's/^/      /'
echo ""
echo "      → libutilitas.so TIDAK ada di dalam program!"
echo "        Dibutuhkan saat runtime (LD_LIBRARY_PATH atau /usr/lib)"

# ── Langkah 5: Jalankan ──
echo ""
echo "[5/5] Menjalankan: LD_LIBRARY_PATH=. ./program_dyn"
echo ""
LD_LIBRARY_PATH=. ./program_dyn

echo ""
echo "File yang dihasilkan:"
echo "  utilitas.o       — Object file (perantara)"
echo "  libutilitas.so   — Shared library (diload saat runtime)"
echo "  program_dyn      — Executable (kecil, butuh .so)"
echo ""
echo "Tips deployment:"
echo "  1. Salin libutilitas.so ke /usr/local/lib/"
echo "  2. Jalankan: sudo ldconfig"
echo "  3. Atau gunakan: LD_LIBRARY_PATH=/path/ke/.so ./program_dyn"
