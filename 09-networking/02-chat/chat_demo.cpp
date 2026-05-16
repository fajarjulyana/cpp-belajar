// ============================================================
// File  : chat_demo.cpp
// Topik : Simulasi Chat Client-Server — Satu Proses, Multi-Thread
//
// File ini menjalankan server + 3 klien dalam SATU PROSES.
// Tidak perlu buka banyak terminal. Cocok untuk belajar
// dan memahami alur komunikasi socket dua arah.
//
// Kompilasi:
//   g++ -std=c++17 -pthread -o chat_demo chat_demo.cpp
// Jalankan:
//   ./chat_demo
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <sstream>
#include <cstring>
#include <ctime>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// ============================================================
// Warna ANSI (agar output lebih mudah dibaca)
// ============================================================
namespace Warna {
    const char* RESET  = "\033[0m";
    const char* MERAH  = "\033[31m";
    const char* HIJAU  = "\033[32m";
    const char* KUNING = "\033[33m";
    const char* BIRU   = "\033[34m";
    const char* UNGU   = "\033[35m";
    const char* CYAN   = "\033[36m";
    const char* TEBAL  = "\033[1m";
}

static const int PORT = 54300;

// ============================================================
// Mutex untuk output agar tidak saling tumpang tindih
// ============================================================
std::mutex gPrintMutex;

void cetak(const std::string& tag, const std::string& warna,
           const std::string& pesan) {
    std::lock_guard<std::mutex> lock(gPrintMutex);
    std::time_t t = std::time(nullptr);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    std::cout << "[" << buf << "] "
              << warna << std::left
              << tag << Warna::RESET
              << " " << pesan << "\n";
    std::cout.flush();
}

// ============================================================
// State server — daftar klien yang terhubung
// ============================================================
struct KlienInfo {
    int         fd;
    std::string nama;
};

std::mutex              gMutex;
std::vector<KlienInfo>  gKlien;
std::atomic<bool>       gServerBerjalan{true};

void tambahKlien(int fd, const std::string& nama) {
    std::lock_guard<std::mutex> lk(gMutex);
    gKlien.push_back({fd, nama});
}

void hapusKlien(int fd) {
    std::lock_guard<std::mutex> lk(gMutex);
    gKlien.erase(std::remove_if(gKlien.begin(), gKlien.end(),
        [fd](const KlienInfo& k){ return k.fd == fd; }), gKlien.end());
}

void updateNama(int fd, const std::string& nama) {
    std::lock_guard<std::mutex> lk(gMutex);
    for (auto& k : gKlien) if (k.fd == fd) { k.nama = nama; break; }
}

std::string dapatNama(int fd) {
    std::lock_guard<std::mutex> lk(gMutex);
    for (auto& k : gKlien) if (k.fd == fd) return k.nama;
    return "?";
}

int cariFd(const std::string& nama) {
    std::lock_guard<std::mutex> lk(gMutex);
    for (auto& k : gKlien) if (k.nama == nama) return k.fd;
    return -1;
}

std::string daftarNama() {
    std::lock_guard<std::mutex> lk(gMutex);
    std::string hasil;
    for (auto& k : gKlien) hasil += k.nama + " ";
    return hasil.empty() ? "(kosong)" : hasil;
}

// Kirim pesan ke semua kecuali pengirim
void broadcast(const std::string& pesan, int kecuali = -1) {
    std::lock_guard<std::mutex> lk(gMutex);
    std::string baris = pesan + "\n";
    for (auto& k : gKlien) {
        if (k.fd != kecuali)
            send(k.fd, baris.c_str(), baris.size(), MSG_NOSIGNAL);
    }
}

void kirim(int fd, const std::string& pesan) {
    std::string baris = pesan + "\n";
    send(fd, baris.c_str(), baris.size(), MSG_NOSIGNAL);
}

// ============================================================
// Handler per klien di sisi server
// ============================================================
void handlerServerKlien(int fd) {
    std::string nama = dapatNama(fd);

    kirim(fd, "=== Selamat datang, " + nama + "! ===");
    kirim(fd, "Perintah: /nama <nama>  /daftar  /pm <nama> <pesan>  /keluar");
    broadcast("*** " + nama + " bergabung ***", fd);

    char buf[1024];
    while (gServerBerjalan) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;

        std::string baris(buf, n);
        while (!baris.empty() && (baris.back() == '\n' || baris.back() == '\r'))
            baris.pop_back();
        if (baris.empty()) continue;

        nama = dapatNama(fd);

        if (baris.rfind("/nama ", 0) == 0) {
            std::string namaBaru = baris.substr(6);
            broadcast("*** " + nama + " → " + namaBaru + " ***");
            updateNama(fd, namaBaru);
            kirim(fd, "[SERVER] Nama kamu: " + namaBaru);

        } else if (baris == "/daftar") {
            kirim(fd, "[SERVER] Online: " + daftarNama());

        } else if (baris.rfind("/pm ", 0) == 0) {
            std::istringstream ss(baris.substr(4));
            std::string tujuan, isi;
            ss >> tujuan;
            std::getline(ss, isi);
            if (!isi.empty() && isi[0] == ' ') isi = isi.substr(1);
            int tFd = cariFd(tujuan);
            if (tFd < 0) {
                kirim(fd, "[SERVER] User tidak ditemukan: " + tujuan);
            } else {
                kirim(tFd, "[PM dari " + nama + "] " + isi);
                kirim(fd,  "[PM → " + tujuan + "] " + isi);
            }

        } else if (baris == "/keluar") {
            kirim(fd, "[SERVER] Sampai jumpa!");
            break;

        } else {
            // Pesan biasa — broadcast
            std::string keluar = nama + ": " + baris;
            cetak("[SERVER]", Warna::CYAN, "broadcast → " + keluar);
            broadcast(keluar, fd);
        }
    }

    hapusKlien(fd);
    close(fd);
    broadcast("*** " + nama + " keluar ***");
}

// ============================================================
// Thread utama server
// ============================================================
void threadServer() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);
    bind(sfd, (sockaddr*)&addr, sizeof(addr));
    listen(sfd, 10);

    cetak("[SERVER]", Warna::CYAN, "Mendengarkan di port " + std::to_string(PORT));

    int no = 1;
    while (gServerBerjalan) {
        sockaddr_in ca{};
        socklen_t   cl = sizeof(ca);
        int cfd = accept(sfd, (sockaddr*)&ca, &cl);
        if (cfd < 0) break;

        std::string nama = "Tamu" + std::to_string(no++);
        tambahKlien(cfd, nama);
        cetak("[SERVER]", Warna::CYAN, "Klien terhubung → " + nama);
        std::thread(handlerServerKlien, cfd).detach();
    }
    close(sfd);
}

// ============================================================
// Thread klien simulasi — percakapan otomatis
// ============================================================
struct SkenarioKlien {
    std::string nama;        // nama tampilan
    std::string warna;       // warna ANSI
    std::vector<std::pair<int, std::string>> aksi;
    // aksi: { delay_ms, pesan }
};

void threadKlien(SkenarioKlien sk) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cetak("[" + sk.nama + "]", sk.warna, "Gagal connect!");
        return;
    }

    // Thread kecil untuk terima pesan dari server
    std::atomic<bool> aktif{true};
    std::thread tTerima([&](){
        char buf[1024];
        while (aktif) {
            memset(buf, 0, sizeof(buf));
            ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
            if (n <= 0) break;
            std::string baris(buf, n);
            while (!baris.empty() && (baris.back() == '\n' || baris.back() == '\r'))
                baris.pop_back();
            cetak("[" + sk.nama + "]", sk.warna, "← " + baris);
        }
    });

    // Atur nama dulu
    std::string setNama = "/nama " + sk.nama + "\n";
    send(fd, setNama.c_str(), setNama.size(), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Jalankan skenario aksi
    for (auto& [delay, pesan] : sk.aksi) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        cetak("[" + sk.nama + "]", sk.warna, "→ kirim: \"" + pesan + "\"");
        std::string kirim = pesan + "\n";
        send(fd, kirim.c_str(), kirim.size(), 0);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    aktif = false;
    shutdown(fd, SHUT_RDWR);
    close(fd);

    if (tTerima.joinable()) tTerima.join();
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n"
              << Warna::TEBAL
              << "╔══════════════════════════════════════════════╗\n"
              << "║   C++ CHAT DEMO — Simulasi Tiga Pengguna    ║\n"
              << "║   (Server + 3 Client dalam satu proses)     ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << Warna::RESET << "\n";

    // Jalankan server di background
    std::thread tSrv(threadServer);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Definisi skenario percakapan tiga pengguna
    SkenarioKlien budi{
        "Budi", Warna::HIJAU,
        {
            {200,  "Halo semua!"},
            {800,  "Ada yang bisa bantu aku belajar C++?"},
            {1500, "/daftar"},
            {600,  "Wah ada Sari dan Eko juga!"},
            {800,  "/pm Sari Hai Sari, lagi sibuk?"},
            {1200, "Oke sampai jumpa semua!"},
            {300,  "/keluar"},
        }
    };

    SkenarioKlien sari{
        "Sari", Warna::UNGU,
        {
            {400,  "Hai Budi! Aku di sini juga :)"},
            {900,  "C++ memang butuh latihan rutin"},
            {700,  "/pm Budi Enggak sibuk, mau tanya apa?"},
            {1100, "Bye Budi, semangat belajarnya!"},
            {800,  "/keluar"},
        }
    };

    SkenarioKlien eko{
        "Eko", Warna::KUNING,
        {
            {600,  "Halo! Eko hadir!"},
            {1000, "Kalau mau belajar networking, socket programming itu seru"},
            {900,  "Kode chat ini dibuat pakai POSIX socket lho"},
            {1500, "Oke aku duluan, ada meeting"},
            {300,  "/keluar"},
        }
    };

    // Jalankan tiga klien dengan delay agar tidak terhubung bersamaan
    std::thread t1([&](){ threadKlien(budi); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t2([&](){ threadKlien(sari); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread t3([&](){ threadKlien(eko); });

    t1.join();
    t2.join();
    t3.join();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    gServerBerjalan = false;

    // Hubungkan dummy untuk unblock accept()
    int tmp = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_port   = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
    connect(tmp, (sockaddr*)&a, sizeof(a));
    close(tmp);

    if (tSrv.joinable()) tSrv.join();

    std::cout << "\n" << Warna::TEBAL
              << "═══════════════════════════════════════════════\n"
              << " Simulasi selesai.\n"
              << " Lihat chat_server.cpp dan chat_client.cpp\n"
              << " untuk versi interaktif (dua terminal).\n"
              << "═══════════════════════════════════════════════\n"
              << Warna::RESET << "\n";
    return 0;
}
