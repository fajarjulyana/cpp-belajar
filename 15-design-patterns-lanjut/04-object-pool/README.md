# Object Pool Pattern

## Masalah: Alokasi Berulang Itu Mahal

```cpp
// Setiap frame: musuh mati → delete, musuh baru → new
// new/delete: ~100-1000 ns per panggilan
// 60 fps × banyak objek = ratusan ms terbuang!

// Contoh buruk:
while (bermain) {
    auto* peluru = new Peluru();  // alokasi heap!
    tembak(peluru);
    // nanti: delete peluru;      // deallokasi!
}
```

## Solusi: Object Pool

```
Pool awal (10 objek):
  [P1][P2][P3][P4][P5][P6][P7][P8][P9][P10]
   aktif                             tidak aktif

Ambil dari pool (acquire):
  [P1*][P2][P3]...                   ← tidak ada new!

Kembalikan ke pool (release):
  [P1][P2][P3]...                    ← tidak ada delete!
```

## Kapan Pakai Object Pool?

- Peluru / partikel yang sering spawn/destroy
- Koneksi database (connection pool)
- Thread pool
- Audio buffer
- Network packet buffer

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -o object_pool object_pool.cpp
./object_pool
```
