// ============================================================
// File  : tcp_echo.cpp
// Topik : TCP Echo Server + Client — Simulasi dalam satu proses
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o tcp_echo tcp_echo.cpp
// Jalankan:
//   ./tcp_echo
// ============================================================

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <vector>

// POSIX socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ============================================================
// Utilitas — Log dengan timestamp
// ============================================================
std::atomic<bool> gBerjalan{true};

void log(const std::string& pihak, const std::string& pesan) {
    auto now = std::chrono::system_clock::now();
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm   = std::localtime(&t);

    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", tm);

    std::printf("[%s.%03lld] %-8s %s\n",
                buf, (long long)ms.count(), pihak.c_str(), pesan.c_str());
    std::fflush(stdout);
}

// ============================================================
// SERVER — Menerima koneksi dan membalas pesan
// ============================================================
void jalankanServer(int port) {
    // 1. Buat socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) { log("[SERVER]", "Gagal buat socket"); return; }

    // Agar port bisa dipakai ulang segera setelah program berhenti
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind ke port
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;   // terima dari semua antarmuka
    addr.sin_port        = htons(port);

    if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        log("[SERVER]", "Gagal bind"); close(serverFd); return;
    }

    // 3. Mulai dengarkan
    listen(serverFd, 5);
    log("[SERVER]", "Mendengarkan di port " + std::to_string(port) + " ...");

    // 4. Terima satu koneksi
    sockaddr_in clientAddr{};
    socklen_t   addrLen = sizeof(clientAddr);
    int clientFd = accept(serverFd, (sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) { log("[SERVER]", "Gagal accept"); close(serverFd); return; }

    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, sizeof(ipStr));
    log("[SERVER]", std::string("Klien terhubung dari ") + ipStr);

    // 5. Loop — terima & balas
    char buf[1024];
    int  no = 1;
    while (gBerjalan) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(clientFd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        std::string terima(buf, n);
        log("[SERVER]", "Terima: \"" + terima + "\"");

        std::string balas = "[ECHO #" + std::to_string(no++) + "] " + terima;
        send(clientFd, balas.c_str(), balas.size(), 0);
        log("[SERVER]", "Kirim : \"" + balas + "\"");
    }

    log("[SERVER]", "Klien memutus koneksi. Server selesai.");
    close(clientFd);
    close(serverFd);
}

// ============================================================
// CLIENT — Mengirim pesan dan menerima balasan
// ============================================================
void jalankanClient(int port) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 1. Buat socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { log("[CLIENT]", "Gagal buat socket"); return; }

    // 2. Hubungkan ke server
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        log("[CLIENT]", "Gagal connect"); close(fd); return;
    }
    log("[CLIENT]", "Terhubung ke server!");

    // 3. Kirim beberapa pesan
    std::vector<std::string> pesanList = {
        "Halo Server!",
        "Ini pesan kedua.",
        "Apakah kamu bisa mendengarku?",
        "Pesan terakhir — sampai jumpa!"
    };

    char buf[1024];
    for (const auto& pesan : pesanList) {
        send(fd, pesan.c_str(), pesan.size(), 0);
        log("[CLIENT]", "Kirim : \"" + pesan + "\"");

        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n > 0) log("[CLIENT]", "Terima: \"" + std::string(buf, n) + "\"");

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    gBerjalan = false;
    close(fd);
    log("[CLIENT]", "Koneksi ditutup.");
}

// ============================================================
// Main
// ============================================================
int main() {
    const int PORT = 54321;

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   TCP ECHO — Simulasi Client-Server      ║\n";
    std::cout << "║   Port: " << PORT << "                          ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    // Jalankan server dan client di thread terpisah
    std::thread tServer(jalankanServer, PORT);
    std::thread tClient(jalankanClient, PORT);

    tServer.join();
    tClient.join();

    std::cout << "\n[SELESAI] Program berakhir.\n\n";
    return 0;
}
