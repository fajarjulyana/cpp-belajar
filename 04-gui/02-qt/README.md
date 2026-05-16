# GUI dengan Qt6/Qt5

Qt adalah framework GUI lintas platform paling populer untuk C++. Mendukung Windows, Linux, macOS, Android, iOS, dan embedded systems.

---

## Instalasi

```bash
# Ubuntu/Debian (Qt5)
sudo apt install qt5-default qtbase5-dev build-essential

# Ubuntu/Debian (Qt6)
sudo apt install qt6-base-dev build-essential cmake

# Fedora
sudo dnf install qt6-qtbase-devel cmake gcc-c++

# macOS
brew install qt@6

# Windows
# Download Qt Installer: https://www.qt.io/download-open-source
# Atau MSYS2: pacman -S mingw-w64-x86_64-qt6-base
```

---

## Kompilasi dengan qmake

```bash
# Buat file .pro
qmake -project          # buat .pro otomatis dari file di folder
qmake hello_qt.pro      # buat Makefile dari .pro
make                    # kompilasi

# File .pro contoh:
# QT += core gui widgets
# CONFIG += c++17
# TARGET = hello_qt
# SOURCES += main.cpp mainwindow.cpp
# HEADERS += mainwindow.h
```

## Kompilasi dengan CMake (direkomendasikan untuk Qt6)

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt
cmake --build build
```

---

## Konsep Qt

### Signals & Slots — Mekanisme Event Qt
```cpp
// Signal: dikirim saat sesuatu terjadi
// Slot  : fungsi yang dipanggil saat signal diterima

// Sambungkan signal ke slot
connect(tombol, &QPushButton::clicked, this, &MainWindow::onKlik);
connect(tombol, &QPushButton::clicked, [](){ qDebug() << "Diklik!"; });

// Emit signal sendiri
class Timer : public QObject {
    Q_OBJECT
signals:
    void tick(int detik);
public:
    void mulai() { emit tick(0); }  // kirim signal
};
```

### Meta-Object System
```cpp
// Q_OBJECT makro wajib untuk class yang menggunakan signal/slot
class Window : public QWidget {
    Q_OBJECT
public:
    explicit Window(QWidget* parent = nullptr);
};
```

---

## Widget Dasar

```cpp
// Label
QLabel* label = new QLabel("Teks label", this);
label->setAlignment(Qt::AlignCenter);

// Button
QPushButton* btn = new QPushButton("Klik Saya!", this);
connect(btn, &QPushButton::clicked, this, &MyClass::handler);

// Line Edit
QLineEdit* input = new QLineEdit(this);
input->setPlaceholderText("Masukkan teks...");
QString teks = input->text();

// Layout
QVBoxLayout* layout = new QVBoxLayout;
layout->addWidget(label);
layout->addWidget(input);
layout->addWidget(btn);
setLayout(layout);
```

---

## Contoh Minimal (Satu File)

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Halo Qt!");
    window.resize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(&window);
    QLabel*      label  = new QLabel("Masukkan nama:");
    QLineEdit*   entry  = new QLineEdit;
    QPushButton* btn    = new QPushButton("Sapa!");

    layout->addWidget(label);
    layout->addWidget(entry);
    layout->addWidget(btn);

    QObject::connect(btn, &QPushButton::clicked, [&]() {
        QString nama = entry->text();
        if (nama.isEmpty()) nama = "Dunia";
        label->setText("Halo, " + nama + "!");
    });

    window.show();
    return app.exec();
}
```

---

## Widget Lanjutan

| Widget | Deskripsi |
|--------|-----------|
| `QMainWindow` | Window utama dengan toolbar & menu |
| `QDialog` | Dialog modal/non-modal |
| `QTableWidget` | Tabel |
| `QTreeWidget` | Tree view |
| `QListWidget` | Daftar |
| `QTabWidget` | Tab view |
| `QScrollArea` | Area gulir |
| `QStackedWidget` | Multi-halaman |
| `QGraphicsView` | Grafik 2D |
| `QOpenGLWidget` | OpenGL rendering |
| `QWebEngineView` | Browser web (Qt WebEngine) |

---

## Qt Resource System

```cpp
// File resources.qrc
// <RCC>
//   <qresource>
//     <file>icons/logo.png</file>
//   </qresource>
// </RCC>

// Pakai di kode
QIcon icon(":/icons/logo.png");
QPixmap pix(":/icons/logo.png");
```

---

## Latihan

1. Buat aplikasi to-do list dengan `QListWidget` dan tombol tambah/hapus
2. Buat file browser sederhana menggunakan `QTreeView` dan `QFileSystemModel`
3. Buat image viewer dengan `QLabel`, `QScrollArea`, dan `QFileDialog`
4. Buat chat UI sederhana dengan `QTextEdit` dan `QLineEdit`

---

**[← GTK](../01-gtkmm/README.md)** | **[Berikutnya → FLTK](../03-fltk/README.md)**
