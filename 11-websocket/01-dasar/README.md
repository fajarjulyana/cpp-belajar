# WebSocket — Komunikasi Real-Time Dua Arah

## Apa itu WebSocket?

WebSocket adalah protokol komunikasi **full-duplex** di atas satu koneksi TCP. Berbeda dengan HTTP yang request-response, WebSocket memungkinkan server dan client mengirim data kapan saja.

```
HTTP biasa:
  Client → [Request] → Server
  Client ← [Response] ← Server
  (koneksi ditutup)

WebSocket:
  Client ←→ Server  (koneksi tetap terbuka, keduanya bisa kirim kapan saja)
```

## Proses Handshake

WebSocket dimulai dengan HTTP Upgrade:

```
Client → Server:
  GET /chat HTTP/1.1
  Host: localhost:9001
  Upgrade: websocket
  Connection: Upgrade
  Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
  Sec-WebSocket-Version: 13

Server → Client:
  HTTP/1.1 101 Switching Protocols
  Upgrade: websocket
  Connection: Upgrade
  Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
```

**Rumus Accept Key:**
```
accept = base64( sha1( clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" ) )
```

## Format Frame WebSocket

```
 Bit: 0       4       8      16             32
      ┌───────┬───────┬───────┬─────────────┐
      │FIN RSV│OPCODE │MASK   │PAYLOAD LEN  │
      │1  000 │0001   │0      │xxxxxxx      │
      └───────┴───────┴───────┴─────────────┘
```

| Opcode | Keterangan |
|--------|------------|
| 0x1 | Text frame |
| 0x2 | Binary frame |
| 0x8 | Close |
| 0x9 | Ping |
| 0xA | Pong |

## File

| File | Keterangan |
|------|------------|
| `websocket_demo.cpp` | Simulasi lengkap server + client dalam satu proses |

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -pthread -o ws_demo websocket_demo.cpp
./ws_demo
```

## Penggunaan Nyata

Untuk produksi, gunakan library:

| Library | Bahasa | Keterangan |
|---------|--------|------------|
| **Boost.Beast** | C++ | Bagian dari Boost, high-performance |
| **uWebSockets** | C++ | Sangat cepat, dipakai di banyak server |
| **libwebsockets** | C | Stabil, banyak dipakai embedded |
| **cpp-httplib** | C++ | Header-only, mudah |
