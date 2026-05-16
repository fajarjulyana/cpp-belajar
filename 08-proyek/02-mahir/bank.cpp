// ============================================================
// File  : bank.cpp
// Proyek: Sistem Bank — OOP, Inheritance, Exception Handling
//
// Kompilasi:
//   g++ -std=c++17 -Wall -o bank bank.cpp
// Jalankan:
//   ./bank
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <ctime>
#include <map>
#include <functional>

// ============================================================
// Custom Exceptions
// ============================================================
struct BankException : std::runtime_error {
    explicit BankException(const std::string& msg) : std::runtime_error(msg) {}
};
struct SaldoKurang    : BankException { using BankException::BankException; };
struct AkunTidakAda   : BankException { using BankException::BankException; };
struct AkunTerkunci   : BankException { using BankException::BankException; };
struct LimitTerlampaui: BankException { using BankException::BankException; };

// ============================================================
// Utilitas
// ============================================================
std::string timestamp() {
    auto t = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return buf;
}

std::string formatUang(double jumlah) {
    std::ostringstream ss;
    ss << "Rp " << std::fixed << std::setprecision(2) << jumlah;
    // Format ribuan
    std::string s = ss.str();
    int insertPos = (int)s.find('.') - 3;
    while (insertPos > 3) {
        s.insert((size_t)insertPos, ".");
        insertPos -= 3;
    }
    return s;
}

// ============================================================
// Transaksi
// ============================================================
struct Transaksi {
    enum class Jenis { Setor, Tarik, Transfer, Bunga, Biaya };

    std::string waktu;
    Jenis       jenis;
    double      jumlah;
    double      saldoSetelah;
    std::string keterangan;

    static std::string namaJenis(Jenis j) {
        switch(j) {
        case Jenis::Setor:    return "SETOR   ";
        case Jenis::Tarik:    return "TARIK   ";
        case Jenis::Transfer: return "TRANSFER";
        case Jenis::Bunga:    return "BUNGA   ";
        case Jenis::Biaya:    return "BIAYA   ";
        }
        return "";
    }

    void cetak() const {
        std::cout << "  " << waktu << "  " << namaJenis(jenis) << "  "
                  << std::setw(16) << std::right << formatUang(jumlah) << "  "
                  << std::setw(18) << formatUang(saldoSetelah)
                  << "  " << keterangan << "\n";
    }
};

// ============================================================
// Kelas Dasar: Rekening
// ============================================================
class Rekening {
public:
    Rekening(const std::string& noRek, const std::string& namaPemilik, double saldoAwal)
        : nomorRekening(noRek), nama(namaPemilik), saldo(saldoAwal)
    {
        log({ timestamp(), Transaksi::Jenis::Setor, saldoAwal, saldoAwal, "Setoran awal" });
    }

    virtual ~Rekening() = default;

    // Accessor
    const std::string& getNomor() const { return nomorRekening; }
    const std::string& getNama()  const { return nama; }
    double             getSaldo() const { return saldo; }
    bool               isTerkunci() const { return terkunci; }

    // Setor
    virtual void setor(double jumlah, const std::string& ket = "Setoran tunai") {
        cekTerkunci();
        if (jumlah <= 0) throw BankException("Jumlah setor harus positif");
        saldo += jumlah;
        log({ timestamp(), Transaksi::Jenis::Setor, jumlah, saldo, ket });
        std::cout << "  ✓ Setor " << formatUang(jumlah) << " berhasil\n";
    }

    // Tarik
    virtual void tarik(double jumlah, const std::string& ket = "Penarikan tunai") {
        cekTerkunci();
        if (jumlah <= 0)     throw BankException("Jumlah tarik harus positif");
        if (jumlah > saldo)  throw SaldoKurang("Saldo tidak mencukupi. Saldo: " + formatUang(saldo));
        saldo -= jumlah;
        log({ timestamp(), Transaksi::Jenis::Tarik, jumlah, saldo, ket });
        std::cout << "  ✓ Tarik " << formatUang(jumlah) << " berhasil\n";
    }

    // Transfer ke rekening lain
    virtual void transfer(Rekening& tujuan, double jumlah) {
        std::string ket = "Transfer ke " + tujuan.getNomor() + " (" + tujuan.getNama() + ")";
        tarik(jumlah, ket);
        tujuan.setor(jumlah, "Transfer dari " + nomorRekening + " (" + nama + ")");
    }

    // Kunci/buka rekening
    void kunci(bool status = true) {
        terkunci = status;
        std::cout << "  Rekening " << (status ? "dikunci" : "dibuka kuncinya") << "\n";
    }

    // Cetak mutasi
    void cetakMutasi(int nBaris = 10) const {
        std::cout << "\n  ══════ Rekening Koran ══════\n";
        std::cout << "  Nomor : " << nomorRekening << "\n";
        std::cout << "  Nama  : " << nama << "\n";
        std::cout << "  Tipe  : " << tipeName() << "\n";
        std::cout << "  Saldo : " << formatUang(saldo) << "\n";
        std::cout << "  ────────────────────────────────────────────────────────\n";
        std::cout << "  Waktu               Jenis      Jumlah           Saldo\n";
        std::cout << "  ────────────────────────────────────────────────────────\n";
        int mulai = std::max(0, (int)riwayat.size() - nBaris);
        for (int i = mulai; i < (int)riwayat.size(); i++)
            riwayat[i].cetak();
        std::cout << "  ════════════════════════════════════════════════════════\n\n";
    }

    // Info singkat
    void info() const {
        std::cout << "  " << std::setw(14) << nomorRekening
                  << "  " << std::setw(20) << std::left << nama
                  << "  " << std::setw(12) << tipeName()
                  << "  " << std::setw(18) << std::right << formatUang(saldo)
                  << (terkunci ? "  [KUNCI]" : "") << "\n";
    }

    virtual std::string tipeName() const { return "Rekening"; }

    // Operasi bunga/biaya bulanan (override per tipe)
    virtual void prosesAkhirBulan() {}

protected:
    void cekTerkunci() const {
        if (terkunci) throw AkunTerkunci("Rekening " + nomorRekening + " terkunci");
    }

    void log(Transaksi t) { riwayat.push_back(std::move(t)); }

    std::string nomorRekening;
    std::string nama;
    double      saldo   = 0.0;
    bool        terkunci = false;
    std::vector<Transaksi> riwayat;
};

// ============================================================
// Rekening Tabungan
// ============================================================
class RekeningTabungan : public Rekening {
public:
    RekeningTabungan(const std::string& no, const std::string& nama,
                     double saldo, double bungaPerTahun = 0.035)
        : Rekening(no, nama, saldo), bungaTahunan(bungaPerTahun) {}

    std::string tipeName() const override { return "Tabungan"; }

    void prosesAkhirBulan() override {
        double bunga = saldo * (bungaTahunan / 12.0);
        saldo += bunga;
        log({ timestamp(), Transaksi::Jenis::Bunga, bunga, saldo,
              "Bunga " + std::to_string(bungaTahunan * 100) + "% p.a." });
        std::cout << "  Bunga tabungan " << getNomor() << ": " << formatUang(bunga) << "\n";
    }

private:
    double bungaTahunan = 0.035;  // 3.5% per tahun
};

// ============================================================
// Rekening Giro (bisa overdraft sampai limit)
// ============================================================
class RekeningGiro : public Rekening {
public:
    RekeningGiro(const std::string& no, const std::string& nama,
                 double saldo, double limitOverdraft = 5000000.0)
        : Rekening(no, nama, saldo), limitOD(limitOverdraft) {}

    std::string tipeName() const override { return "Giro"; }

    void tarik(double jumlah, const std::string& ket = "Penarikan giro") override {
        cekTerkunci();
        if (jumlah <= 0) throw BankException("Jumlah harus positif");
        if (jumlah > saldo + limitOD)
            throw LimitTerlampaui("Melebihi limit overdraft " + formatUang(limitOD));
        saldo -= jumlah;
        log({ timestamp(), Transaksi::Jenis::Tarik, jumlah, saldo, ket });
        if (saldo < 0)
            std::cout << "  ⚠ Overdraft aktif: " << formatUang(-saldo) << "\n";
    }

    void prosesAkhirBulan() override {
        // Biaya administrasi
        double biaya = 25000.0;
        saldo -= biaya;
        log({ timestamp(), Transaksi::Jenis::Biaya, biaya, saldo, "Biaya administrasi" });
        std::cout << "  Biaya admin giro " << getNomor() << ": " << formatUang(biaya) << "\n";

        // Bunga overdraft jika minus
        if (saldo < 0) {
            double bunga = (-saldo) * 0.02;  // 2% per bulan untuk overdraft
            saldo -= bunga;
            log({ timestamp(), Transaksi::Jenis::Bunga, bunga, saldo, "Bunga overdraft 2%/bulan" });
        }
    }

private:
    double limitOD;
};

// ============================================================
// Rekening Deposito (tidak bisa ditarik sebelum jatuh tempo)
// ============================================================
class RekeningDeposito : public Rekening {
public:
    RekeningDeposito(const std::string& no, const std::string& nama,
                     double modal, int bulan, double bungaBulanan = 0.006)
        : Rekening(no, nama, modal), tenorBulan(bulan),
          bungaPerBulan(bungaBulanan), bulanBerjalan(0) {}

    std::string tipeName() const override { return "Deposito"; }

    void tarik(double jumlah, const std::string&) override {
        if (bulanBerjalan < tenorBulan)
            throw BankException("Deposito belum jatuh tempo! Sisa: " +
                                std::to_string(tenorBulan - bulanBerjalan) + " bulan");
        Rekening::tarik(jumlah, "Pencairan deposito");
    }

    void prosesAkhirBulan() override {
        if (bulanBerjalan >= tenorBulan) return;
        double bunga = saldo * bungaPerBulan;
        saldo += bunga;
        bulanBerjalan++;
        log({ timestamp(), Transaksi::Jenis::Bunga, bunga, saldo,
              "Bunga deposito bulan ke-" + std::to_string(bulanBerjalan) });

        if (bulanBerjalan == tenorBulan)
            std::cout << "  🎉 Deposito " << getNomor() << " telah jatuh tempo!\n";
    }

    int sisaBulan() const { return tenorBulan - bulanBerjalan; }

private:
    int    tenorBulan;
    double bungaPerBulan;
    int    bulanBerjalan;
};

// ============================================================
// Bank (Facade untuk semua rekening)
// ============================================================
class Bank {
public:
    explicit Bank(const std::string& namaBank) : nama(namaBank) {}

    // Buka rekening
    RekeningTabungan* bukaTabungan(const std::string& noRek, const std::string& nasabah,
                                     double modal) {
        auto rek = std::make_unique<RekeningTabungan>(noRek, nasabah, modal);
        auto* ptr = rek.get();
        rekening[noRek] = std::move(rek);
        std::cout << "  ✓ Rekening tabungan " << noRek << " dibuka untuk " << nasabah << "\n";
        return ptr;
    }

    RekeningGiro* bukaGiro(const std::string& noRek, const std::string& nasabah,
                            double modal, double limitOD = 5000000.0) {
        auto rek = std::make_unique<RekeningGiro>(noRek, nasabah, modal, limitOD);
        auto* ptr = rek.get();
        rekening[noRek] = std::move(rek);
        std::cout << "  ✓ Rekening giro " << noRek << " dibuka untuk " << nasabah << "\n";
        return ptr;
    }

    RekeningDeposito* bukaDeposito(const std::string& noRek, const std::string& nasabah,
                                    double modal, int bulan) {
        auto rek = std::make_unique<RekeningDeposito>(noRek, nasabah, modal, bulan);
        auto* ptr = rek.get();
        rekening[noRek] = std::move(rek);
        std::cout << "  ✓ Deposito " << noRek << " (" << bulan << " bulan) dibuka\n";
        return ptr;
    }

    Rekening* cari(const std::string& noRek) {
        auto it = rekening.find(noRek);
        if (it == rekening.end()) throw AkunTidakAda("Rekening " + noRek + " tidak ditemukan");
        return it->second.get();
    }

    void tampilSemua() const {
        std::cout << "\n  ══ " << nama << " — Daftar Rekening ══\n";
        std::cout << "  " << std::setw(14) << "No. Rek"
                  << "  " << std::setw(20) << "Nama"
                  << "  " << std::setw(12) << "Tipe"
                  << "  " << std::setw(18) << "Saldo" << "\n";
        std::cout << "  " << std::string(70, '-') << "\n";
        for (const auto& [no, rek] : rekening)
            rek->info();
        std::cout << "\n";
    }

    void prosesAkhirBulan() {
        std::cout << "\n  ── Proses Akhir Bulan ──\n";
        for (auto& [_, rek] : rekening)
            rek->prosesAkhirBulan();
    }

    double totalAset() const {
        double total = 0;
        for (const auto& [_, rek] : rekening) total += rek->getSaldo();
        return total;
    }

private:
    std::string nama;
    std::map<std::string, std::unique_ptr<Rekening>> rekening;
};

// ============================================================
// Main — Demo Sistem Bank
// ============================================================
int main() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║   SIMULASI SISTEM BANK C++   v1.0       ║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n\n";

    Bank bank("Bank Belajar C++");

    // ── Buka Rekening ───────────────────────────────────────
    std::cout << "  === PEMBUKAAN REKENING ===\n";
    auto* tabBudi  = bank.bukaTabungan("3001-001", "Budi Santoso",   5'000'000.0);
    auto* tabSari  = bank.bukaTabungan("3001-002", "Sari Dewi",      2'500'000.0);
    auto* giroCV   = bank.bukaGiro ("4001-001", "CV Maju Jaya",    10'000'000.0, 5'000'000.0);
    auto* depBudi  = bank.bukaDeposito("5001-001", "Budi Santoso", 20'000'000.0, 3);

    bank.tampilSemua();

    // ── Transaksi ────────────────────────────────────────────
    std::cout << "  === TRANSAKSI ===\n";
    try {
        tabBudi->setor(1'500'000, "Gaji bulan ini");
        tabBudi->tarik(500'000,   "Belanja supermarket");
        tabSari->setor(3'000'000, "Transfer dari suami");
        tabBudi->transfer(*tabSari, 750'000);  // transfer antar rekening
        giroCV->setor(5'000'000,  "Pembayaran dari pelanggan");
        giroCV->tarik(12'000'000, "Bayar supplier");  // overdraft!

        // Coba tarik deposito sebelum jatuh tempo (akan error)
        try {
            depBudi->tarik(5'000'000, "Tarik darurat");
        } catch (const BankException& e) {
            std::cout << "  ✗ " << e.what() << "\n";
        }

    } catch (const BankException& e) {
        std::cout << "  ✗ Error: " << e.what() << "\n";
    }

    // ── Proses Akhir Bulan 3x ────────────────────────────────
    std::cout << "\n  === PROSES AKHIR BULAN (3 bulan) ===\n";
    for (int i = 1; i <= 3; i++) {
        std::cout << "\n  -- Bulan " << i << " --\n";
        bank.prosesAkhirBulan();
    }

    // ── Laporan ──────────────────────────────────────────────
    std::cout << "\n  === LAPORAN MUTASI ===\n";
    tabBudi->cetakMutasi(15);
    giroCV->cetakMutasi(10);

    // ── Ringkasan Bank ───────────────────────────────────────
    std::cout << "  === RINGKASAN BANK ===\n";
    bank.tampilSemua();
    std::cout << "  Total Aset Bank: " << formatUang(bank.totalAset()) << "\n\n";

    return 0;
}
