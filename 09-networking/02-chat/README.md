# Chat Client-Server Dua Arah — C++ POSIX Sockets

## Arsitektur

```
┌──────────────────────────────────────────────────────────┐
│                      CHAT SERVER                         │
│                                                          │
│  ┌──────────┐   broadcast()   ┌──────────┐              │
│  │ Client A │ ←───────────────│ Client B │              │
│  │ thread   │ ────────────────→ thread   │              │
│  └──────────┘                 └──────────┘              │
│        ↕                            ↕                   │
│  ┌──────────────────────────────────────┐               │
│  │      daftar klien (mutex-protected)  │               │
│  └──────────────────────────────────────┘               │
└──────────────────────────────────────────────────────────┘
         ↕                       ↕
   [Terminal A]            [Terminal B]
   > /nama Budi            > /nama Sari
   > Halo Sari!            > Halo Budi!
```

## Fitur

- Multi-klien: server menangani banyak klien sekaligus
- Broadcast: pesan dari satu klien dikirim ke **semua** klien lain
- Nama pengguna: perintah `/nama <nama>` untuk mengatur nama
- Daftar user: perintah `/daftar` untuk melihat siapa yang online
- Pesan pribadi: `/pm <nama> <pesan>` untuk pesan ke satu orang
- Keluar: `/keluar` untuk disconnect dengan rapi

## File

| File | Keterangan |
|------|------------|
| `chat_server.cpp` | Server multi-klien (jalankan dulu) |
| `chat_client.cpp` | Client interaktif (buka di terminal lain) |
| `chat_demo.cpp` | Simulasi otomatis dalam satu proses (untuk uji coba) |

## Cara Kompilasi

```bash
# Server
g++ -std=c++17 -pthread -o chat_server chat_server.cpp

# Client
g++ -std=c++17 -pthread -o chat_client chat_client.cpp

# Simulasi satu-proses (tanpa perlu dua terminal)
g++ -std=c++17 -pthread -o chat_demo chat_demo.cpp
```

## Cara Menjalankan (dua terminal)

**Terminal 1 — Server:**
```bash
./chat_server 7777
# Server mendengarkan di port 7777 ...
```

**Terminal 2 — Client A:**
```bash
./chat_client 127.0.0.1 7777
> /nama Budi
> Halo semua!
> /daftar
```

**Terminal 3 — Client B:**
```bash
./chat_client 127.0.0.1 7777
> /nama Sari
> Halo Budi!
> /pm Budi Ini pesan rahasia ya
```

## Perintah Chat

| Perintah | Contoh | Keterangan |
|----------|--------|------------|
| `/nama <nama>` | `/nama Budi` | Atur nama tampilan |
| `/daftar` | `/daftar` | Tampilkan semua user online |
| `/pm <nama> <pesan>` | `/pm Sari hai!` | Pesan pribadi |
| `/keluar` | `/keluar` | Keluar dari chat |
| *(teks biasa)* | `Halo semua!` | Broadcast ke semua |

## Simulasi Satu Proses

Jalankan `chat_demo` untuk melihat percakapan otomatis tanpa perlu buka banyak terminal:

```bash
./chat_demo
```

Output akan menunjukkan server + 3 klien saling berkirim pesan secara real-time.

## Konsep yang Dipelajari

- `accept()` dalam loop — menerima banyak koneksi
- `std::thread` per klien — konkurensi
- `std::mutex` + `std::lock_guard` — proteksi data bersama
- `send()` / `recv()` — transfer data TCP
- Protokol teks sederhana dengan perintah
- Graceful shutdown dengan atomic flag
