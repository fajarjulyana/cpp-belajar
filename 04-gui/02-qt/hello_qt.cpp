// ============================================================
// File  : hello_qt.cpp
// Topik : GUI dengan Qt (Qt5/Qt6)
// Kompilasi:
//   qmake -project && qmake && make
//   ATAU: cmake -B build && cmake --build build
// CATATAN: Butuh Qt5 atau Qt6 terinstal
// ============================================================
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include <QFont>

// ====================================================
// Widget: Aplikasi Sapa
// ====================================================
class WidgetSapa : public QWidget {
    Q_OBJECT
    QVBoxLayout* layout;
    QLabel*      labelJudul;
    QLineEdit*   entryNama;
    QPushButton* tombolSapa;
    QLabel*      labelHasil;
    int          hitungan = 0;
public:
    WidgetSapa(QWidget* parent = nullptr) : QWidget(parent) {
        layout     = new QVBoxLayout(this);
        labelJudul = new QLabel("Masukkan nama Anda:");
        entryNama  = new QLineEdit;
        tombolSapa = new QPushButton("Sapa!");
        labelHasil = new QLabel("Selamat datang!");

        entryNama->setPlaceholderText("Nama...");
        QFont font = labelHasil->font();
        font.setPointSize(14); font.setBold(true);
        labelHasil->setFont(font);
        labelHasil->setAlignment(Qt::AlignCenter);

        layout->addWidget(labelJudul);
        layout->addWidget(entryNama);
        layout->addWidget(tombolSapa);
        layout->addWidget(labelHasil);
        layout->addStretch();

        connect(tombolSapa, &QPushButton::clicked, this, &WidgetSapa::onSapa);
        connect(entryNama, &QLineEdit::returnPressed, this, &WidgetSapa::onSapa);
    }
private slots:
    void onSapa() {
        hitungan++;
        QString nama = entryNama->text().trimmed();
        if (nama.isEmpty()) nama = "Dunia";
        labelHasil->setText("Halo, " + nama + "!\n(sapaan ke-" + QString::number(hitungan) + ")");
    }
};

// ====================================================
// Widget: Kalkulator
// ====================================================
class WidgetKalkulator : public QWidget {
    Q_OBJECT
    QGridLayout* grid;
    QLineEdit*   display;
    double       nilaiA = 0;
    QString      opAktif;
    bool         inputBaru = true;

    void tekanAngka(const QString& c) {
        if (inputBaru) { display->setText(""); inputBaru = false; }
        QString t = display->text();
        if (t == "0" && c != ".") t = "";
        display->setText(t + c);
    }
    void tekanOp(const QString& op) {
        nilaiA = display->text().isEmpty() ? 0 : display->text().toDouble();
        opAktif = op; inputBaru = true;
    }
    void tekanSama() {
        if (opAktif.isEmpty()) return;
        double b = display->text().isEmpty() ? 0 : display->text().toDouble();
        double r = 0;
        if      (opAktif == "+") r = nilaiA + b;
        else if (opAktif == "-") r = nilaiA - b;
        else if (opAktif == "×") r = nilaiA * b;
        else if (opAktif == "÷") r = b != 0 ? nilaiA / b : 0;
        display->setText(QString::number(r, 'g', 12));
        opAktif = ""; inputBaru = true;
    }
public:
    WidgetKalkulator(QWidget* parent = nullptr) : QWidget(parent) {
        grid    = new QGridLayout(this);
        display = new QLineEdit("0");
        display->setReadOnly(true);
        display->setAlignment(Qt::AlignRight);
        QFont f = display->font(); f.setPointSize(18);
        display->setFont(f);
        grid->addWidget(display, 0, 0, 1, 4);

        const QVector<QVector<QString>> labels = {
            {"7","8","9","÷"}, {"4","5","6","×"},
            {"1","2","3","-"}, {"0",".","=","+"}
        };
        for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) {
            auto* btn = new QPushButton(labels[r][c]);
            btn->setMinimumHeight(50);
            grid->addWidget(btn, r+1, c);
            const QString lbl = labels[r][c];
            if (lbl == "=")
                connect(btn, &QPushButton::clicked, this, &WidgetKalkulator::tekanSama);
            else if (lbl=="+"||lbl=="-"||lbl=="×"||lbl=="÷")
                connect(btn, &QPushButton::clicked, [this,lbl](){ tekanOp(lbl); });
            else
                connect(btn, &QPushButton::clicked, [this,lbl](){ tekanAngka(lbl); });
        }
        auto* btnC = new QPushButton("C");
        btnC->setMinimumHeight(40);
        grid->addWidget(btnC, 5, 0, 1, 4);
        connect(btnC, &QPushButton::clicked, [this](){
            display->setText("0"); nilaiA=0; opAktif=""; inputBaru=true;
        });
    }
};

// ====================================================
// Widget: To-Do List
// ====================================================
class WidgetTodo : public QWidget {
    Q_OBJECT
    QVBoxLayout* layout;
    QListWidget* daftar;
    QLineEdit*   entryTugas;
    QHBoxLayout* inputRow;
    QPushButton* btnTambah;
    QPushButton* btnHapus;
    QLabel*      labelInfo;
public:
    WidgetTodo(QWidget* parent = nullptr) : QWidget(parent) {
        layout   = new QVBoxLayout(this);
        daftar   = new QListWidget;
        entryTugas = new QLineEdit;
        inputRow = new QHBoxLayout;
        btnTambah = new QPushButton("+ Tambah");
        btnHapus  = new QPushButton("🗑 Hapus");
        labelInfo = new QLabel("Tidak ada tugas");

        entryTugas->setPlaceholderText("Tugas baru...");
        daftar->setMinimumHeight(200);
        daftar->setSortingEnabled(false);

        inputRow->addWidget(entryTugas);
        inputRow->addWidget(btnTambah);
        inputRow->addWidget(btnHapus);

        layout->addWidget(new QLabel("<b>To-Do List</b>"));
        layout->addWidget(daftar);
        layout->addLayout(inputRow);
        layout->addWidget(labelInfo);

        connect(btnTambah, &QPushButton::clicked, this, &WidgetTodo::onTambah);
        connect(entryTugas, &QLineEdit::returnPressed, this, &WidgetTodo::onTambah);
        connect(btnHapus, &QPushButton::clicked, this, &WidgetTodo::onHapus);
        connect(daftar, &QListWidget::itemDoubleClicked, [](QListWidgetItem* item){
            if (item->checkState() == Qt::Checked) item->setCheckState(Qt::Unchecked);
            else item->setCheckState(Qt::Checked);
        });

        // Tambah beberapa contoh
        for (const QString& t : {"Belajar Qt", "Buat aplikasi GUI", "Kuasai C++"}) {
            auto* item = new QListWidgetItem(t, daftar);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
        updateLabel();
    }
private slots:
    void onTambah() {
        QString teks = entryTugas->text().trimmed();
        if (teks.isEmpty()) return;
        auto* item = new QListWidgetItem(teks, daftar);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        entryTugas->clear();
        updateLabel();
    }
    void onHapus() {
        for (auto* item : daftar->selectedItems()) delete item;
        updateLabel();
    }
    void updateLabel() {
        int total = daftar->count(), selesai = 0;
        for (int i = 0; i < total; i++)
            if (daftar->item(i)->checkState() == Qt::Checked) selesai++;
        if (total == 0) labelInfo->setText("Tidak ada tugas");
        else labelInfo->setText(QString("%1/%2 tugas selesai").arg(selesai).arg(total));
    }
};

// ====================================================
// Main Window dengan Tab
// ====================================================
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Belajar Qt — C++ GUI");
        resize(500, 420);

        // Tab widget sebagai central widget
        auto* tabs = new QTabWidget(this);
        tabs->addTab(new WidgetSapa,       "👋 Sapa");
        tabs->addTab(new WidgetKalkulator, "🔢 Kalkulator");
        tabs->addTab(new WidgetTodo,       "✅ To-Do");
        setCentralWidget(tabs);

        // Menu
        auto* menuFile = menuBar()->addMenu("&File");
        menuFile->addAction("&Tentang", [this](){
            QMessageBox::about(this, "Tentang",
                "Belajar Qt\nGUI C++ Framework\n© 2024");
        });
        menuFile->addSeparator();
        menuFile->addAction("&Keluar", this, &QMainWindow::close);

        statusBar()->showMessage("Siap");
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Belajar Qt");
    app.setOrganizationName("BelajarCpp");

    MainWindow w;
    w.show();
    return app.exec();
}

#include "hello_qt.moc"
