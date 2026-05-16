# HTTP Client dari Nol — C++ POSIX

## Cara Kerja HTTP Client

Client membuka koneksi TCP ke server, mengirim teks HTTP request, lalu membaca response.

```
1. socket()   → buat socket TCP
2. connect()  → hubungkan ke host:port
3. send()     → kirim teks HTTP request
4. recv()     → baca response (header + body)
5. close()    → tutup koneksi
```

## File

| File | Keterangan |
|------|------------|
| `http_client.cpp` | HTTP client lengkap: GET, POST, PUT, DELETE |
| Demo internal | Memanggil server nyata (httpbin.org) atau server lokal |

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -pthread -o http_client http_client.cpp
./http_client
```

## Fitur http_client.cpp

- **GET** — ambil data, parsing header & body
- **POST** — kirim JSON body
- **PUT / DELETE** — update & hapus resource
- **URL parsing** — pisah host, port, path dari URL string
- **Response parsing** — status code, headers, body terpisah
- **Redirect** — deteksi 3xx (dicatat, tidak diikuti otomatis)

## Contoh Penggunaan (API)

Program demo memanggil `httpbin.org` (layanan test HTTP publik):

```
GET  https://httpbin.org/get        → tampilkan info request
POST https://httpbin.org/post       → kirim JSON, lihat echo
GET  https://httpbin.org/status/404 → contoh error handling
GET  https://httpbin.org/delay/1    → timeout handling
```

> **Catatan:** `http_client.cpp` hanya mendukung HTTP (bukan HTTPS).
> Untuk HTTPS diperlukan library seperti OpenSSL atau libcurl.

## Konsep Penting

### URL Parsing
```
http://api.contoh.com:8080/pengguna?filter=aktif
      ───────────────  ────  ───────────────────
           host        port          path
```

### HTTP Request Manual
```
GET /api/data HTTP/1.1\r\n
Host: api.contoh.com\r\n
Accept: application/json\r\n
Connection: close\r\n
\r\n
```

### Membaca Response yang Panjang
Response bisa datang dalam beberapa paket TCP. Harus terus `recv()` sampai koneksi ditutup server atau `Content-Length` terpenuhi.

### Status Code Handling
```cpp
if (response.statusCode == 200) { /* sukses */ }
else if (response.statusCode == 404) { /* tidak ditemukan */ }
else if (response.statusCode >= 500) { /* error server */ }
```

## Alternatif Library (Produksi)

Untuk aplikasi nyata, gunakan library yang sudah teruji:

| Library | Kelebihan |
|---------|-----------|
| **libcurl** | Paling populer, HTTPS, semua platform |
| **cpp-httplib** | Header-only, mudah dipakai |
| **Boost.Beast** | Asynchronous, performa tinggi |
| **Poco::HTTP** | Lengkap, cross-platform |
