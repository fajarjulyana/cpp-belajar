// ============================================================
// File  : http_client.cpp
// Topik : HTTP Client dari Nol — GET, POST, PUT, DELETE
//
// Tidak menggunakan library eksternal — murni POSIX socket.
// Hanya mendukung HTTP (bukan HTTPS).
// Untuk HTTPS gunakan libcurl atau cpp-httplib.
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o http_client http_client.cpp
// Jalankan:
//   ./http_client
// ============================================================

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <iomanip>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

// ============================================================
// Struktur URL yang sudah diurai
// ============================================================
struct Url {
    std::string skema;   // "http"
    std::string host;    // "api.contoh.com"
    int         port;    // 80
    std::string path;    // "/pengguna?filter=aktif"
};

// ============================================================
// Struktur HTTP Response
// ============================================================
struct HttpResponse {
    int         statusCode = 0;
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;

    bool ok() const { return statusCode >= 200 && statusCode < 300; }
};

// ============================================================
// Parsing URL
// ============================================================
Url parseUrl(const std::string& url) {
    Url hasil;
    std::string sisa = url;

    // Skema
    auto pos = sisa.find("://");
    if (pos != std::string::npos) {
        hasil.skema = sisa.substr(0, pos);
        sisa = sisa.substr(pos + 3);
    } else {
        hasil.skema = "http";
    }

    // Host + port + path
    auto pathPos = sisa.find('/');
    std::string hostPort = (pathPos != std::string::npos)
                           ? sisa.substr(0, pathPos)
                           : sisa;
    hasil.path = (pathPos != std::string::npos)
                 ? sisa.substr(pathPos)
                 : "/";

    auto kolon = hostPort.find(':');
    if (kolon != std::string::npos) {
        hasil.host = hostPort.substr(0, kolon);
        hasil.port = std::stoi(hostPort.substr(kolon + 1));
    } else {
        hasil.host = hostPort;
        hasil.port = 80;
    }

    return hasil;
}

// ============================================================
// Resolve host → IP dan buat koneksi TCP
// ============================================================
int buatKoneksi(const std::string& host, int port) {
    addrinfo hints{}, *res;
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string portStr = std::to_string(port);
    int rc = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res);
    if (rc != 0) {
        throw std::runtime_error("getaddrinfo gagal: " +
                                 std::string(gai_strerror(rc)));
    }

    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0) { freeaddrinfo(res); throw std::runtime_error("socket gagal"); }

    if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
        close(fd);
        freeaddrinfo(res);
        throw std::runtime_error("connect gagal ke " + host + ":" + portStr);
    }

    freeaddrinfo(res);
    return fd;
}

// ============================================================
// Parsing HTTP Response
// ============================================================
HttpResponse parseResponse(const std::string& raw) {
    HttpResponse res;
    std::istringstream ss(raw);
    std::string baris;

    // Baris pertama: HTTP/1.1 200 OK
    if (!std::getline(ss, baris)) return res;
    if (!baris.empty() && baris.back() == '\r') baris.pop_back();
    {
        std::istringstream b(baris);
        std::string versi;
        b >> versi >> res.statusCode;
        std::getline(b, res.statusText);
        if (!res.statusText.empty() && res.statusText.front() == ' ')
            res.statusText.erase(res.statusText.begin());
    }

    // Headers
    while (std::getline(ss, baris)) {
        if (!baris.empty() && baris.back() == '\r') baris.pop_back();
        if (baris.empty()) break;
        auto kolon = baris.find(':');
        if (kolon != std::string::npos) {
            std::string k = baris.substr(0, kolon);
            std::string v = baris.substr(kolon + 1);
            while (!v.empty() && v.front() == ' ') v.erase(v.begin());
            // Lowercase key untuk kemudahan lookup
            for (char& c : k) c = (char)std::tolower((unsigned char)c);
            res.headers[k] = v;
        }
    }

    // Body — sisa stream
    std::string bodyBaris;
    while (std::getline(ss, bodyBaris)) res.body += bodyBaris + "\n";
    // Hapus newline trailing berlebih
    while (!res.body.empty() && res.body.back() == '\n')
        res.body.pop_back();

    return res;
}

// ============================================================
// Kirim HTTP Request
// ============================================================
HttpResponse kirimRequest(const std::string& method,
                          const std::string& url,
                          const std::string& body = "",
                          const std::string& contentType = "") {
    Url u = parseUrl(url);
    int fd = buatKoneksi(u.host, u.port);

    // Bangun request string
    std::string req = method + " " + u.path + " HTTP/1.1\r\n";
    req += "Host: " + u.host + "\r\n";
    req += "Accept: */*\r\n";
    req += "User-Agent: CppHttpClient/1.0\r\n";
    if (!body.empty()) {
        req += "Content-Type: " + contentType + "\r\n";
        req += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    }
    req += "Connection: close\r\n";
    req += "\r\n";
    req += body;

    send(fd, req.c_str(), req.size(), 0);

    // Baca semua response
    std::string rawResponse;
    char buf[4096];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        rawResponse.append(buf, n);
    }
    close(fd);

    return parseResponse(rawResponse);
}

// Shortcut helpers
HttpResponse httpGet(const std::string& url) {
    return kirimRequest("GET", url);
}
HttpResponse httpPost(const std::string& url, const std::string& jsonBody) {
    return kirimRequest("POST", url, jsonBody, "application/json");
}
HttpResponse httpPut(const std::string& url, const std::string& jsonBody) {
    return kirimRequest("PUT", url, jsonBody, "application/json");
}
HttpResponse httpDelete(const std::string& url) {
    return kirimRequest("DELETE", url);
}

// ============================================================
// Tampilkan response dengan rapi
// ============================================================
void tampilkanResponse(const std::string& label, const HttpResponse& res) {
    std::string warna = res.ok() ? "\033[32m" : "\033[31m";
    std::cout << "\n\033[1m── " << label << " ──\033[0m\n";
    std::cout << "Status : " << warna << res.statusCode
              << " " << res.statusText << "\033[0m\n";

    if (res.headers.count("content-type"))
        std::cout << "Tipe   : " << res.headers.at("content-type") << "\n";
    if (res.headers.count("content-length"))
        std::cout << "Ukuran : " << res.headers.at("content-length") << " byte\n";

    // Tampilkan body (potong jika terlalu panjang)
    std::string tampilBody = res.body;
    if (tampilBody.size() > 500) tampilBody = tampilBody.substr(0, 497) + "...";
    std::cout << "Body   :\n" << tampilBody << "\n";
}

// ============================================================
// Demo 1 — Memanggil server lokal (http_server.cpp)
// ============================================================
void demoServerLokal(int port) {
    std::string base = "http://localhost:" + std::to_string(port);

    std::cout << "\n\033[1;34m══════════════════════════════════════════\033[0m\n";
    std::cout << "\033[1;34m  DEMO: Memanggil Server Lokal (port "
              << port << ")\033[0m\n";
    std::cout << "\033[1;34m══════════════════════════════════════════\033[0m\n";

    try {
        // GET semua pengguna
        tampilkanResponse("GET /api/pengguna",
                          httpGet(base + "/api/pengguna"));

        // GET satu pengguna
        tampilkanResponse("GET /api/pengguna/1",
                          httpGet(base + "/api/pengguna/1"));

        // POST — tambah pengguna baru
        tampilkanResponse("POST /api/pengguna",
                          httpPost(base + "/api/pengguna",
                                   R"({"nama":"Dewi Lestari","email":"dewi@email.com"})"));

        // GET ulang untuk konfirmasi
        tampilkanResponse("GET /api/pengguna (setelah POST)",
                          httpGet(base + "/api/pengguna"));

        // PUT — update pengguna id=2
        tampilkanResponse("PUT /api/pengguna/2",
                          httpPut(base + "/api/pengguna/2",
                                  R"({"nama":"Sari Dewi Update","email":"sari.baru@email.com"})"));

        // DELETE — hapus pengguna id=3
        tampilkanResponse("DELETE /api/pengguna/3",
                          httpDelete(base + "/api/pengguna/3"));

        // GET akhir
        tampilkanResponse("GET /api/pengguna (akhir)",
                          httpGet(base + "/api/pengguna"));

        // 404
        tampilkanResponse("GET /api/pengguna/999 (tidak ada)",
                          httpGet(base + "/api/pengguna/999"));

    } catch (const std::exception& e) {
        std::cout << "\033[31m[!] Error: " << e.what() << "\033[0m\n";
        std::cout << "    Pastikan http_server sudah berjalan di port " << port << "!\n";
    }
}

// ============================================================
// Demo 2 — Memanggil httpbin.org (layanan test HTTP publik)
// ============================================================
void demoHttpbin() {
    std::cout << "\n\033[1;33m══════════════════════════════════════════\033[0m\n";
    std::cout << "\033[1;33m  DEMO: Memanggil httpbin.org (Internet)\033[0m\n";
    std::cout << "\033[1;33m══════════════════════════════════════════\033[0m\n";

    try {
        // GET sederhana
        auto resGet = httpGet("http://httpbin.org/get");
        tampilkanResponse("GET httpbin.org/get", resGet);

        // POST dengan JSON
        auto resPost = httpPost("http://httpbin.org/post",
                                R"({"pesan":"Halo dari C++!","versi":"17"})");
        tampilkanResponse("POST httpbin.org/post", resPost);

        // Status 404
        auto res404 = httpGet("http://httpbin.org/status/404");
        tampilkanResponse("GET httpbin.org/status/404 (error handling)", res404);

    } catch (const std::exception& e) {
        std::cout << "\033[31m[!] Tidak bisa terhubung ke httpbin.org: "
                  << e.what() << "\033[0m\n";
        std::cout << "    Demo ini butuh koneksi internet.\n";
    }
}

// ============================================================
// Demo 3 — Konsep: tunjukkan raw HTTP request yang dikirim
// ============================================================
void demoRawRequest() {
    std::cout << "\n\033[1;35m══════════════════════════════════════════\033[0m\n";
    std::cout << "\033[1;35m  KONSEP: Format Raw HTTP Request\033[0m\n";
    std::cout << "\033[1;35m══════════════════════════════════════════\033[0m\n";

    std::cout << "\nGET Request:\n";
    std::cout << "\033[36m";
    std::cout << "GET /api/pengguna HTTP/1.1\\r\\n\n";
    std::cout << "Host: localhost:8080\\r\\n\n";
    std::cout << "Accept: */*\\r\\n\n";
    std::cout << "Connection: close\\r\\n\n";
    std::cout << "\\r\\n\n";
    std::cout << "\033[0m";

    std::cout << "\nPOST Request:\n";
    std::cout << "\033[36m";
    std::cout << "POST /api/pengguna HTTP/1.1\\r\\n\n";
    std::cout << "Host: localhost:8080\\r\\n\n";
    std::cout << "Content-Type: application/json\\r\\n\n";
    std::cout << "Content-Length: 42\\r\\n\n";
    std::cout << "Connection: close\\r\\n\n";
    std::cout << "\\r\\n\n";
    std::cout << "{\"nama\":\"Dewi\",\"email\":\"dewi@email.com\"}\n";
    std::cout << "\033[0m";

    std::cout << "\nHTTP Response:\n";
    std::cout << "\033[36m";
    std::cout << "HTTP/1.1 201 Created\\r\\n\n";
    std::cout << "Content-Type: application/json\\r\\n\n";
    std::cout << "Content-Length: 45\\r\\n\n";
    std::cout << "Connection: close\\r\\n\n";
    std::cout << "\\r\\n\n";
    std::cout << "{\"id\":4,\"nama\":\"Dewi\",\"email\":\"dewi@email.com\"}\n";
    std::cout << "\033[0m";
}

// ============================================================
// Main
// ============================================================
int main(int argc, char* argv[]) {
    std::cout << "\n\033[1m";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   C++ HTTP CLIENT — dari Nol             ║\n";
    std::cout << "║   GET / POST / PUT / DELETE              ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
    std::cout << "\033[0m";

    // Tampilkan konsep raw request dulu
    demoRawRequest();

    // Demo server lokal (jika ada argumen port, atau default 8080)
    int port = (argc > 1) ? std::stoi(argv[1]) : 8080;
    demoServerLokal(port);

    // Demo httpbin.org (opsional, butuh internet)
    std::cout << "\n";
    std::cout << "Coba memanggil httpbin.org (butuh internet)...\n";
    demoHttpbin();

    std::cout << "\n\033[1m[SELESAI]\033[0m\n\n";
    return 0;
}
