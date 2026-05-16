# Static & Dynamic Libraries — C++

## Perbedaan Utama

```
┌─────────────────────────────────────────────────────────────┐
│  STATIC LIBRARY (.a / .lib)                                 │
│                                                             │
│  program.cpp  ──┐                                           │
│  libmatematika.a─┼──→ [LINKER] ──→ program (satu file)     │
│                  ┘                  ↑                       │
│                              semua kode disatukan           │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  DYNAMIC / SHARED LIBRARY (.so / .dll / .dylib)             │
│                                                             │
│  program.cpp  ──→ [LINKER] ──→ program (kecil)             │
│                                    │  ← diload saat runtime │
│                               libutilitas.so                │
└─────────────────────────────────────────────────────────────┘
```

## Perbandingan

| Aspek | Static (.a) | Dynamic (.so/.dll) |
|-------|-------------|---------------------|
| Ukuran executable | Besar (kode library disertakan) | Kecil |
| Kecepatan startup | Lebih cepat | Sedikit lebih lambat |
| Update library | Harus kompilasi ulang | Cukup update .so |
| Distribusi | Satu file, mudah | Perlu sertakan .so |
| Berbagi memori | Tidak (tiap proses punya salinan) | Ya (satu salinan di RAM) |
| Penggunaan | Tools CLI, embedded | Library sistem, plugin |

## Subfolder

| Folder | Isi |
|--------|-----|
| `01-static/` | Membuat & menggunakan static library |
| `02-dynamic/` | Membuat & menggunakan shared library |

## Alat yang Digunakan

| Alat | Fungsi |
|------|--------|
| `g++ -c` | Kompilasi ke object file (.o) |
| `ar rcs` | Buat static library dari object files |
| `g++ -shared -fPIC` | Buat shared library (.so) |
| `ldd` | Lihat dependency .so suatu program |
| `nm` | Lihat simbol dalam library/object |
| `objdump -d` | Disassembly |
| `LD_LIBRARY_PATH` | Path pencarian .so saat runtime |
| `pkg-config` | Ambil flag kompilasi library terinstal |

## Perintah Berguna

```bash
# Lihat library yang dibutuhkan program
ldd ./program

# Lihat simbol dalam library
nm -D libutilitas.so
nm libmatematika.a

# Info detail library
objdump -p libutilitas.so | grep NEEDED

# pkg-config (untuk library populer)
pkg-config --cflags --libs sfml-graphics
pkg-config --cflags --libs libcurl
```
