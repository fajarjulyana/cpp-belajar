# GUI dengan wxWidgets

wxWidgets menggunakan widget **native** di setiap platform — tampilan Windows di Windows, macOS di Mac, GTK di Linux.

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install libwxgtk3.0-gtk3-dev build-essential

# Fedora
sudo dnf install wxGTK3-devel gcc-c++

# macOS
brew install wxwidgets

# Windows
# Download dari: https://www.wxwidgets.org/downloads/
# Atau MSYS2: pacman -S mingw-w64-x86_64-wxwidgets3.2-msw
```

---

## Kompilasi

```bash
# wx-config otomatis menangani flags
g++ -std=c++17 `wx-config --cxxflags --libs` -o hello_wx hello_wx.cpp

# Cek versi
wx-config --version
```

---

## Struktur Dasar wxWidgets

```cpp
#include <wx/wx.h>

// 1. Class App — entry point
class MyApp : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new MyFrame("Judul", wxPoint(50, 50), wxSize(400, 300));
        frame->Show(true);
        return true;
    }
};

// 2. Makro pengganti main()
wxIMPLEMENT_APP(MyApp);

// 3. Class Frame — jendela utama
class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(nullptr, wxID_ANY, title, pos, size) {
        // ... setup widget ...
    }
};
```

---

## Widget & Layout

```cpp
// Panel (container biasa)
auto* panel = new wxPanel(this, wxID_ANY);

// Layout Sizer
auto* sizer = new wxBoxSizer(wxVERTICAL);
sizer->Add(new wxStaticText(panel, wxID_ANY, "Nama:"), 0, wxALL, 5);
sizer->Add(new wxTextCtrl(panel, wxID_ANY), 1, wxEXPAND | wxALL, 5);
sizer->Add(new wxButton(panel, wxID_ANY, "OK"), 0, wxALIGN_RIGHT | wxALL, 5);
panel->SetSizer(sizer);

// Grid sizer
auto* gridSizer = new wxGridSizer(3, 3, 5, 5);  // rows, cols, vgap, hgap
```

---

## Event Handling

```cpp
// Cara 1: Event table (macro)
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(BTN_OK, MyFrame::OnOk)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_CLOSE(MyFrame::OnClose)
END_EVENT_TABLE()

// Cara 2: Bind (modern, direkomendasikan)
btn->Bind(wxEVT_BUTTON, [](wxCommandEvent&) {
    wxMessageBox("Diklik!", "Info");
});

txtInput->Bind(wxEVT_TEXT, [this](wxCommandEvent& evt) {
    labelHasil->SetLabel("Anda ketik: " + evt.GetString());
});
```

---

## Dialog Bawaan

```cpp
// Message box
wxMessageBox("Pesan ini!", "Judul", wxOK | wxICON_INFORMATION);

// Konfirmasi
int pilihan = wxMessageBox("Yakin?", "Konfirmasi", wxYES_NO | wxICON_QUESTION);
if (pilihan == wxYES) { /* ya */ }

// Input teks
wxTextEntryDialog dlg(this, "Masukkan nama:", "Input");
if (dlg.ShowModal() == wxID_OK) {
    wxString nama = dlg.GetValue();
}

// File picker
wxFileDialog openDlg(this, "Buka file", "", "", "All files (*.*)|*.*",
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);
if (openDlg.ShowModal() == wxID_OK) {
    wxString path = openDlg.GetPath();
}

// Colour picker
wxColourDialog colorDlg(this);
if (colorDlg.ShowModal() == wxID_OK) {
    wxColour color = colorDlg.GetColourData().GetColour();
}
```

---

## Latihan

1. Buat aplikasi notepad dengan `wxTextCtrl` multiline, menu File (Buka/Simpan/Keluar)
2. Buat aplikasi to-do list dengan `wxListBox`
3. Buat dialog pengaturan dengan `wxNotebook` (tab)
4. Buat aplikasi kalkulator menggunakan `wxGridSizer`

---

**[← FLTK](../03-fltk/README.md)** | **[Berikutnya → Win32 API](../05-win32api/README.md)**
