# Static Library (.a) — Membuat dan Menggunakan

## Alur Kerja

```
matematika.cpp
     │
     │  g++ -c matematika.cpp -o matematika.o
     ▼
matematika.o   ←── Object File (kode terkompilasi)
     │
     │  ar rcs libmatematika.a matematika.o
     ▼
libmatematika.a  ←── Static Library (arsip object files)
     │
     │  g++ main_static.cpp -L. -lmatematika -o program
     ▼
program  ←── Executable (semua kode sudah di dalam!)
```

## File dalam Folder ini

| File | Keterangan |
|------|------------|
| `matematika.h` | Header — deklarasi fungsi library |
| `matematika.cpp` | Implementasi fungsi library |
| `main_static.cpp` | Program yang menggunakan library |
| `build_static.sh` | Script build lengkap |

## Cara Build Manual

```bash
# 1. Kompilasi ke object file
g++ -std=c++17 -O2 -c matematika.cpp -o matematika.o

# 2. Buat static library (ar = archiver)
ar rcs libmatematika.a matematika.o

# 3. Inspeksi isi library
ar -t libmatematika.a          # daftar object files
nm libmatematika.a             # daftar simbol

# 4. Link program dengan library
g++ -std=c++17 -o program main_static.cpp -L. -lmatematika

# 5. Jalankan (tidak butuh file .a!)
./program
```

## Cara Build Otomatis

```bash
chmod +x build_static.sh
./build_static.sh
```

## Flags Penting

| Flag | Fungsi |
|------|--------|
| `-c` | Kompilasi saja, jangan link |
| `ar rcs` | r=replace, c=create, s=index |
| `-L.` | Tambah direktori `.` ke library search path |
| `-lmatematika` | Link dengan `libmatematika.a` atau `.so` |
| `-I.` | Tambah direktori `.` ke include search path |

## Library Matematika yang Dibuat

- Operasi dasar: `tambah`, `kurang`, `kali`, `bagi`
- Statistik: `rata2`, `median`, `standarDeviasi`, `min`, `max`
- Bilangan: `adalahPrima`, `faktorial`, `fibonacci`, `gcd`, `lcm`
- Konversi: `derajatKeRadian`, `radianKeDerjat`
