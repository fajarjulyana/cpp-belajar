# GUI dengan GTK3/gtkmm

gtkmm adalah binding C++ resmi untuk GTK, toolkit GUI lintas platform (Linux, Windows, macOS).

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install libgtkmm-3.0-dev build-essential

# Fedora/RHEL
sudo dnf install gtkmm30-devel gcc-c++

# Arch Linux
sudo pacman -S gtkmm3

# macOS (Homebrew)
brew install gtkmm3

# Windows
# Gunakan MSYS2: pacman -S mingw-w64-x86_64-gtkmm3
```

---

## Kompilasi

```bash
# pkg-config otomatis menangani include path & linker flags
g++ -std=c++17 `pkg-config --cflags --libs gtkmm-3.0` -o hello_gtk hello_gtk.cpp

# Atau dengan Makefile
PKG = gtkmm-3.0
CXXFLAGS += $(shell pkg-config --cflags $(PKG))
LDFLAGS  += $(shell pkg-config --libs $(PKG))
```

---

## Konsep Utama

### Widget Hierarchy
```
Gtk::Window
└── Gtk::Box (vertical)
    ├── Gtk::Label
    ├── Gtk::Entry
    └── Gtk::Button
```

### Signal & Slot
```cpp
// Sambungkan signal ke handler
button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::onKlik));

// Lambda
button.signal_clicked().connect([]() { cout << "Diklik!\n"; });
```

---

## Contoh Aplikasi Hello World

```cpp
#include <gtkmm.h>

class JendelaUtama : public Gtk::Window {
    Gtk::Box   kotakV{Gtk::ORIENTATION_VERTICAL, 10};
    Gtk::Label labelSalam;
    Gtk::Entry entryNama;
    Gtk::Button tombolSalam{"Sapa!"};
public:
    JendelaUtama() {
        set_title("Halo GTK!");
        set_default_size(300, 150);
        set_border_width(15);

        labelSalam.set_text("Masukkan nama Anda:");
        entryNama.set_placeholder_text("Nama...");

        tombolSalam.signal_clicked().connect([this]() {
            string nama = entryNama.get_text();
            if (nama.empty()) nama = "Dunia";
            labelSalam.set_text("Halo, " + nama + "!");
        });

        kotakV.pack_start(labelSalam);
        kotakV.pack_start(entryNama);
        kotakV.pack_start(tombolSalam);
        add(kotakV);
        show_all_children();
    }
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "com.contoh.halo");
    JendelaUtama jendela;
    return app->run(jendela);
}
```

---

## Widget-Widget Penting

| Widget | Deskripsi |
|--------|-----------|
| `Gtk::Label` | Teks statis |
| `Gtk::Button` | Tombol |
| `Gtk::Entry` | Input teks satu baris |
| `Gtk::TextView` | Editor teks multi-baris |
| `Gtk::Box` | Layout container (vertikal/horizontal) |
| `Gtk::Grid` | Layout grid |
| `Gtk::TreeView` | Tabel/daftar |
| `Gtk::ComboBox` | Dropdown |
| `Gtk::CheckButton` | Checkbox |
| `Gtk::RadioButton` | Radio button |
| `Gtk::SpinButton` | Input angka |
| `Gtk::Scale` | Slider |
| `Gtk::ProgressBar` | Progress bar |
| `Gtk::Dialog` | Dialog popup |
| `Gtk::FileChooserDialog` | Pilih file |
| `Gtk::DrawingArea` | Area gambar kustom (Canvas) |
| `Gtk::Notebook` | Tab view |
| `Gtk::Toolbar` | Toolbar |
| `Gtk::MenuBar` | Menu bar |

---

## Layout Managers

```cpp
// Gtk::Box — susun horizontal atau vertical
Gtk::Box box(Gtk::ORIENTATION_HORIZONTAL, 5);  // spacing 5px
box.pack_start(widget1, true, true, 0);  // expand=true, fill=true
box.pack_end(widget2, false, false, 0);  // di kanan/bawah

// Gtk::Grid — susunan baris dan kolom
Gtk::Grid grid;
grid.attach(widget, col, row, col_span, row_span);
grid.set_row_spacing(5);
grid.set_column_spacing(5);
```

---

## CSS Styling

```cpp
// Terapkan CSS ke widget
auto css = Gtk::CssProvider::create();
css->load_from_data("button { background-color: #3498db; color: white; }");
widget.get_style_context()->add_provider(css, GTK_STYLE_PROVIDER_PRIORITY_USER);
```

---

## Latihan

1. Buat aplikasi kalkulator dengan tombol 0-9, +, -, *, /
2. Buat editor teks sederhana dengan menu File (Buka, Simpan, Keluar)
3. Buat aplikasi to-do list dengan `Gtk::TreeView`
4. Buat paint app sederhana dengan `Gtk::DrawingArea`

---

**[← Optimasi](../../03-ahli/04-optimasi/README.md)** | **[Berikutnya → Qt](../02-qt/README.md)**
