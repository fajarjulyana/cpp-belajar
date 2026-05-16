// ============================================================
// File  : chat_client.cpp
// Topik : Chat Client Interaktif — TCP POSIX Sockets
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o chat_client chat_client.cpp
// Jalankan:
//   ./chat_client 127.0.0.1 7777
// ============================================================

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::atomic<bool> gBerjalan{true};

// ============================================================
// Thread penerima — menampilkan pesan dari server
// ============================================================
void threadTerima(int fd) {
    char buf[2048];
    while (gBerjalan) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) {
            if (gBerjalan) {
                std::cout << "\n[!] Koneksi ke server terputus.\n";
            }
            gBerjalan = false;
            break;
        }
        // Tampilkan di atas prompt (sederhana, tanpa TUI library)
        std::cout << "\r" << std::string(60, ' ') << "\r"; // hapus baris
        std::cout << std::string(buf, n);
        std::cout.flush();
        // Tampilkan ulang prompt
        std::cout << "> ";
        std::cout.flush();
    }
}

// ============================================================
// Main — loop kirim pesan
// ============================================================
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Penggunaan: " << argv[0] << " <host> <port>\n";
        std::cerr << "Contoh   : " << argv[0] << " 127.0.0.1 7777\n";
        return 1;
    }

    const char* host = argv[1];
    int         port = std::stoi(argv[2]);

    // Buat socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { std::perror("socket"); return 1; }

    // Hubungkan ke server
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        std::cerr << "Alamat tidak valid: " << host << "\n";
        return 1;
    }

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::perror("connect");
        std::cerr << "Pastikan server sudah berjalan di " << host << ":" << port << "\n";
        return 1;
    }

    std::cout << "Terhubung ke " << host << ":" << port << "\n";
    std::cout << "Ketik pesan dan tekan Enter. Ketik /keluar untuk keluar.\n\n";

    // Thread penerima berjalan di background
    std::thread tTerima(threadTerima, fd);

    // Loop pengirim — thread utama
    std::string baris;
    while (gBerjalan) {
        std::cout << "> ";
        std::cout.flush();

        if (!std::getline(std::cin, baris)) break;
        if (baris.empty()) continue;

        // Kirim ke server
        std::string kirim = baris + "\n";
        send(fd, kirim.c_str(), kirim.size(), MSG_NOSIGNAL);

        if (baris == "/keluar") {
            gBerjalan = false;
            break;
        }
    }

    gBerjalan = false;
    shutdown(fd, SHUT_RDWR);
    close(fd);

    if (tTerima.joinable()) tTerima.join();

    std::cout << "Keluar dari chat.\n";
    return 0;
}
