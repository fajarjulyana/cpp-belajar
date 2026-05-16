// ============================================================
// File  : todo.cpp
// Proyek: To-Do List dengan File Persistence
//
// Kompilasi:
//   g++ -std=c++17 -Wall -o todo todo.cpp
// Jalankan:
//   ./todo
// Data disimpan di: todo_data.txt (otomatis)
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

// ============================================================
// Enum & Struct
// ============================================================
enum class Prioritas { Rendah = 1, Sedang = 2, Tinggi = 3 };
enum class Status    { Belum, Proses, Selesai };

std::string namaStatus(Status s) {
    switch(s) {
    case Status::Belum:   return "⏳ Belum";
    case Status::Proses:  return "🔄 Proses";
    case Status::Selesai: return "✅ Selesai";
    }
    return "?";
}

std::string namaPrioritas(Prioritas p) {
    switch(p) {
    case Prioritas::Rendah: return "[ ]";
    case Prioritas::Sedang: return "[!]";
    case Prioritas::Tinggi: return "[!!]";
    }
    return "";
}

struct Tugas {
    int       id;
    std::string judul;
    std::string deskripsi;
    Prioritas prioritas = Prioritas::Sedang;
    Status    status    = Status::Belum;
    std::string kategori;

    // Serialisasi ke string untuk disimpan ke file
    std::string serialize() const {
        std::ostringstream ss;
        ss << id << "|"
           << judul << "|"
           << deskripsi << "|"
           << (int)prioritas << "|"
           << (int)status << "|"
           << kategori;
        return ss.str();
    }

    // Deserialisasi dari string
    static Tugas deserialize(const std::string& line) {
        std::istringstream ss(line);
        std::string token;
        Tugas t;
        std::getline(ss, token, '|'); t.id = std::stoi(token);
        std::getline(ss, t.judul, '|');
        std::getline(ss, t.deskripsi, '|');
        std::getline(ss, token, '|'); t.prioritas = (Prioritas)std::stoi(token);
        std::getline(ss, token, '|'); t.status = (Status)std::stoi(token);
        std::getline(ss, t.kategori);
        return t;
    }

    void tampil(bool detail = false) const {
        std::cout << "  " << std::setw(4) << id << ". "
                  << namaPrioritas(prioritas) << " "
                  << std::setw(35) << std::left << judul << " "
                  << std::setw(14) << namaStatus(status);
        if (!kategori.empty())
            std::cout << " [" << kategori << "]";
        std::cout << "\n";
        if (detail && !deskripsi.empty())
            std::cout << "       └─ " << deskripsi << "\n";
    }
};

// ============================================================
// Kelas TodoApp
// ============================================================
class TodoApp {
    static constexpr const char* FILE_PATH = "todo_data.txt";
    std::vector<Tugas> daftar;
    int nextId = 1;

public:
    TodoApp() { muat(); }
    ~TodoApp() { simpan(); }

    // Simpan ke file
    void simpan() const {
        std::ofstream f(FILE_PATH);
        if (!f) return;
        f << nextId << "\n";
        for (const auto& t : daftar)
            f << t.serialize() << "\n";
    }

    // Muat dari file
    void muat() {
        std::ifstream f(FILE_PATH);
        if (!f) return;
        std::string line;
        std::getline(f, line);
        try { nextId = std::stoi(line); } catch(...) { nextId = 1; }
        while (std::getline(f, line)) {
            if (!line.empty()) {
                try { daftar.push_back(Tugas::deserialize(line)); }
                catch(...) {}
            }
        }
    }

    // Tambah tugas baru
    void tambah(const std::string& judul, const std::string& desk = "",
                Prioritas p = Prioritas::Sedang, const std::string& kat = "") {
        Tugas t;
        t.id          = nextId++;
        t.judul       = judul;
        t.deskripsi   = desk;
        t.prioritas   = p;
        t.kategori    = kat;
        daftar.push_back(t);
        std::cout << "  ✓ Tugas ditambahkan (ID=" << t.id << ")\n";
        simpan();
    }

    // Hapus tugas berdasarkan ID
    bool hapus(int id) {
        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [id](const Tugas& t){ return t.id == id; });
        if (it == daftar.end()) { std::cout << "  ✗ ID tidak ditemukan\n"; return false; }
        std::cout << "  ✓ Tugas '" << it->judul << "' dihapus\n";
        daftar.erase(it);
        simpan();
        return true;
    }

    // Ubah status
    bool setStatus(int id, Status s) {
        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [id](const Tugas& t){ return t.id == id; });
        if (it == daftar.end()) { std::cout << "  ✗ ID tidak ditemukan\n"; return false; }
        it->status = s;
        std::cout << "  ✓ Status diperbarui: " << namaStatus(s) << "\n";
        simpan();
        return true;
    }

    // Tandai selesai
    bool selesai(int id) { return setStatus(id, Status::Selesai); }

    // Tampilkan semua
    void tampilSemua(Status* filterStatus = nullptr,
                     Prioritas* filterPrioritas = nullptr,
                     const std::string& filterKat = "") {
        std::cout << "\n  ══════════════════════════════════════════════════\n";
        std::cout << "   ID    Pri  Judul                               Status\n";
        std::cout << "  ══════════════════════════════════════════════════\n";

        int hitungan = 0;
        for (const auto& t : daftar) {
            if (filterStatus && t.status != *filterStatus) continue;
            if (filterPrioritas && t.prioritas != *filterPrioritas) continue;
            if (!filterKat.empty() && t.kategori != filterKat) continue;
            t.tampil();
            hitungan++;
        }

        if (hitungan == 0)
            std::cout << "  (tidak ada tugas)\n";

        // Statistik
        int belum = 0, proses = 0, selesaiCount = 0;
        for (const auto& t : daftar) {
            if (t.status == Status::Belum)   belum++;
            if (t.status == Status::Proses)  proses++;
            if (t.status == Status::Selesai) selesaiCount++;
        }
        std::cout << "  ══════════════════════════════════════════════════\n";
        std::cout << "  Total: " << daftar.size()
                  << "  ⏳ " << belum
                  << "  🔄 " << proses
                  << "  ✅ " << selesaiCount << "\n\n";
    }

    // Cari tugas
    void cari(const std::string& kata) {
        std::cout << "\n  Hasil pencarian '" << kata << "':\n";
        bool ada = false;
        for (const auto& t : daftar) {
            std::string judul_lower = t.judul;
            std::string kata_lower  = kata;
            std::transform(judul_lower.begin(), judul_lower.end(), judul_lower.begin(), ::tolower);
            std::transform(kata_lower.begin(), kata_lower.end(), kata_lower.begin(), ::tolower);
            if (judul_lower.find(kata_lower) != std::string::npos ||
                t.deskripsi.find(kata) != std::string::npos) {
                t.tampil(true);
                ada = true;
            }
        }
        if (!ada) std::cout << "  (tidak ditemukan)\n";
        std::cout << "\n";
    }

    // Urutkan berdasarkan prioritas (tinggi dulu)
    void urutkan() {
        std::sort(daftar.begin(), daftar.end(), [](const Tugas& a, const Tugas& b){
            if (a.status != b.status) return (int)a.status < (int)b.status;
            return (int)a.prioritas > (int)b.prioritas;
        });
        simpan();
        std::cout << "  ✓ Daftar diurutkan\n";
    }
};

// ============================================================
// Fungsi input helper
// ============================================================
std::string inputStr(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

int inputInt(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    try { return std::stoi(s); } catch(...) { return -1; }
}

Prioritas pilihPrioritas() {
    std::cout << "  Prioritas: [1] Rendah  [2] Sedang  [3] Tinggi\n";
    int p = inputInt("  Pilih: ");
    if (p == 1) return Prioritas::Rendah;
    if (p == 3) return Prioritas::Tinggi;
    return Prioritas::Sedang;
}

// ============================================================
// Menu utama
// ============================================================
void tampilMenu() {
    std::cout << "\n  ┌─────────────────────────────┐\n";
    std::cout << "  │    TO-DO LIST  C++  v1.0    │\n";
    std::cout << "  ├─────────────────────────────┤\n";
    std::cout << "  │ [1] Tampilkan semua tugas   │\n";
    std::cout << "  │ [2] Tambah tugas baru       │\n";
    std::cout << "  │ [3] Tandai selesai          │\n";
    std::cout << "  │ [4] Ubah status             │\n";
    std::cout << "  │ [5] Hapus tugas             │\n";
    std::cout << "  │ [6] Cari tugas              │\n";
    std::cout << "  │ [7] Urutkan & filter        │\n";
    std::cout << "  │ [0] Keluar                  │\n";
    std::cout << "  └─────────────────────────────┘\n";
    std::cout << "  Pilih: ";
}

int main() {
    TodoApp app;

    // Contoh data awal jika kosong (opsional)
    // Akan disimpan ke file sehingga tidak ditambah lagi saat restart

    while (true) {
        tampilMenu();
        std::string pil;
        std::getline(std::cin, pil);

        switch (pil.empty() ? '?' : pil[0]) {
        case '1':
            app.tampilSemua();
            break;

        case '2': {
            std::cout << "\n  ── Tambah Tugas Baru ──\n";
            std::string judul = inputStr("  Judul: ");
            if (judul.empty()) { std::cout << "  Judul tidak boleh kosong.\n"; break; }
            std::string desk = inputStr("  Deskripsi (opsional): ");
            Prioritas p = pilihPrioritas();
            std::string kat = inputStr("  Kategori (opsional): ");
            app.tambah(judul, desk, p, kat);
            break;
        }

        case '3': {
            app.tampilSemua();
            int id = inputInt("  ID tugas yang selesai: ");
            if (id > 0) app.selesai(id);
            break;
        }

        case '4': {
            app.tampilSemua();
            int id = inputInt("  ID tugas: ");
            if (id <= 0) break;
            std::cout << "  Status: [1] Belum  [2] Proses  [3] Selesai\n";
            int s = inputInt("  Pilih: ");
            Status status = (s == 2) ? Status::Proses : (s == 3) ? Status::Selesai : Status::Belum;
            app.setStatus(id, status);
            break;
        }

        case '5': {
            app.tampilSemua();
            int id = inputInt("  ID tugas yang akan dihapus: ");
            if (id > 0) app.hapus(id);
            break;
        }

        case '6': {
            std::string kata = inputStr("  Kata kunci: ");
            if (!kata.empty()) app.cari(kata);
            break;
        }

        case '7':
            app.urutkan();
            app.tampilSemua();
            break;

        case '0':
            std::cout << "\n  Data tersimpan. Sampai jumpa!\n\n";
            return 0;

        default:
            std::cout << "  Pilihan tidak valid.\n";
        }
    }
}
