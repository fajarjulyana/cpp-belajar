# Proyek Mahir — C++

Proyek-proyek yang lebih kompleks untuk mengasah kemampuan C++ tingkat lanjut: OOP penuh, multithreading, jaringan, dan integrasi sistem.

---

## Daftar Proyek

| No | Proyek | File | Konsep |
|----|--------|------|--------|
| 01 | Bank Account System | `bank.cpp` | OOP, inheritance, exception, file I/O |
| 02 | HTTP Client Sederhana | `http_client.cpp` | Socket, protocol, string parsing |
| 03 | Thread Pool & Task Queue | `thread_pool.cpp` | Multithreading, mutex, condition_variable |
| 04 | Mini Database (JSON) | `mini_db.cpp` | File I/O, indexing, query sederhana |
| 05 | Plugin Audio JUCE | Lihat folder `07-audio-juce/` | DSP, GUI, real-time audio |

---

## Proyek 01: Sistem Bank

Simulasi sistem perbankan dengan berbagai jenis akun, transaksi, dan laporan.

**Fitur:**
- Akun Tabungan, Giro, Deposito (inheritance)
- Transaksi: setor, tarik, transfer
- Log transaksi dengan timestamp
- Bunga otomatis
- Laporan rekening koran

Lihat: [bank.cpp](bank.cpp)

---

## Proyek 02: Thread Pool

Implementasi thread pool untuk memproses task secara paralel.

**Fitur:**
- Pool N thread worker
- Task queue thread-safe
- Future/promise untuk hasil async
- Graceful shutdown
- Statistik performa

Lihat: [thread_pool.cpp](thread_pool.cpp)

---

## Proyek 03: Mini Database

Database key-value sederhana dengan persistensi file dan query dasar.

**Fitur:**
- CRUD operasi (Create, Read, Update, Delete)
- Tipe data: string, integer, float, boolean
- Index untuk pencarian cepat
- Transaksi (ACID sederhana)
- Export ke CSV/JSON

Lihat: [mini_db.cpp](mini_db.cpp)

---

**[← Proyek Sederhana](../01-sederhana/README.md)** | **[← Kembali ke Daftar Isi](../../README.md)**
