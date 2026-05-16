// ============================================================
// File  : kontak.cpp
// Proyek: Manajemen Kontak / Buku Alamat
//
// Kompilasi:
//   g++ -std=c++17 -Wall -o kontak kontak.cpp
// Jalankan:
//   ./kontak
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>

// ============================================================
// Struct Kontak
// ============================================================
struct Kontak {
    int         id = 0;
    std::string nama;
    std::string telepon;
    std::string email;
    std::string alamat;
    std::string grup;         // keluarga, kerja, teman
    std::string catatan;

    std::string serialize() const {
        std::ostringstream ss;
        ss << id << "||" << nama << "||" << telepon << "||"
           << email << "||" << alamat << "||" << grup << "||" << catatan;
        return ss.str();
    }

    static Kontak deserialize(const std::string& line) {
        std::vector<std::string> parts;
        size_t found;
        std::string src = line;
        while ((found = src.find("||")) != std::string::npos) {
            parts.push_back(src.substr(0, found));
            src = src.substr(found + 2);
        }
        parts.push_back(src);

        Kontak k;
        if (parts.size() >= 7) {
            k.id      = std::stoi(parts[0]);
            k.nama    = parts[1];
            k.telepon = parts[2];
            k.email   = parts[3];
            k.alamat  = parts[4];
            k.grup    = parts[5];
            k.catatan = parts[6];
        }
        return k;
    }

    void tampilSingkat() const {
        std::cout << "  " << std::setw(4) << id << ".  "
                  << std::setw(22) << std::left << nama
                  << std::setw(16) << telepon
                  << std::setw(26) << email;
        if (!grup.empty()) std::cout << "[" << grup << "]";
        std::cout << "\n";
    }

    void tampilDetail() const {
        std::cout << "\n  ┌─────────────────────────────────────┐\n";
        std::cout << "  │  Kontak #" << id << "\n";
        std::cout << "  ├─────────────────────────────────────┤\n";
        std::cout << "  │  Nama    : " << nama << "\n";
        std::cout << "  │  Telepon : " << telepon << "\n";
        std::cout << "  │  Email   : " << email << "\n";
        std::cout << "  │  Alamat  : " << alamat << "\n";
        std::cout << "  │  Grup    : " << grup << "\n";
        if (!catatan.empty())
            std::cout << "  │  Catatan : " << catatan << "\n";
        std::cout << "  └─────────────────────────────────────┘\n";
    }
};

// ============================================================
// Kelas BukuAlamat
// ============================================================
class BukuAlamat {
    static constexpr const char* FILE_PATH = "kontak_data.txt";
    std::vector<Kontak> daftar;
    int nextId = 1;

    void simpan() {
        std::ofstream f(FILE_PATH);
        if (!f) return;
        f << nextId << "\n";
        for (const auto& k : daftar)
            f << k.serialize() << "\n";
    }

    void muat() {
        std::ifstream f(FILE_PATH);
        if (!f) return;
        std::string line;
        std::getline(f, line);
        try { nextId = std::stoi(line); } catch(...) { nextId = 1; }
        while (std::getline(f, line))
            if (!line.empty())
                try { daftar.push_back(Kontak::deserialize(line)); } catch(...) {}
    }

public:
    BukuAlamat() { muat(); }
    ~BukuAlamat() { simpan(); }

    void tambah(Kontak k) {
        k.id = nextId++;
        daftar.push_back(k);
        simpan();
        std::cout << "  ✓ Kontak '" << k.nama << "' ditambahkan (ID=" << k.id << ")\n";
    }

    bool hapus(int id) {
        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [id](const Kontak& k){ return k.id == id; });
        if (it == daftar.end()) { std::cout << "  ✗ ID tidak ditemukan\n"; return false; }
        std::cout << "  ✓ Kontak '" << it->nama << "' dihapus\n";
        daftar.erase(it);
        simpan();
        return true;
    }

    Kontak* cariId(int id) {
        auto it = std::find_if(daftar.begin(), daftar.end(),
                               [id](const Kontak& k){ return k.id == id; });
        return (it == daftar.end()) ? nullptr : &(*it);
    }

    void tampilSemua(const std::string& filterGrup = "") {
        std::cout << "\n  ══════════════════════════════════════════════════════════\n";
        std::cout << "    ID   Nama                   Telepon         Email\n";
        std::cout << "  ══════════════════════════════════════════════════════════\n";

        // Urutkan berdasarkan nama
        std::vector<const Kontak*> sorted;
        for (const auto& k : daftar) {
            if (!filterGrup.empty() && k.grup != filterGrup) continue;
            sorted.push_back(&k);
        }
        std::sort(sorted.begin(), sorted.end(), [](const Kontak* a, const Kontak* b){
            return a->nama < b->nama;
        });

        if (sorted.empty())
            std::cout << "  (tidak ada kontak)\n";
        else
            for (const auto* k : sorted) k->tampilSingkat();

        std::cout << "  ══════════════════════════════════════════════════════════\n";
        std::cout << "  Total: " << sorted.size() << " kontak\n\n";
    }

    void cari(const std::string& kata) {
        std::string kw = kata;
        std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);
        std::cout << "\n  Hasil pencarian '" << kata << "':\n";
        bool ada = false;
        for (const auto& k : daftar) {
            std::string n = k.nama, t = k.telepon, e = k.email;
            std::transform(n.begin(), n.end(), n.begin(), ::tolower);
            std::transform(t.begin(), t.end(), t.begin(), ::tolower);
            std::transform(e.begin(), e.end(), e.begin(), ::tolower);
            if (n.find(kw)!=std::string::npos ||
                t.find(kw)!=std::string::npos ||
                e.find(kw)!=std::string::npos) {
                k.tampilSingkat();
                ada = true;
            }
        }
        if (!ada) std::cout << "  (tidak ditemukan)\n";
        std::cout << "\n";
    }

    std::vector<std::string> daftarGrup() const {
        std::map<std::string,int> grupCount;
        for (const auto& k : daftar) if (!k.grup.empty()) grupCount[k.grup]++;
        std::vector<std::string> result;
        for (const auto& [g,_] : grupCount) result.push_back(g);
        return result;
    }

    void exportCSV(const std::string& filename) {
        std::ofstream f(filename);
        if (!f) { std::cout << "  ✗ Gagal export\n"; return; }
        f << "ID,Nama,Telepon,Email,Alamat,Grup,Catatan\n";
        for (const auto& k : daftar)
            f << k.id << ",\"" << k.nama << "\",\"" << k.telepon << "\",\""
              << k.email << "\",\"" << k.alamat << "\",\""
              << k.grup << "\",\"" << k.catatan << "\"\n";
        std::cout << "  ✓ Diekspor ke " << filename << " (" << daftar.size() << " kontak)\n";
    }
};

// ============================================================
// Helper input
// ============================================================
std::string inputStr(const std::string& prompt, bool wajib = false) {
    while(true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        if (!wajib || !s.empty()) return s;
        std::cout << "  (Wajib diisi)\n";
    }
}

int inputInt(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    try { return std::stoi(s); } catch(...) { return -1; }
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n  ╔════════════════════════════╗\n";
    std::cout << "  ║  BUKU ALAMAT C++  v1.0    ║\n";
    std::cout << "  ╚════════════════════════════╝\n";

    BukuAlamat buku;

    while (true) {
        std::cout << "\n  [1] Lihat semua   [2] Tambah    [3] Detail\n";
        std::cout << "  [4] Edit          [5] Hapus     [6] Cari\n";
        std::cout << "  [7] Filter grup   [8] Export CSV [0] Keluar\n";
        std::cout << "  Pilih: ";
        std::string pil;
        std::getline(std::cin, pil);

        switch (pil.empty() ? '?' : pil[0]) {
        case '1':
            buku.tampilSemua();
            break;

        case '2': {
            std::cout << "\n  ── Tambah Kontak Baru ──\n";
            Kontak k;
            k.nama    = inputStr("  Nama    : ", true);
            k.telepon = inputStr("  Telepon : ");
            k.email   = inputStr("  Email   : ");
            k.alamat  = inputStr("  Alamat  : ");
            k.grup    = inputStr("  Grup    : ");
            k.catatan = inputStr("  Catatan : ");
            buku.tambah(k);
            break;
        }

        case '3': {
            int id = inputInt("  ID kontak: ");
            Kontak* k = buku.cariId(id);
            if (k) k->tampilDetail();
            else std::cout << "  ✗ Tidak ditemukan\n";
            break;
        }

        case '4': {
            int id = inputInt("  ID kontak yang akan diedit: ");
            Kontak* k = buku.cariId(id);
            if (!k) { std::cout << "  ✗ Tidak ditemukan\n"; break; }
            k->tampilDetail();
            std::cout << "  (Kosongkan untuk tidak mengubah)\n";
            std::string tmp;
            tmp = inputStr("  Nama [" + k->nama + "]: ");
            if (!tmp.empty()) k->nama = tmp;
            tmp = inputStr("  Telepon [" + k->telepon + "]: ");
            if (!tmp.empty()) k->telepon = tmp;
            tmp = inputStr("  Email [" + k->email + "]: ");
            if (!tmp.empty()) k->email = tmp;
            std::cout << "  ✓ Kontak diperbarui\n";
            break;
        }

        case '5': {
            buku.tampilSemua();
            int id = inputInt("  ID yang akan dihapus: ");
            if (id > 0) buku.hapus(id);
            break;
        }

        case '6': {
            std::string kata = inputStr("  Kata kunci: ");
            if (!kata.empty()) buku.cari(kata);
            break;
        }

        case '7': {
            auto grups = buku.daftarGrup();
            if (grups.empty()) { std::cout << "  (belum ada grup)\n"; break; }
            std::cout << "  Grup tersedia:\n";
            for (const auto& g : grups) std::cout << "    - " << g << "\n";
            std::string g = inputStr("  Filter grup: ");
            buku.tampilSemua(g);
            break;
        }

        case '8':
            buku.exportCSV("kontak_export.csv");
            break;

        case '0':
            std::cout << "\n  Data tersimpan. Sampai jumpa!\n\n";
            return 0;
        }
    }
}
