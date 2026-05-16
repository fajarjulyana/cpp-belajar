// ============================================================
// File  : hello_fltk.cpp
// Topik : GUI dengan FLTK (Fast Light Toolkit)
// Kompilasi:
//   g++ -std=c++17 `fltk-config --cxxflags --ldflags` -o hello_fltk hello_fltk.cpp
// CATATAN: Butuh FLTK terinstal (libfltk1.3-dev di Ubuntu)
// ============================================================
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <cmath>
#include <string>
#include <sstream>
using namespace std;

// ====================================================
// Widget kustom: kanvas gambar
// ====================================================
class KanvasLingkaran : public Fl_Widget {
    double sudut = 0;
    int    r     = 50;
    Fl_Color warna = FL_RED;
public:
    KanvasLingkaran(int X, int Y, int W, int H, const char* L = nullptr)
        : Fl_Widget(X, Y, W, H, L) {}

    void setSudut(double s)   { sudut = s; redraw(); }
    void setWarna(Fl_Color c) { warna = c; redraw(); }

    void draw() override {
        // Latar
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());
        fl_color(FL_BLACK);
        fl_rect(x(), y(), w(), h());

        // Titik pusat & jari-jari
        int cx = x() + w()/2;
        int cy = y() + h()/2;

        // Gambar sumbu
        fl_color(fl_rgb_color(200, 200, 200));
        fl_line(x(), cy, x()+w(), cy);
        fl_line(cx, y(), cx, y()+h());

        // Gambar lingkaran
        fl_color(warna);
        fl_circle(cx, cy, r);

        // Gambar titik pada sudut
        int px = cx + (int)(r * cos(sudut));
        int py = cy - (int)(r * sin(sudut));  // y terbalik di layar
        fl_color(FL_BLUE);
        fl_circle(px, py, 6);

        // Label sudut
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA, 12);
        string info = "sudut=" + to_string((int)(sudut * 180.0 / M_PI)) + "°";
        fl_draw(info.c_str(), x()+5, y()+15);
    }
};

// ====================================================
// Kalkulator FLTK
// ====================================================
struct DataKalk {
    Fl_Output* display;
    double     nilaiA;
    string     op;
    bool       inputBaru;
    DataKalk() : display(nullptr), nilaiA(0), inputBaru(true) {}
};

DataKalk kalkState;

void cbAngka(Fl_Widget* w, void* data) {
    const char* label = w->label();
    if (kalkState.inputBaru) {
        kalkState.display->value("");
        kalkState.inputBaru = false;
    }
    string t = kalkState.display->value();
    if (t == "0" && label[0] != '.') t = "";
    kalkState.display->value((t + label).c_str());
}

void cbOp(Fl_Widget* w, void* data) {
    string v = kalkState.display->value();
    kalkState.nilaiA  = v.empty() ? 0 : stod(v);
    kalkState.op      = w->label();
    kalkState.inputBaru = true;
}

void cbSama(Fl_Widget*, void*) {
    if (kalkState.op.empty()) return;
    string v = kalkState.display->value();
    double b = v.empty() ? 0 : stod(v);
    double r = 0;
    if (kalkState.op == "+") r = kalkState.nilaiA + b;
    else if (kalkState.op == "-") r = kalkState.nilaiA - b;
    else if (kalkState.op == "*") r = kalkState.nilaiA * b;
    else if (kalkState.op == "/") r = b != 0 ? kalkState.nilaiA / b : 0;
    ostringstream ss; ss << r;
    kalkState.display->value(ss.str().c_str());
    kalkState.op = ""; kalkState.inputBaru = true;
}

void cbBersih(Fl_Widget*, void*) {
    kalkState.display->value("0");
    kalkState.nilaiA = 0; kalkState.op = ""; kalkState.inputBaru = true;
}

// ====================================================
// Main: dua window
// ====================================================
KanvasLingkaran* kanvas;
Fl_Value_Slider* slider;

void cbSlider(Fl_Widget*, void*) {
    kanvas->setSudut(slider->value() * M_PI / 180.0);
}

int main() {
    // ---- Window 1: Sapa ----
    Fl_Window* win1 = new Fl_Window(320, 200, "Halo FLTK!");
    Fl_Input*  entry = new Fl_Input(100, 20, 200, 30, "Nama:");
    Fl_Output* output = new Fl_Output(100, 60, 200, 30, "Hasil:");
    Fl_Button* btnSapa = new Fl_Button(100, 100, 100, 30, "Sapa!");
    Fl_Button* btnKeluar = new Fl_Button(210, 100, 90, 30, "Keluar");
    btnSapa->callback([](Fl_Widget*, void* d) {
        auto* p = (pair<Fl_Input*, Fl_Output*>*)d;
        string nama = p->first->value();
        if (nama.empty()) nama = "Dunia";
        p->second->value(("Halo, " + nama + "!").c_str());
    }, new pair<Fl_Input*, Fl_Output*>(entry, output));
    btnKeluar->callback([](Fl_Widget* w, void*){ exit(0); });
    win1->end();

    // ---- Window 2: Kalkulator ----
    Fl_Window* win2 = new Fl_Window(240, 280, "Kalkulator FLTK");
    Fl_Output* disp = new Fl_Output(10, 10, 220, 35);
    kalkState.display = disp; disp->value("0");
    disp->textsize(18);

    const char* tombol[4][4] = {{"7","8","9","/"}, {"4","5","6","*"}, {"1","2","3","-"}, {"0",".","=","+"}};
    for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) {
        Fl_Button* b = new Fl_Button(10+c*55, 55+r*50, 50, 45, tombol[r][c]);
        string lbl = tombol[r][c];
        if (lbl=="=") b->callback(cbSama, nullptr);
        else if (lbl=="+"||lbl=="-"||lbl=="*"||lbl=="/") b->callback(cbOp, nullptr);
        else b->callback(cbAngka, nullptr);
    }
    Fl_Button* btnC = new Fl_Button(10, 255, 220, 20, "C");
    btnC->callback(cbBersih, nullptr);
    win2->end();

    // ---- Window 3: Canvas ----
    Fl_Window* win3 = new Fl_Window(320, 280, "Kanvas FLTK");
    kanvas  = new KanvasLingkaran(10, 10, 300, 200);
    slider  = new Fl_Value_Slider(10, 220, 300, 30, "Sudut (derajat)");
    slider->type(FL_HORIZONTAL); slider->range(0, 360); slider->value(0);
    slider->callback(cbSlider, nullptr);
    win3->end();

    // Tampilkan semua window
    win1->show(); win2->show(); win3->show();
    return Fl::run();
}
