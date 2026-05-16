// ============================================================
// File  : websocket_demo.cpp
// Topik : WebSocket dari Nol — SHA1 + Base64 + Frame Parsing
//
// Implementasi penuh WebSocket RFC 6455 tanpa library eksternal.
// Server + Client berjalan dalam satu proses (untuk demo/belajar).
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o ws_demo websocket_demo.cpp
// Jalankan:
//   ./ws_demo
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <algorithm>
#include <functional>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ============================================================
// SHA-1 — diperlukan untuk WebSocket handshake key
// ============================================================
namespace SHA1 {

struct Context {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
};

static uint32_t rol(uint32_t v, uint32_t n) {
    return (v << n) | (v >> (32 - n));
}

static void transform(Context& ctx, const uint8_t blok[64]) {
    uint32_t a, b, c, d, e, f, k, tmp;
    uint32_t w[80];

    for (int i = 0; i < 16; ++i)
        w[i] = (blok[i*4]<<24)|(blok[i*4+1]<<16)|(blok[i*4+2]<<8)|blok[i*4+3];
    for (int i = 16; i < 80; ++i)
        w[i] = rol(w[i-3]^w[i-8]^w[i-14]^w[i-16], 1);

    a = ctx.state[0]; b = ctx.state[1];
    c = ctx.state[2]; d = ctx.state[3]; e = ctx.state[4];

    for (int i = 0; i < 80; ++i) {
        if      (i < 20) { f = (b&c)|((~b)&d); k = 0x5A827999; }
        else if (i < 40) { f = b^c^d;           k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b&c)|(b&d)|(c&d); k = 0x8F1BBCDC; }
        else             { f = b^c^d;           k = 0xCA62C1D6; }

        tmp = rol(a,5) + f + e + k + w[i];
        e = d; d = c; c = rol(b,30); b = a; a = tmp;
    }

    ctx.state[0] += a; ctx.state[1] += b;
    ctx.state[2] += c; ctx.state[3] += d; ctx.state[4] += e;
}

static void init(Context& ctx) {
    ctx.state[0] = 0x67452301; ctx.state[1] = 0xEFCDAB89;
    ctx.state[2] = 0x98BADCFE; ctx.state[3] = 0x10325476;
    ctx.state[4] = 0xC3D2E1F0;
    ctx.count[0] = ctx.count[1] = 0;
}

static void update(Context& ctx, const uint8_t* data, size_t len) {
    size_t i, j = (ctx.count[0] >> 3) & 63;
    if ((ctx.count[0] += (uint32_t)(len << 3)) < (uint32_t)(len << 3))
        ++ctx.count[1];
    ctx.count[1] += (uint32_t)(len >> 29);
    if ((j + len) > 63) {
        i = 64 - j;
        memcpy(&ctx.buffer[j], data, i);
        transform(ctx, ctx.buffer);
        for (; i + 63 < len; i += 64) transform(ctx, data + i);
        j = 0;
    } else {
        i = 0;
    }
    memcpy(&ctx.buffer[j], &data[i], len - i);
}

static void final_(Context& ctx, uint8_t digest[20]) {
    uint8_t finalcount[8];
    for (int i = 0; i < 8; ++i)
        finalcount[i] = (uint8_t)((ctx.count[(i >= 4 ? 0 : 1)]
                        >> ((3-(i&3))*8)) & 0xFF);
    uint8_t c = 0200;
    update(ctx, &c, 1);
    c = 0;
    while ((ctx.count[0] & 504) != 448) update(ctx, &c, 1);
    update(ctx, finalcount, 8);
    for (int i = 0; i < 20; ++i)
        digest[i] = (uint8_t)((ctx.state[i>>2] >> ((3-(i&3))*8)) & 0xFF);
}

std::string hash(const std::string& input) {
    Context ctx;
    init(ctx);
    update(ctx, (const uint8_t*)input.data(), input.size());
    uint8_t digest[20];
    final_(ctx, digest);
    return std::string((char*)digest, 20);
}

} // namespace SHA1

// ============================================================
// Base64 Encode
// ============================================================
std::string base64Encode(const std::string& input) {
    static const char tabel[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, bits = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        bits += 8;
        while (bits >= 0) {
            out += tabel[(val >> bits) & 0x3F];
            bits -= 6;
        }
    }
    if (bits > -6) out += tabel[((val << 8) >> (bits + 8)) & 0x3F];
    while (out.size() % 4) out += '=';
    return out;
}

// ============================================================
// Hitung Sec-WebSocket-Accept
// ============================================================
const std::string WS_MAGIC = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

std::string hitungAcceptKey(const std::string& clientKey) {
    return base64Encode(SHA1::hash(clientKey + WS_MAGIC));
}

// ============================================================
// Log Thread-Safe
// ============================================================
std::mutex gLogMux;

void wlog(const std::string& tag, const std::string& warna, const std::string& pesan) {
    std::lock_guard<std::mutex> lk(gLogMux);
    std::time_t t = std::time(nullptr);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    std::cout << "[" << buf << "] " << warna << tag << "\033[0m " << pesan << "\n";
    std::cout.flush();
}

// ============================================================
// WebSocket Frame Builder
// ============================================================
std::string buildFrame(const std::string& payload,
                       uint8_t opcode = 0x1,  // 0x1 = text
                       bool mask = false) {
    std::string frame;
    uint8_t byte0 = 0x80 | opcode;  // FIN bit set
    frame += (char)byte0;

    uint8_t maskBit = mask ? 0x80 : 0x00;
    size_t  len     = payload.size();

    if (len <= 125) {
        frame += (char)(maskBit | len);
    } else if (len <= 65535) {
        frame += (char)(maskBit | 126);
        frame += (char)((len >> 8) & 0xFF);
        frame += (char)(len & 0xFF);
    } else {
        frame += (char)(maskBit | 127);
        for (int i = 7; i >= 0; --i)
            frame += (char)((len >> (8*i)) & 0xFF);
    }

    if (mask) {
        uint8_t maskKey[4] = {0x12, 0x34, 0x56, 0x78};
        frame.append((char*)maskKey, 4);
        for (size_t i = 0; i < payload.size(); ++i)
            frame += (char)(payload[i] ^ maskKey[i % 4]);
    } else {
        frame += payload;
    }
    return frame;
}

// ============================================================
// WebSocket Frame Parser
// ============================================================
struct WsFrame {
    bool    fin;
    uint8_t opcode;
    bool    masked;
    std::string payload;
    bool valid = false;
    size_t bytesConsumed = 0;
};

WsFrame parseFrame(const std::string& data) {
    WsFrame f;
    if (data.size() < 2) return f;

    f.fin    = (data[0] & 0x80) != 0;
    f.opcode = data[0] & 0x0F;
    f.masked = (data[1] & 0x80) != 0;

    size_t offset = 2;
    uint64_t payloadLen = data[1] & 0x7F;

    if (payloadLen == 126) {
        if (data.size() < 4) return f;
        payloadLen = ((uint8_t)data[2] << 8) | (uint8_t)data[3];
        offset = 4;
    } else if (payloadLen == 127) {
        if (data.size() < 10) return f;
        payloadLen = 0;
        for (int i = 0; i < 8; ++i)
            payloadLen = (payloadLen << 8) | (uint8_t)data[2+i];
        offset = 10;
    }

    uint8_t maskKey[4] = {};
    if (f.masked) {
        if (data.size() < offset + 4) return f;
        memcpy(maskKey, data.data() + offset, 4);
        offset += 4;
    }

    if (data.size() < offset + payloadLen) return f;

    f.payload.resize(payloadLen);
    for (uint64_t i = 0; i < payloadLen; ++i)
        f.payload[i] = data[offset + i] ^ (f.masked ? maskKey[i % 4] : 0);

    f.valid = true;
    f.bytesConsumed = offset + (size_t)payloadLen;
    return f;
}

// ============================================================
// Baca semua data dari socket sampai tidak ada lagi
// ============================================================
std::string bacaSocket(int fd, size_t maxLen = 65536) {
    std::string out;
    char buf[4096];
    while (out.size() < maxLen) {
        ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
        if (n <= 0) break;
        out.append(buf, n);
    }
    return out;
}

// ============================================================
// SERVER — Handle satu WebSocket klien
// ============================================================
std::atomic<bool> gServerJalan{true};

void handleWsKlien(int fd) {
    // 1. Terima HTTP Upgrade request
    char buf[2048] = {};
    recv(fd, buf, sizeof(buf) - 1, 0);
    std::string httpReq(buf);

    // Cari Sec-WebSocket-Key
    std::string clientKey;
    auto pos = httpReq.find("Sec-WebSocket-Key:");
    if (pos == std::string::npos) { close(fd); return; }
    pos += 19;
    auto end = httpReq.find("\r\n", pos);
    clientKey = httpReq.substr(pos, end - pos);
    while (!clientKey.empty() && clientKey.front() == ' ')
        clientKey.erase(clientKey.begin());
    while (!clientKey.empty() && (clientKey.back() == ' ' || clientKey.back() == '\r'))
        clientKey.pop_back();

    // 2. Kirim HTTP 101 Switching Protocols
    std::string acceptKey = hitungAcceptKey(clientKey);
    std::string handshake =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + acceptKey + "\r\n\r\n";
    send(fd, handshake.c_str(), handshake.size(), 0);

    wlog("[SERVER]", "\033[36m", "Handshake selesai! Key: " + clientKey);
    wlog("[SERVER]", "\033[36m", "Accept:              " + acceptKey);

    // 3. Loop baca frame WebSocket
    std::string buffer;
    int pesanKe = 1;
    while (gServerJalan) {
        char rbuf[1024] = {};
        ssize_t n = recv(fd, rbuf, sizeof(rbuf) - 1, 0);
        if (n <= 0) break;
        buffer.append(rbuf, n);

        while (buffer.size() >= 2) {
            WsFrame frame = parseFrame(buffer);
            if (!frame.valid) break;
            buffer.erase(0, frame.bytesConsumed);

            if (frame.opcode == 0x8) {  // Close
                wlog("[SERVER]", "\033[36m", "Klien menutup koneksi.");
                goto selesai;
            }
            if (frame.opcode == 0x9) {  // Ping → Pong
                auto pong = buildFrame(frame.payload, 0xA);
                send(fd, pong.c_str(), pong.size(), 0);
                continue;
            }
            if (frame.opcode == 0x1 || frame.opcode == 0x2) {
                wlog("[SERVER]", "\033[36m",
                     "Terima[" + std::to_string(pesanKe) + "]: \"" + frame.payload + "\"");

                // Balas dengan uppercase
                std::string balas = frame.payload;
                std::transform(balas.begin(), balas.end(), balas.begin(), ::toupper);
                balas = "[ECHO #" + std::to_string(pesanKe++) + "] " + balas;

                auto f = buildFrame(balas);
                send(fd, f.c_str(), f.size(), 0);
                wlog("[SERVER]", "\033[36m", "Kirim  : \"" + balas + "\"");
            }
        }
    }
selesai:
    // Kirim close frame
    auto closeFrame = buildFrame("", 0x8);
    send(fd, closeFrame.c_str(), closeFrame.size(), 0);
    close(fd);
    wlog("[SERVER]", "\033[36m", "Koneksi ditutup.");
}

void threadServer(int port) {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    bind(sfd, (sockaddr*)&addr, sizeof(addr));
    listen(sfd, 5);

    wlog("[SERVER]", "\033[36m", "WebSocket server di ws://localhost:" + std::to_string(port));

    sockaddr_in ca{};
    socklen_t cl = sizeof(ca);
    int cfd = accept(sfd, (sockaddr*)&ca, &cl);
    if (cfd >= 0) handleWsKlien(cfd);

    close(sfd);
}

// ============================================================
// CLIENT — Buka koneksi WebSocket dan kirim pesan
// ============================================================
void threadClient(int port) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(fd, (sockaddr*)&addr, sizeof(addr));

    // 1. Kirim HTTP Upgrade
    std::string wsKey = base64Encode("contoh-kunci-ws!!");  // 16+ byte
    std::string upgrade =
        "GET /chat HTTP/1.1\r\n"
        "Host: localhost:" + std::to_string(port) + "\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: " + wsKey + "\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n";
    send(fd, upgrade.c_str(), upgrade.size(), 0);

    // 2. Baca 101 Switching Protocols
    char httpBuf[1024] = {};
    recv(fd, httpBuf, sizeof(httpBuf) - 1, 0);
    std::string httpResp(httpBuf);
    bool ok = httpResp.find("101") != std::string::npos;
    wlog("[CLIENT]", "\033[32m",
         ok ? "Handshake berhasil! (101 Switching Protocols)"
            : "Handshake GAGAL: " + httpResp.substr(0, 40));
    if (!ok) { close(fd); return; }

    // 3. Kirim beberapa pesan WebSocket
    std::vector<std::string> pesan = {
        "halo dari client!",
        "ini pesan websocket dua arah",
        "apakah kamu mendengarku?",
        "pesan terakhir — selamat tinggal!",
    };

    std::string buffer;
    for (auto& p : pesan) {
        // Kirim dengan mask (wajib dari client ke server per RFC 6455)
        auto frame = buildFrame(p, 0x1, true);
        send(fd, frame.c_str(), frame.size(), 0);
        wlog("[CLIENT]", "\033[32m", "Kirim  : \"" + p + "\"");

        // Tunggu balasan
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        char rbuf[1024] = {};
        ssize_t n = recv(fd, rbuf, sizeof(rbuf) - 1, 0);
        if (n > 0) {
            buffer.append(rbuf, n);
            WsFrame f = parseFrame(buffer);
            if (f.valid) {
                wlog("[CLIENT]", "\033[32m", "Terima : \"" + f.payload + "\"");
                buffer.erase(0, f.bytesConsumed);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    // 4. Kirim Ping
    wlog("[CLIENT]", "\033[32m", "Kirim Ping...");
    auto ping = buildFrame("ping-test", 0x9, true);
    send(fd, ping.c_str(), ping.size(), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    char pingBuf[64] = {};
    recv(fd, pingBuf, sizeof(pingBuf), 0);
    WsFrame pongFrame = parseFrame(std::string(pingBuf, 64));
    if (pongFrame.valid && pongFrame.opcode == 0xA)
        wlog("[CLIENT]", "\033[32m", "Pong diterima! payload=\"" + pongFrame.payload + "\"");

    // 5. Close
    auto closeF = buildFrame("", 0x8, true);
    send(fd, closeF.c_str(), closeF.size(), 0);

    gServerJalan = false;
    close(fd);
    wlog("[CLIENT]", "\033[32m", "Koneksi WebSocket ditutup.");
}

// ============================================================
// Main
// ============================================================
int main() {
    const int PORT = 54400;

    std::cout << "\n\033[1m"
              << "╔══════════════════════════════════════════════╗\n"
              << "║   WebSocket Demo — RFC 6455 dari Nol         ║\n"
              << "║   SHA1 + Base64 + Framing + Masking          ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << "\033[0m\n";

    std::thread ts(threadServer, PORT);
    std::thread tc(threadClient, PORT);

    ts.join();
    tc.join();

    std::cout << "\n\033[1m[SELESAI]\033[0m\n\n";
    return 0;
}
