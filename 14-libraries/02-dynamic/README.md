# Dynamic/Shared Library (.so) — Membuat dan Menggunakan

## Alur Kerja

```
utilitas.cpp
     │
     │  g++ -fPIC -c utilitas.cpp -o utilitas.o
     ▼
utilitas.o  ←── Object File (Position Independent Code!)
     │
     │  g++ -shared -o libutilitas.so utilitas.o
     ▼
libutilitas.so  ←── Shared Library (dimuat saat runtime)
     │
     │  g++ main_dynamic.cpp -L. -lutilitas -o program
     ▼
program (kecil)  ←── Membutuhkan libutilitas.so saat dijalankan!
```

## File dalam Folder ini

| File | Keterangan |
|------|------------|
| `utilitas.h` | Header — deklarasi fungsi library |
| `utilitas.cpp` | Implementasi fungsi library |
| `main_dynamic.cpp` | Program yang menggunakan library |
| `build_dynamic.sh` | Script build lengkap |

## Cara Build Manual

```bash
# 1. Kompilasi dengan -fPIC (Position Independent Code — WAJIB untuk .so)
g++ -std=c++17 -fPIC -O2 -c utilitas.cpp -o utilitas.o

# 2. Buat shared library
g++ -shared -o libutilitas.so utilitas.o

# 3. Inspeksi library
nm -D libutilitas.so          # simbol yang diekspor
ldd libutilitas.so            # dependency

# 4. Kompilasi program
g++ -std=c++17 -o program_dyn main_dynamic.cpp -L. -lutilitas

# 5. Jalankan (perlu tahu lokasi .so)
LD_LIBRARY_PATH=. ./program_dyn
```

## Cara Build Otomatis

```bash
chmod +x build_dynamic.sh
./build_dynamic.sh
```

## Menempatkan .so di Tempat Standar

```bash
# Salin ke direktori sistem
sudo cp libutilitas.so /usr/local/lib/

# Update cache linker
sudo ldconfig

# Sekarang bisa jalan tanpa LD_LIBRARY_PATH
./program_dyn
```

## -fPIC — Kenapa Wajib?

**Position Independent Code** = kode yang bisa diload ke alamat memori mana saja.

Tanpa `-fPIC`, kode asumsikan alamat memory tetap → tidak bisa di-share antar proses.

Dengan `-fPIC`, instruksi menggunakan **relative addressing** → bisa dimuat di alamat mana saja → satu salinan di RAM dipakai semua proses.

## Library Utilitas yang Dibuat

- String: `hurufBesar`, `hurufKecil`, `balik`, `palindrom`, `hitungKata`, `pisah`, `gabung`, `formatRibu`
- Waktu: `waktuSekarang`, `tanggalSekarang`, `timestampSekarang`
- Validasi: `validEmail`, `validAngka`, `validURL`
- Hash: `hash32` (FNV-1a)

## Perbandingan Static vs Dynamic

```bash
# Ukuran yang khas:
ls -lh program_static program_dyn libutilitas.so libmatematika.a

# Static: program lebih besar, tidak butuh .a saat run
# Dynamic: program lebih kecil, butuh .so saat run
```
