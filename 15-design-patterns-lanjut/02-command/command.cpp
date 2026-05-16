// ============================================================
// File  : command.cpp
// Topik : Command Pattern — Undo/Redo + Macro + Input Replay
//
// Kompilasi:
//   g++ -std=c++17 -o command command.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <map>
#include <chrono>

const char* RST  = "\033[0m";
const char* BOLD = "\033[1m";
const char* GRN  = "\033[32m";
const char* YLW  = "\033[33m";
const char* RED  = "\033[31m";
const char* BLU  = "\033[34m";
const char* CYN  = "\033[36m";

void judul(const std::string& s) {
    std::cout << "\n" << BOLD << BLU << "══ " << s << " ══" << RST << "\n";
}

// ============================================================
// State dunia yang dimanipulasi command
// ============================================================
struct Peta {
    int pemainX = 5, pemainY = 5;
    int hp = 100, skor = 0;
    std::vector<std::string> log;

    void tampilkan() const {
        std::cout << "  Posisi:(" << pemainX << "," << pemainY << ")"
                  << "  HP:" << hp
                  << "  Skor:" << skor << "\n";
    }

    void catat(const std::string& aksi) {
        log.push_back(aksi);
        std::cout << "  " << GRN << "▶ " << RST << aksi << "\n";
    }
};

// ============================================================
// Interface Command
// ============================================================
struct ICommand {
    virtual ~ICommand() = default;
    virtual void execute(Peta& peta) = 0;
    virtual void undo(Peta& peta)    = 0;
    virtual std::string nama() const = 0;
};

// ============================================================
// Concrete Commands
// ============================================================
struct PindahCommand : ICommand {
    int dx, dy;
    int oldX, oldY;

    PindahCommand(int dx, int dy) : dx(dx), dy(dy), oldX(0), oldY(0) {}

    void execute(Peta& p) override {
        oldX = p.pemainX;
        oldY = p.pemainY;
        p.pemainX += dx;
        p.pemainY += dy;
        p.catat("Pindah ke (" + std::to_string(p.pemainX) +
                "," + std::to_string(p.pemainY) + ")");
    }

    void undo(Peta& p) override {
        p.pemainX = oldX;
        p.pemainY = oldY;
        p.catat("UNDO Pindah → kembali ke (" +
                std::to_string(oldX) + "," + std::to_string(oldY) + ")");
    }

    std::string nama() const override {
        return "Pindah(" + std::to_string(dx) + "," + std::to_string(dy) + ")";
    }
};

struct SerangCommand : ICommand {
    int dmg;
    int oldHp;
    int poinSkor;

    SerangCommand(int dmg, int poin) : dmg(dmg), oldHp(0), poinSkor(poin) {}

    void execute(Peta& p) override {
        oldHp = p.hp;
        p.hp -= dmg;
        p.skor += poinSkor;
        p.catat("Serang → Musuh kena " + std::to_string(dmg) +
                " dmg, +skor " + std::to_string(poinSkor));
    }

    void undo(Peta& p) override {
        p.hp    = oldHp;
        p.skor -= poinSkor;
        p.catat("UNDO Serang → HP=" + std::to_string(oldHp) +
                ", Skor-=" + std::to_string(poinSkor));
    }

    std::string nama() const override {
        return "Serang(dmg=" + std::to_string(dmg) + ")";
    }
};

struct ItemCommand : ICommand {
    std::string namaItem;
    int hpPulih;
    int oldHp;

    ItemCommand(const std::string& item, int pulih)
        : namaItem(item), hpPulih(pulih), oldHp(0) {}

    void execute(Peta& p) override {
        oldHp = p.hp;
        p.hp = std::min(100, p.hp + hpPulih);
        p.catat("Pakai item '" + namaItem + "' → HP " +
                std::to_string(oldHp) + " → " + std::to_string(p.hp));
    }

    void undo(Peta& p) override {
        p.hp = oldHp;
        p.catat("UNDO Pakai '" + namaItem + "' → HP kembali ke " +
                std::to_string(oldHp));
    }

    std::string nama() const override {
        return "Item(" + namaItem + ")";
    }
};

// ============================================================
// Command History — Undo / Redo Stack
// ============================================================
class CommandHistory {
    std::vector<std::unique_ptr<ICommand>> undoStack_;
    std::vector<std::unique_ptr<ICommand>> redoStack_;
    Peta& peta_;

public:
    explicit CommandHistory(Peta& p) : peta_(p) {}

    void execute(std::unique_ptr<ICommand> cmd) {
        cmd->execute(peta_);
        undoStack_.push_back(std::move(cmd));
        redoStack_.clear();  // redo hangus setelah aksi baru
    }

    bool undo() {
        if (undoStack_.empty()) {
            std::cout << "  " << RED << "[!] Tidak ada yang bisa di-undo\n" << RST;
            return false;
        }
        auto& cmd = undoStack_.back();
        std::cout << "  " << YLW << "↩ UNDO: " << cmd->nama() << RST << "\n";
        cmd->undo(peta_);
        redoStack_.push_back(std::move(cmd));
        undoStack_.pop_back();
        return true;
    }

    bool redo() {
        if (redoStack_.empty()) {
            std::cout << "  " << RED << "[!] Tidak ada yang bisa di-redo\n" << RST;
            return false;
        }
        auto& cmd = redoStack_.back();
        std::cout << "  " << CYN << "↪ REDO: " << cmd->nama() << RST << "\n";
        cmd->execute(peta_);
        undoStack_.push_back(std::move(cmd));
        redoStack_.pop_back();
        return true;
    }

    void tampilkanStack() const {
        std::cout << "  Undo stack (" << undoStack_.size() << "): ";
        for (auto& c : undoStack_) std::cout << "[" << c->nama() << "] ";
        std::cout << "\n";
        std::cout << "  Redo stack (" << redoStack_.size() << "): ";
        for (auto& c : redoStack_) std::cout << "[" << c->nama() << "] ";
        std::cout << "\n";
    }
};

void demoUndoRedo() {
    judul("Demo 1: Undo / Redo Stack");

    Peta peta;
    CommandHistory hist(peta);

    std::cout << "  State awal: ";
    peta.tampilkan();

    hist.execute(std::make_unique<PindahCommand>(2, 0));
    hist.execute(std::make_unique<PindahCommand>(0, -3));
    hist.execute(std::make_unique<SerangCommand>(20, 50));
    hist.execute(std::make_unique<ItemCommand>("Potion Kecil", 30));

    std::cout << "\n  State setelah 4 aksi: ";
    peta.tampilkan();
    hist.tampilkanStack();

    std::cout << "\n";
    hist.undo();   // batal pakai potion
    hist.undo();   // batal serang
    std::cout << "\n  Setelah 2x undo: ";
    peta.tampilkan();
    hist.tampilkanStack();

    std::cout << "\n";
    hist.redo();   // redo serang
    std::cout << "\n  Setelah 1x redo: ";
    peta.tampilkan();

    // Aksi baru → redo stack hangus
    std::cout << "\n";
    hist.execute(std::make_unique<PindahCommand>(-1, 1));
    std::cout << "\n  Redo sekarang:\n";
    hist.redo();  // tidak ada yang bisa di-redo
}

// ============================================================
// VARIASI 2: Macro Command — rekam & putar ulang serangkaian aksi
// ============================================================
class MacroCommand : public ICommand {
    std::vector<std::unique_ptr<ICommand>> cmds_;
    std::string nama_;
public:
    MacroCommand(const std::string& nama) : nama_(nama) {}

    void tambah(std::unique_ptr<ICommand> cmd) {
        cmds_.push_back(std::move(cmd));
    }

    void execute(Peta& p) override {
        p.catat("── Macro '" + nama_ + "' mulai ──");
        for (auto& c : cmds_) c->execute(p);
        p.catat("── Macro '" + nama_ + "' selesai ──");
    }

    void undo(Peta& p) override {
        p.catat("── UNDO Macro '" + nama_ + "' ──");
        for (auto it = cmds_.rbegin(); it != cmds_.rend(); ++it)
            (*it)->undo(p);
    }

    std::string nama() const override { return "Macro(" + nama_ + ")"; }
};

void demoMacro() {
    judul("Demo 2: Macro Command (Rekam & Putar Ulang)");

    Peta peta;
    CommandHistory hist(peta);

    // Buat macro "Combo Serangan"
    auto combo = std::make_unique<MacroCommand>("Combo Serangan");
    combo->tambah(std::make_unique<PindahCommand>(1, 0));
    combo->tambah(std::make_unique<SerangCommand>(15, 30));
    combo->tambah(std::make_unique<SerangCommand>(10, 20));
    combo->tambah(std::make_unique<PindahCommand>(-1, 0));

    std::cout << "  State awal: ";
    peta.tampilkan();

    hist.execute(std::move(combo));
    std::cout << "\n  Setelah combo: ";
    peta.tampilkan();

    std::cout << "\n";
    hist.undo();  // undo seluruh macro sekaligus
    std::cout << "\n  Setelah undo combo: ";
    peta.tampilkan();
}

// ============================================================
// VARIASI 3: Input Replay — rekam urutan input, putar ulang
// ============================================================
struct InputRecord {
    long long timestamp;
    std::string cmdNama;
    std::unique_ptr<ICommand> cmd;
};

class InputRecorder {
    std::vector<std::pair<long long, std::string>> replay_;
    bool merekam_ = false;
    long long startTime_ = 0;

public:
    void mulaiRekam() {
        replay_.clear();
        merekam_ = true;
        startTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        std::cout << "  " << RED << "● REC" << RST << " Rekaman dimulai\n";
    }

    void rekam(const std::string& nama) {
        if (!merekam_) return;
        long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        replay_.push_back({now - startTime_, nama});
    }

    void berhentiRekam() {
        merekam_ = false;
        std::cout << "  " << RED << "■ STOP" << RST
                  << " Rekaman selesai (" << replay_.size() << " aksi)\n";
    }

    void putarUlang() const {
        std::cout << "  " << GRN << "▶ REPLAY" << RST
                  << " Memutar " << replay_.size() << " aksi:\n";
        for (auto& [ts, nama] : replay_)
            std::cout << "    t+" << std::setw(4) << ts << "ms → " << nama << "\n";
    }
};

void demoReplay() {
    judul("Demo 3: Input Recording & Replay");

    InputRecorder rec;
    Peta peta;
    CommandHistory hist(peta);

    auto jalankan = [&](std::unique_ptr<ICommand> cmd) {
        rec.rekam(cmd->nama());
        hist.execute(std::move(cmd));
    };

    rec.mulaiRekam();
    jalankan(std::make_unique<PindahCommand>(3, 0));
    jalankan(std::make_unique<PindahCommand>(0, 2));
    jalankan(std::make_unique<SerangCommand>(25, 60));
    jalankan(std::make_unique<ItemCommand>("Elixir", 50));
    jalankan(std::make_unique<PindahCommand>(-1, -1));
    rec.berhentiRekam();

    std::cout << "\n  State akhir: ";
    peta.tampilkan();

    std::cout << "\n";
    rec.putarUlang();
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n" << BOLD
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Command Pattern — Undo/Redo + Macro + Replay║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << RST;

    demoUndoRedo();
    demoMacro();
    demoReplay();

    std::cout << "\n" << BOLD
              << "─────────────────────────────────────────────\n"
              << " Ringkasan:\n"
              << "  Undo/Redo → simpan state sebelum di stack\n"
              << "  Macro     → kumpulan command jadi satu unit\n"
              << "  Replay    → rekam timestamp + nama command\n"
              << "─────────────────────────────────────────────\n"
              << RST << "\n";
    return 0;
}
