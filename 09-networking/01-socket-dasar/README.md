# Socket Dasar — Networking C++

## Apa itu Socket?

Socket adalah titik akhir komunikasi dua arah antara dua program melalui jaringan (atau lokal). Di Linux/POSIX, socket diperlakukan seperti file descriptor biasa.

```
Client                          Server
  |                               |
  |--- connect() ---------------→ |
  |                               | ← accept()
  |--- send("Halo") -----------→ |
  |                    recv() →   |
  |← send("Balasan") ----------- |
  |    ← recv()                   |
  |--- close() -----------------→ |
```

## Jenis Socket

| Jenis | Protokol | Karakteristik |
|-------|----------|---------------|
| `SOCK_STREAM` | TCP | Andal, berurutan, koneksi |
| `SOCK_DGRAM` | UDP | Cepat, tidak terjamin, tanpa koneksi |
| `SOCK_RAW` | IP mentah | Level rendah, perlu root |

## Alur Pemrograman TCP

### Sisi Server
```
socket()   → buat socket
bind()     → ikat ke port
listen()   → mulai mendengarkan
accept()   → terima koneksi masuk (blokir)
recv/send  → komunikasi data
close()    → tutup koneksi
```

### Sisi Client
```
socket()   → buat socket
connect()  → hubungkan ke server
send/recv  → komunikasi data
close()    → tutup koneksi
```

## Contoh TCP Echo (lihat tcp_echo.cpp)

File `tcp_echo.cpp` berisi simulasi lengkap client-server dalam satu file menggunakan thread.

### Kompilasi & Jalankan
```bash
g++ -std=c++17 -pthread -o tcp_echo tcp_echo.cpp
./tcp_echo
```

## Konsep Penting

### Address Structure
```cpp
struct sockaddr_in {
    sa_family_t    sin_family;   // AF_INET
    in_port_t      sin_port;     // Port (network byte order)
    struct in_addr sin_addr;     // Alamat IP
};
```

### Byte Order
Jaringan menggunakan **big-endian** (network byte order).  
CPU x86 menggunakan **little-endian**.  
Gunakan fungsi konversi:

```cpp
htons(port)   // host → network (short/16-bit)
htonl(addr)   // host → network (long/32-bit)
ntohs(port)   // network → host (short)
ntohl(addr)   // network → host (long)
```

### Non-blocking Socket
```cpp
int flags = fcntl(sock, F_GETFL, 0);
fcntl(sock, F_SETFL, flags | O_NONBLOCK);
```

### select() — Multiplexing
Memungkinkan satu thread memantau banyak socket sekaligus:
```cpp
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(sock1, &readfds);
FD_SET(sock2, &readfds);

timeval tv{5, 0};  // timeout 5 detik
int siap = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);

if (FD_ISSET(sock1, &readfds)) { /* sock1 siap dibaca */ }
```

## Referensi

- `man 7 socket` — dokumentasi socket POSIX
- `man 7 tcp` — detail protokol TCP
- RFC 793 — spesifikasi TCP
- Beej's Guide to Network Programming: https://beej.us/guide/bgnet/
