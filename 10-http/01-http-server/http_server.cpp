// ============================================================
// File  : http_server.cpp
// Topik : Mini HTTP Server dari Nol — REST API JSON
//
// Fitur:
//   - Multi-klien (thread per koneksi)
//   - Routing: GET / POST / PUT / DELETE
//   - REST API /api/pengguna (CRUD)
//   - Respon JSON & HTML
//   - Thread-safe dengan mutex
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o http_server http_server.cpp
// Jalankan:
//   ./http_server          (port 8080)
//   ./http_server 9000     (port kustom)
//
// Uji coba:
//   curl http://localhost:8080/
//   curl http://localhost:8080/api/pengguna
//   curl -X POST http://localhost:8080/api/pengguna
//        -H "Content-Type: application/json"
//        -d '{"nama":"Dewi","email":"dewi@email.com"}'
// ============================================================

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ============================================================
// Utilitas Log
// ============================================================
std::mutex gLogMutex;

void log(const std::string& level, const std::string& pesan) {
    std::time_t t = std::time(nullptr);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    std::lock_guard<std::mutex> lk(gLogMutex);
    std::cout << "[" << buf << "] [" << level << "] " << pesan << "\n";
    std::cout.flush();
}

// ============================================================
// Model Data — Pengguna
// ============================================================
struct Pengguna {
    int         id;
    std::string nama;
    std::string email;
};

std::mutex              gDataMutex;
std::vector<Pengguna>   gPengguna = {
    {1, "Budi Santoso",  "budi@email.com"},
    {2, "Sari Dewi",     "sari@email.com"},
    {3, "Eko Prasetyo",  "eko@email.com"},
};
std::atomic<int> gNextId{4};

// ============================================================
// JSON helpers (tanpa library)
// ============================================================
std::string escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"')       out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else                out += c;
    }
    return out;
}

std::string penggunaKeJson(const Pengguna& p) {
    return "{\"id\":" + std::to_string(p.id) +
           ",\"nama\":\"" + escape(p.nama) +
           "\",\"email\":\"" + escape(p.email) + "\"}";
}

std::string daftarPenggunaKeJson() {
    std::lock_guard<std::mutex> lk(gDataMutex);
    std::string out = "[";
    for (size_t i = 0; i < gPengguna.size(); ++i) {
        if (i > 0) out += ",";
        out += penggunaKeJson(gPengguna[i]);
    }
    out += "]";
    return out;
}

// Ambil nilai string dari JSON sederhana: {"kunci":"nilai",...}
std::string ambilNilaiJson(const std::string& json, const std::string& kunci) {
    std::string cari = "\"" + kunci + "\"";
    auto pos = json.find(cari);
    if (pos == std::string::npos) return "";
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    ++pos;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    if (pos >= json.size() || json[pos] != '"') return "";
    ++pos;
    std::string hasil;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) { ++pos; }
        hasil += json[pos++];
    }
    return hasil;
}

// ============================================================
// HTTP Request / Response
// ============================================================
struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int         statusCode = 200;
    std::string statusText = "OK";
    std::map<std::string, std::string> headers;
    std::string body;
};

// ============================================================
// Parser HTTP Request
// ============================================================
HttpRequest parseRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream ss(raw);
    std::string baris;

    // Baris pertama: METHOD PATH VERSION
    if (!std::getline(ss, baris)) return req;
    if (!baris.empty() && baris.back() == '\r') baris.pop_back();
    std::istringstream barisStream(baris);
    barisStream >> req.method >> req.path >> req.version;

    // Header
    while (std::getline(ss, baris)) {
        if (!baris.empty() && baris.back() == '\r') baris.pop_back();
        if (baris.empty()) break;  // baris kosong = akhir header
        auto kolon = baris.find(':');
        if (kolon != std::string::npos) {
            std::string k = baris.substr(0, kolon);
            std::string v = baris.substr(kolon + 1);
            while (!v.empty() && v.front() == ' ') v.erase(v.begin());
            // Lowercase key
            std::transform(k.begin(), k.end(), k.begin(), ::tolower);
            req.headers[k] = v;
        }
    }

    // Body (sisa)
    std::string sisaTeks;
    std::string barisSisa;
    while (std::getline(ss, barisSisa)) sisaTeks += barisSisa + "\n";
    req.body = sisaTeks;

    return req;
}

// ============================================================
// Bangun HTTP Response string
// ============================================================
std::string bangunResponse(const HttpResponse& res) {
    std::string out = "HTTP/1.1 " + std::to_string(res.statusCode) +
                      " " + res.statusText + "\r\n";
    for (auto& [k, v] : res.headers) {
        out += k + ": " + v + "\r\n";
    }
    out += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
    out += "Connection: close\r\n";
    out += "\r\n";
    out += res.body;
    return out;
}

HttpResponse buatJson(int kode, const std::string& teks,
                      const std::string& jsonBody) {
    HttpResponse res;
    res.statusCode = kode;
    res.statusText = teks;
    res.headers["Content-Type"] = "application/json; charset=utf-8";
    res.headers["Access-Control-Allow-Origin"] = "*";
    res.body = jsonBody;
    return res;
}

HttpResponse buatError(int kode, const std::string& pesan) {
    return buatJson(kode,
        kode == 404 ? "Not Found" :
        kode == 400 ? "Bad Request" :
        kode == 405 ? "Method Not Allowed" : "Error",
        "{\"error\":\"" + escape(pesan) + "\"}");
}

// ============================================================
// Routing — Handler per endpoint
// ============================================================

// Ambil segmen terakhir path sebagai ID integer
int ambilId(const std::string& path) {
    auto pos = path.rfind('/');
    if (pos == std::string::npos) return -1;
    try { return std::stoi(path.substr(pos + 1)); }
    catch (...) { return -1; }
}

// GET /
HttpResponse handleRoot(const HttpRequest&) {
    HttpResponse res;
    res.headers["Content-Type"] = "text/html; charset=utf-8";
    res.body = R"html(<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>C++ HTTP Server</title>
  <style>
    body { font-family: monospace; max-width: 700px; margin: 40px auto;
           background:#1e1e2e; color:#cdd6f4; padding: 20px; }
    h1   { color:#89b4fa; }
    h2   { color:#a6e3a1; }
    code { background:#313244; padding:2px 6px; border-radius:4px; color:#f38ba8; }
    pre  { background:#313244; padding:12px; border-radius:8px; overflow:auto; }
    .endpoint { color:#fab387; }
  </style>
</head>
<body>
  <h1>🚀 C++ Mini HTTP Server</h1>
  <p>Server berjalan! Dibuat murni dari POSIX socket dan C++17.</p>

  <h2>Endpoint Tersedia</h2>
  <pre>
<span class="endpoint">GET</span>    /api/pengguna          → daftar semua pengguna
<span class="endpoint">GET</span>    /api/pengguna/{id}     → satu pengguna
<span class="endpoint">POST</span>   /api/pengguna          → tambah pengguna
<span class="endpoint">PUT</span>    /api/pengguna/{id}     → update pengguna
<span class="endpoint">DELETE</span> /api/pengguna/{id}     → hapus pengguna
  </pre>

  <h2>Contoh curl</h2>
  <pre>curl http://localhost:8080/api/pengguna
curl -X POST http://localhost:8080/api/pengguna \
     -H "Content-Type: application/json" \
     -d '{"nama":"Dewi","email":"dewi@email.com"}'</pre>
</body>
</html>)html";
    return res;
}

// GET /api/pengguna
HttpResponse handleGetSemua(const HttpRequest&) {
    return buatJson(200, "OK", daftarPenggunaKeJson());
}

// GET /api/pengguna/:id
HttpResponse handleGetSatu(const HttpRequest& req) {
    int id = ambilId(req.path);
    std::lock_guard<std::mutex> lk(gDataMutex);
    for (auto& p : gPengguna) {
        if (p.id == id) return buatJson(200, "OK", penggunaKeJson(p));
    }
    return buatError(404, "Pengguna dengan id " + std::to_string(id) + " tidak ditemukan");
}

// POST /api/pengguna
HttpResponse handleTambah(const HttpRequest& req) {
    std::string nama  = ambilNilaiJson(req.body, "nama");
    std::string email = ambilNilaiJson(req.body, "email");
    if (nama.empty() || email.empty())
        return buatError(400, "Field 'nama' dan 'email' wajib diisi");

    Pengguna baru{gNextId++, nama, email};
    {
        std::lock_guard<std::mutex> lk(gDataMutex);
        gPengguna.push_back(baru);
    }
    log("POST", "Pengguna baru: " + nama);
    HttpResponse res = buatJson(201, "Created", penggunaKeJson(baru));
    return res;
}

// PUT /api/pengguna/:id
HttpResponse handleUpdate(const HttpRequest& req) {
    int id = ambilId(req.path);
    std::string nama  = ambilNilaiJson(req.body, "nama");
    std::string email = ambilNilaiJson(req.body, "email");
    if (nama.empty() || email.empty())
        return buatError(400, "Field 'nama' dan 'email' wajib diisi");

    std::lock_guard<std::mutex> lk(gDataMutex);
    for (auto& p : gPengguna) {
        if (p.id == id) {
            p.nama  = nama;
            p.email = email;
            log("PUT", "Update id=" + std::to_string(id) + " → " + nama);
            return buatJson(200, "OK", penggunaKeJson(p));
        }
    }
    return buatError(404, "Pengguna dengan id " + std::to_string(id) + " tidak ditemukan");
}

// DELETE /api/pengguna/:id
HttpResponse handleHapus(const HttpRequest& req) {
    int id = ambilId(req.path);
    std::lock_guard<std::mutex> lk(gDataMutex);
    auto it = std::remove_if(gPengguna.begin(), gPengguna.end(),
        [id](const Pengguna& p){ return p.id == id; });
    if (it == gPengguna.end())
        return buatError(404, "Pengguna dengan id " + std::to_string(id) + " tidak ditemukan");
    gPengguna.erase(it, gPengguna.end());
    log("DELETE", "Hapus id=" + std::to_string(id));
    return buatJson(200, "OK", "{\"pesan\":\"Pengguna dihapus\",\"id\":" +
                               std::to_string(id) + "}");
}

// ============================================================
// Router utama
// ============================================================
HttpResponse router(const HttpRequest& req) {
    const std::string& m = req.method;
    const std::string& p = req.path;

    // Halaman utama
    if (m == "GET" && p == "/") return handleRoot(req);

    // Daftar semua
    if (m == "GET" && p == "/api/pengguna") return handleGetSemua(req);

    // Satu pengguna — path harus /api/pengguna/<angka>
    if (p.rfind("/api/pengguna/", 0) == 0) {
        if (m == "GET")    return handleGetSatu(req);
        if (m == "PUT")    return handleUpdate(req);
        if (m == "DELETE") return handleHapus(req);
        return buatError(405, "Method tidak didukung untuk endpoint ini");
    }

    // Tambah pengguna
    if (m == "POST" && p == "/api/pengguna") return handleTambah(req);

    return buatError(404, "Endpoint tidak ditemukan: " + m + " " + p);
}

// ============================================================
// Handler per koneksi (dijalankan di thread terpisah)
// ============================================================
void handleKoneksi(int fd, std::string ipKlien) {
    // Baca semua data dari klien
    std::string rawRequest;
    char buf[4096];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        rawRequest.append(buf, n);
        // Hentikan jika sudah dapat header lengkap
        if (rawRequest.find("\r\n\r\n") != std::string::npos) {
            // Cek Content-Length untuk tahu apakah ada body
            auto pos = rawRequest.find("content-length:");
            if (pos == std::string::npos)
                pos = rawRequest.find("Content-Length:");
            if (pos == std::string::npos) break;
            // Sudah cukup (sederhana: anggap body sudah ada)
            break;
        }
    }

    if (rawRequest.empty()) { close(fd); return; }

    HttpRequest  req = parseRequest(rawRequest);
    HttpResponse res = router(req);
    std::string  out = bangunResponse(res);

    send(fd, out.c_str(), out.size(), MSG_NOSIGNAL);

    log("INFO", ipKlien + " " + req.method + " " + req.path +
        " → " + std::to_string(res.statusCode));

    close(fd);
}

// ============================================================
// Main
// ============================================================
int main(int argc, char* argv[]) {
    int port = (argc > 1) ? std::stoi(argv[1]) : 8080;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { std::perror("socket"); return 1; }

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(sfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::perror("bind"); return 1;
    }
    listen(sfd, 20);

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   C++ MINI HTTP SERVER — REST API        ║\n";
    std::cout << "║   http://localhost:" << port;
    std::cout << std::string(23 - std::to_string(port).size(), ' ') << "║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
    log("INFO", "Server berjalan di port " + std::to_string(port));
    log("INFO", "Tekan Ctrl+C untuk berhenti\n");

    while (true) {
        sockaddr_in ca{};
        socklen_t   cl = sizeof(ca);
        int cfd = accept(sfd, (sockaddr*)&ca, &cl);
        if (cfd < 0) break;

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ca.sin_addr, ip, sizeof(ip));

        std::thread(handleKoneksi, cfd, std::string(ip)).detach();
    }

    close(sfd);
    return 0;
}
