// ============================================================
// File  : chat_server.cpp
// Topik : Chat Server Multi-Klien — TCP POSIX Sockets
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o chat_server chat_server.cpp
// Jalankan:
//   ./chat_server          (default port 7777)
//   ./chat_server 9000     (port kustom)
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <ctime>
#include <atomic>
#include <map>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ============================================================
// Struktur data klien
// ============================================================
struct InfoKlien {
    int         fd;
    std::string nama;
    std::string ip;
    int         port;
    std::time_t waktuMasuk;
};

// ============================================================
// State global (protected by mutex)
// ============================================================
std::mutex              gMutex;
std::vector<InfoKlien>  gKlien;
std::atomic<bool>       gBerjalan{true};

// ============================================================
// Utilitas — timestamp
// ============================================================
std::string timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[10];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    return buf;
}

void logServer(const std::string& pesan) {
    std::cout << "[" << timestamp() << "] [SERVER] " << pesan << "\n";
    std::cout.flush();
}

// ============================================================
// Kirim pesan ke satu klien
// ============================================================
bool kirimKe(int fd, const std::string& pesan) {
    std::string baris = pesan + "\n";
    ssize_t n = send(fd, baris.c_str(), baris.size(), MSG_NOSIGNAL);
    return n > 0;
}

// ============================================================
// Broadcast ke semua klien kecuali pengirim
// ============================================================
void broadcast(const std::string& pesan, int kecualiFd = -1) {
    std::lock_guard<std::mutex> lock(gMutex);
    for (auto& k : gKlien) {
        if (k.fd != kecualiFd) {
            kirimKe(k.fd, pesan);
        }
    }
}

// ============================================================
// Cari klien berdasarkan nama
// ============================================================
int cariFdByNama(const std::string& nama) {
    std::lock_guard<std::mutex> lock(gMutex);
    for (auto& k : gKlien) {
        if (k.nama == nama) return k.fd;
    }
    return -1;
}

// ============================================================
// Tambah / hapus klien
// ============================================================
void tambahKlien(const InfoKlien& k) {
    std::lock_guard<std::mutex> lock(gMutex);
    gKlien.push_back(k);
}

void hapusKlien(int fd) {
    std::lock_guard<std::mutex> lock(gMutex);
    gKlien.erase(std::remove_if(gKlien.begin(), gKlien.end(),
        [fd](const InfoKlien& k){ return k.fd == fd; }), gKlien.end());
}

std::string namaDariFd(int fd) {
    std::lock_guard<std::mutex> lock(gMutex);
    for (auto& k : gKlien) {
        if (k.fd == fd) return k.nama;
    }
    return "?";
}

void updateNama(int fd, const std::string& namaBaru) {
    std::lock_guard<std::mutex> lock(gMutex);
    for (auto& k : gKlien) {
        if (k.fd == fd) { k.nama = namaBaru; break; }
    }
}

std::string daftarKlien() {
    std::lock_guard<std::mutex> lock(gMutex);
    if (gKlien.empty()) return "  (tidak ada user online)";
    std::string hasil;
    int no = 1;
    for (auto& k : gKlien) {
        hasil += "  " + std::to_string(no++) + ". " + k.nama +
                 " (" + k.ip + ":" + std::to_string(k.port) + ")\n";
    }
    return hasil;
}

// ============================================================
// Thread per klien — handle pesan & perintah
// ============================================================
void handleKlien(InfoKlien info) {
    std::string nama = info.nama;
    int fd           = info.fd;

    kirimKe(fd, "=== Selamat datang di C++ Chat! ===");
    kirimKe(fd, "Perintah: /nama <nama>  /daftar  /pm <nama> <pesan>  /keluar");
    kirimKe(fd, "---");

    broadcast("*** " + nama + " bergabung ***", fd);
    logServer(nama + " terhubung dari " + info.ip);

    char buf[2048];
    while (gBerjalan) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        std::string baris(buf, n);
        // Hapus newline
        while (!baris.empty() && (baris.back() == '\n' || baris.back() == '\r'))
            baris.pop_back();
        if (baris.empty()) continue;

        // --- Perintah /nama ---
        if (baris.rfind("/nama ", 0) == 0) {
            std::string namaBaru = baris.substr(6);
            if (namaBaru.empty()) {
                kirimKe(fd, "[SERVER] Penggunaan: /nama <nama>");
                continue;
            }
            std::string namaLama = namaDariFd(fd);
            updateNama(fd, namaBaru);
            nama = namaBaru;
            broadcast("*** " + namaLama + " berganti nama menjadi " + namaBaru + " ***");
            kirimKe(fd, "[SERVER] Nama kamu sekarang: " + namaBaru);
            logServer(namaLama + " berganti nama → " + namaBaru);
        }
        // --- Perintah /daftar ---
        else if (baris == "/daftar") {
            kirimKe(fd, "[SERVER] User online:\n" + daftarKlien());
        }
        // --- Perintah /pm ---
        else if (baris.rfind("/pm ", 0) == 0) {
            std::istringstream ss(baris.substr(4));
            std::string tujuan, isiPesan;
            ss >> tujuan;
            std::getline(ss, isiPesan);
            if (!isiPesan.empty() && isiPesan[0] == ' ')
                isiPesan = isiPesan.substr(1);

            int tujuanFd = cariFdByNama(tujuan);
            if (tujuanFd < 0) {
                kirimKe(fd, "[SERVER] User '" + tujuan + "' tidak ditemukan.");
            } else {
                std::string tag = "[PM dari " + nama + "] " + isiPesan;
                kirimKe(tujuanFd, tag);
                kirimKe(fd, "[PM → " + tujuan + "] " + isiPesan);
                logServer("PM: " + nama + " → " + tujuan + ": " + isiPesan);
            }
        }
        // --- Perintah /keluar ---
        else if (baris == "/keluar") {
            kirimKe(fd, "[SERVER] Sampai jumpa, " + nama + "!");
            break;
        }
        // --- Pesan biasa (broadcast) ---
        else {
            std::string keluar = "[" + timestamp() + "] " + nama + ": " + baris;
            broadcast(keluar, fd);
            logServer(keluar);
        }
    }

    hapusKlien(fd);
    close(fd);
    broadcast("*** " + nama + " meninggalkan chat ***");
    logServer(nama + " memutus koneksi.");
}

// ============================================================
// Main
// ============================================================
int main(int argc, char* argv[]) {
    int port = (argc > 1) ? std::stoi(argv[1]) : 7777;

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) { std::perror("socket"); return 1; }

    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::perror("bind"); return 1;
    }

    listen(serverFd, 10);

    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║          C++ CHAT SERVER v1.0            ║\n";
    std::cout << "║  Port  : " << std::to_string(port)
              << std::string(32 - std::to_string(port).size(), ' ') << "║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";
    logServer("Mendengarkan di port " + std::to_string(port) + " ...");
    logServer("Tekan Ctrl+C untuk berhenti.\n");

    int noKlien = 1;
    while (gBerjalan) {
        sockaddr_in klienAddr{};
        socklen_t   len = sizeof(klienAddr);
        int klienFd = accept(serverFd, (sockaddr*)&klienAddr, &len);
        if (klienFd < 0) break;

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &klienAddr.sin_addr, ipStr, sizeof(ipStr));

        InfoKlien info;
        info.fd         = klienFd;
        info.nama       = "Tamu" + std::to_string(noKlien++);
        info.ip         = ipStr;
        info.port       = ntohs(klienAddr.sin_port);
        info.waktuMasuk = std::time(nullptr);

        tambahKlien(info);

        std::thread t(handleKlien, info);
        t.detach();
    }

    close(serverFd);
    return 0;
}
