# Mini HTTP Server dari Nol — C++ POSIX

## Apa itu HTTP?

HTTP (HyperText Transfer Protocol) adalah protokol teks di atas TCP. Browser mengirim **Request**, server membalas dengan **Response**.

```
Client (Browser)                    Server
     |                                 |
     |-- GET /api/pengguna HTTP/1.1 →  |
     |   Host: localhost:8080          |
     |   Accept: application/json      |
     |                                 |
     |← HTTP/1.1 200 OK -------------- |
     |   Content-Type: application/json|
     |   Content-Length: 42           |
     |                                 |
     |   {"id":1,"nama":"Budi"}        |
```

## Format Request HTTP

```
METHOD /path HTTP/1.1\r\n
Header-Name: Header-Value\r\n
\r\n
[body opsional]
```

**Method umum:**

| Method | Kegunaan |
|--------|----------|
| GET | Ambil data |
| POST | Kirim data baru |
| PUT | Update data (ganti seluruhnya) |
| PATCH | Update sebagian |
| DELETE | Hapus data |

## Format Response HTTP

```
HTTP/1.1 STATUS_CODE STATUS_TEXT\r\n
Header-Name: Header-Value\r\n
\r\n
[body]
```

**Status Code umum:**

| Kode | Arti |
|------|------|
| 200 | OK |
| 201 | Created |
| 204 | No Content |
| 400 | Bad Request |
| 404 | Not Found |
| 405 | Method Not Allowed |
| 500 | Internal Server Error |

## Arsitektur Server (http_server.cpp)

```
main()
  └── socket() → bind() → listen()
      └── loop: accept() → thread per koneksi
                  └── parseRequest()
                      └── router(method, path, body)
                          ├── GET  /              → halaman utama HTML
                          ├── GET  /api/pengguna  → list semua user (JSON)
                          ├── GET  /api/pengguna/:id → satu user
                          ├── POST /api/pengguna  → tambah user
                          ├── PUT  /api/pengguna/:id → update user
                          └── DELETE /api/pengguna/:id → hapus user
```

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -pthread -o http_server http_server.cpp
./http_server          # default port 8080
./http_server 9000     # port kustom
```

## Uji coba dengan curl

```bash
# Halaman utama
curl http://localhost:8080/

# Lihat semua pengguna
curl http://localhost:8080/api/pengguna

# Lihat satu pengguna
curl http://localhost:8080/api/pengguna/1

# Tambah pengguna
curl -X POST http://localhost:8080/api/pengguna \
     -H "Content-Type: application/json" \
     -d '{"nama":"Dewi","email":"dewi@email.com"}'

# Update pengguna
curl -X PUT http://localhost:8080/api/pengguna/1 \
     -H "Content-Type: application/json" \
     -d '{"nama":"Budi Update","email":"budi2@email.com"}'

# Hapus pengguna
curl -X DELETE http://localhost:8080/api/pengguna/1
```

## Konsep Penting

### Parsing Request
HTTP request dipisah baris per baris (`\r\n`). Baris kosong memisahkan header dari body.

### Routing
Cocokkan `method + path` lalu panggil fungsi handler yang sesuai.

### Content-Type
- `text/html` — halaman web
- `application/json` — data API
- `text/plain` — teks biasa

### Keep-Alive vs Close
HTTP/1.1 default keep-alive, tapi server sederhana ini pakai `Connection: close` untuk kemudahan.

### Thread Safety
Data pengguna dilindungi `std::mutex` agar aman diakses banyak thread sekaligus.
