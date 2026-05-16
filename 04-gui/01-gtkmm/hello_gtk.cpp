// ============================================================
// File  : hello_gtk.cpp
// Topik : GUI dengan gtkmm (GTK3 binding C++)
// Kompilasi:
//   g++ -std=c++17 `pkg-config --cflags --libs gtkmm-3.0` -o hello_gtk hello_gtk.cpp
// ============================================================
// CATATAN: File ini memerlukan library gtkmm-3.0 terinstal.
//          Jalankan: sudo apt install libgtkmm-3.0-dev (Ubuntu/Debian)
//          Kode ini bisa dikompilasi setelah library diinstal.

#include <gtkmm.h>
#include <iostream>
#include <string>

// ====================================================
// Window 1: Hello World sederhana
// ====================================================
class JendelaHalo : public Gtk::Window {
    Gtk::Box       kotakV{Gtk::ORIENTATION_VERTICAL, 10};
    Gtk::Label     label;
    Gtk::Entry     entry;
    Gtk::Button    tombol{"Sapa!"};
    Gtk::Button    tombolKeluar{"Keluar"};
    Gtk::Separator separator;
    int hitungan = 0;

public:
    JendelaHalo() {
        set_title("Halo GTK! — Belajar C++");
        set_default_size(350, 180);
        set_border_width(15);

        // Styling teks label
        label.set_markup("<b>Selamat datang di GTK!</b>\nMasukkan nama Anda:");
        entry.set_placeholder_text("Nama Anda...");

        // Tombol Sapa: sambungkan ke lambda
        tombol.signal_clicked().connect([this]() {
            hitungan++;
            Glib::ustring nama = entry.get_text();
            if (nama.empty()) nama = "Dunia";
            label.set_markup("<b>Halo, " + nama + "!</b>\n"
                             "Anda sudah menyapa " + std::to_string(hitungan) + " kali.");
        });

        // Tombol Keluar
        tombolKeluar.signal_clicked().connect([this]() { hide(); });

        // Susun widget ke kotak
        kotakV.pack_start(label,        true,  true,  0);
        kotakV.pack_start(entry,        false, false, 0);
        kotakV.pack_start(tombol,       false, false, 0);
        kotakV.pack_start(separator,    false, false, 5);
        kotakV.pack_start(tombolKeluar, false, false, 0);

        add(kotakV);
        show_all_children();
    }
};

// ====================================================
// Window 2: Kalkulator sederhana
// ====================================================
class Kalkulator : public Gtk::Window {
    Gtk::Grid   grid;
    Gtk::Entry  display;
    double      nilaiA = 0;
    std::string op;
    bool        operasiAktif = false;

    void tekanAngka(const std::string& c) {
        if (operasiAktif) { display.set_text(""); operasiAktif = false; }
        std::string t = display.get_text();
        if (t == "0" && c != ".") t = "";
        display.set_text(t + c);
    }

    void tekanOperasi(const std::string& o) {
        nilaiA = std::stod(display.get_text().empty() ? "0" : display.get_text());
        op = o;
        operasiAktif = true;
    }

    void tekanSama() {
        if (op.empty()) return;
        double b = std::stod(display.get_text().empty() ? "0" : display.get_text());
        double hasil = 0;
        if      (op == "+") hasil = nilaiA + b;
        else if (op == "-") hasil = nilaiA - b;
        else if (op == "*") hasil = nilaiA * b;
        else if (op == "/") hasil = (b != 0) ? nilaiA / b : 0;
        std::string s = std::to_string(hasil);
        // Hapus trailing zero
        if (s.find('.') != std::string::npos) {
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            if (s.back() == '.') s.pop_back();
        }
        display.set_text(s);
        op = ""; operasiAktif = true;
    }

    Gtk::Button* buatTombol(const std::string& teks, int col, int row,
                              int colSpan = 1) {
        auto* btn = Gtk::manage(new Gtk::Button(teks));
        btn->set_hexpand(true); btn->set_vexpand(true);
        grid.attach(*btn, col, row, colSpan, 1);
        return btn;
    }

public:
    Kalkulator() {
        set_title("Kalkulator GTK");
        set_default_size(240, 300);
        set_border_width(10);

        display.set_text("0");
        display.set_editable(false);
        display.set_alignment(1.0);  // align kanan
        grid.attach(display, 0, 0, 4, 1);

        // Baris tombol
        const std::vector<std::vector<std::string>> tombol = {
            {"7","8","9","/"}, {"4","5","6","*"},
            {"1","2","3","-"}, {"0",".","=","+"}
        };
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                auto* btn = buatTombol(tombol[r][c], c, r+1);
                const std::string label = tombol[r][c];
                if (label == "=") btn->signal_clicked().connect([this](){ tekanSama(); });
                else if (label=="+" || label=="-" || label=="*" || label=="/")
                    btn->signal_clicked().connect([this,label](){ tekanOperasi(label); });
                else
                    btn->signal_clicked().connect([this,label](){ tekanAngka(label); });
            }
        }
        auto* btnC = buatTombol("C", 0, 5, 4);
        btnC->signal_clicked().connect([this](){
            display.set_text("0"); nilaiA=0; op=""; operasiAktif=false;
        });

        grid.set_row_spacing(4); grid.set_column_spacing(4);
        add(grid); show_all_children();
    }
};

// ====================================================
// Main: tampilkan dua jendela
// ====================================================
int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "id.cpp.belajar.gtk");

    // Buat dua jendela
    JendelaHalo jendela1;
    Kalkulator  jendela2;

    // Tampilkan keduanya
    jendela1.show();
    jendela2.show();

    return app->run(jendela1);
}
