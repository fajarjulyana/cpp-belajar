# GUI dengan FLTK

FLTK (Fast Light Toolkit) adalah toolkit GUI yang sangat ringan dan cepat, cocok untuk aplikasi embedded dan tool kecil.

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install libfltk1.3-dev fluid build-essential

# Fedora
sudo dnf install fltk-devel gcc-c++

# macOS
brew install fltk

# Windows
# Download dari: https://www.fltk.org/software.php
# Atau MSYS2: pacman -S mingw-w64-x86_64-fltk
```

---

## Kompilasi

```bash
# fltk-config otomatis menangani flags
g++ -std=c++17 `fltk-config --cxxflags --ldflags` -o hello_fltk hello_fltk.cpp

# Manual
g++ -std=c++17 -o hello_fltk hello_fltk.cpp \
    -lfltk -lfltk_images -lX11 -lm

# Windows/macOS — tidak perlu -lX11, cukup:
g++ -std=c++17 `fltk-config --cxxflags --ldflags` -o hello_fltk hello_fltk.cpp
```

---

## Hello World FLTK

```cpp
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>

// Callback untuk tombol
void onKlik(Fl_Widget* w, void* data) {
    Fl_Input*  input  = (Fl_Input*)((void**)data)[0];
    Fl_Output* output = (Fl_Output*)((void**)data)[1];
    std::string nama = input->value();
    if (nama.empty()) nama = "Dunia";
    output->value(("Halo, " + nama + "!").c_str());
}

int main() {
    Fl_Window* win = new Fl_Window(300, 150, "Halo FLTK!");

    Fl_Input*  input  = new Fl_Input(100, 20, 180, 30, "Nama:");
    Fl_Output* output = new Fl_Output(100, 60, 180, 30, "Hasil:");
    Fl_Button* btn    = new Fl_Button(100, 100, 180, 30, "Sapa!");

    void* data[2] = {input, output};
    btn->callback(onKlik, data);

    win->end();
    win->show();
    return Fl::run();
}
```

---

## Widget FLTK

| Widget | Kelas |
|--------|-------|
| Tombol | `Fl_Button`, `Fl_Check_Button`, `Fl_Radio_Button` |
| Input | `Fl_Input`, `Fl_Int_Input`, `Fl_Float_Input` |
| Output | `Fl_Output`, `Fl_Multiline_Output` |
| Slider | `Fl_Slider`, `Fl_Value_Slider` |
| Dropdown | `Fl_Choice`, `Fl_Menu_Button` |
| Teks | `Fl_Text_Editor`, `Fl_Text_Display` |
| Canvas | `Fl_Box`, custom draw dengan `draw()` |

---

## Custom Drawing

```cpp
class CanvasKu : public Fl_Widget {
public:
    CanvasKu(int x, int y, int w, int h) : Fl_Widget(x, y, w, h) {}
    void draw() override {
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());
        // Gambar lingkaran merah
        fl_color(FL_RED);
        fl_circle(x() + w()/2, y() + h()/2, 50);
        // Gambar teks
        fl_color(FL_BLACK);
        fl_draw("FLTK Canvas", x()+10, y()+20);
    }
};
```

---

## Latihan

1. Buat kalkulator body sederhana dengan FLTK
2. Buat viewer gambar dengan `Fl_Shared_Image`
3. Buat custom widget yang menggambar grafik sine wave

---

**[← Qt](../02-qt/README.md)** | **[Berikutnya → wxWidgets](../04-wxwidgets/README.md)**
