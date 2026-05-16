// ============================================================
// File  : hello_wx.cpp
// Topik : GUI dengan wxWidgets
// Kompilasi:
//   g++ -std=c++17 `wx-config --cxxflags --libs` -o hello_wx hello_wx.cpp
// CATATAN: Butuh wxWidgets terinstal (libwxgtk3.0-gtk3-dev di Ubuntu)
// ============================================================
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>
#include <wx/clrpicker.h>
#include <wx/fontdlg.h>
#include <wx/grid.h>
#include <sstream>
#include <vector>

// ====================================================
// Tab 1: Sapa
// ====================================================
class TabSapa : public wxPanel {
    wxTextCtrl*   entryNama;
    wxStaticText* labelHasil;
    int hitungan = 0;
public:
    TabSapa(wxNotebook* parent) : wxPanel(parent) {
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        sizer->Add(new wxStaticText(this, wxID_ANY, "Masukkan nama Anda:"),
                   0, wxALL, 10);
        entryNama  = new wxTextCtrl(this, wxID_ANY, "",
                                    wxDefaultPosition, wxDefaultSize);
        sizer->Add(entryNama, 0, wxEXPAND | wxLR, 10);

        auto* btnSapa = new wxButton(this, wxID_ANY, "Sapa!");
        sizer->Add(btnSapa, 0, wxALL, 10);

        labelHasil = new wxStaticText(this, wxID_ANY, "Selamat datang!");
        wxFont font = labelHasil->GetFont();
        font.SetPointSize(14); font.SetWeight(wxFONTWEIGHT_BOLD);
        labelHasil->SetFont(font);
        sizer->Add(labelHasil, 0, wxALL | wxALIGN_CENTER, 10);

        SetSizer(sizer);

        btnSapa->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            hitungan++;
            wxString nama = entryNama->GetValue().Trim();
            if (nama.IsEmpty()) nama = "Dunia";
            labelHasil->SetLabel(wxString::Format("Halo, %s!\n(ke-%d)", nama, hitungan));
        });
        entryNama->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent& e) {
            wxCommandEvent dummy; dummy.SetString(e.GetString());
        });
    }
};

// ====================================================
// Tab 2: Kalkulator
// ====================================================
class TabKalkulator : public wxPanel {
    wxTextCtrl* display;
    double      nilaiA = 0;
    wxString    op;
    bool        inputBaru = true;

    void tekanAngka(const wxString& c) {
        if (inputBaru) { display->SetValue(""); inputBaru = false; }
        wxString t = display->GetValue();
        if (t == "0" && c != ".") t = "";
        display->SetValue(t + c);
    }
    void tekanOp(const wxString& o) {
        double v; display->GetValue().ToDouble(&v);
        nilaiA = v; op = o; inputBaru = true;
    }
    void tekanSama() {
        if (op.IsEmpty()) return;
        double b; display->GetValue().ToDouble(&b);
        double r = 0;
        if      (op == "+") r = nilaiA + b;
        else if (op == "-") r = nilaiA - b;
        else if (op == "*") r = nilaiA * b;
        else if (op == "/") r = b != 0 ? nilaiA / b : 0;
        display->SetValue(wxString::Format("%g", r));
        op = ""; inputBaru = true;
    }
public:
    TabKalkulator(wxNotebook* parent) : wxPanel(parent) {
        auto* mainSizer = new wxBoxSizer(wxVERTICAL);
        display = new wxTextCtrl(this, wxID_ANY, "0",
                                 wxDefaultPosition, wxSize(-1, 40),
                                 wxTE_RIGHT | wxTE_READONLY);
        wxFont font = display->GetFont(); font.SetPointSize(16);
        display->SetFont(font);
        mainSizer->Add(display, 0, wxEXPAND | wxALL, 5);

        auto* grid = new wxGridSizer(4, 4, 3, 3);
        const wxString labels[4][4] = {{"7","8","9","/"}, {"4","5","6","*"},
                                        {"1","2","3","-"}, {"0",".","=","+"}};
        for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) {
            wxString lbl = labels[r][c];
            auto* btn = new wxButton(this, wxID_ANY, lbl,
                                     wxDefaultPosition, wxSize(55, 45));
            grid->Add(btn, 0, wxEXPAND);
            if (lbl == "=")
                btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&){ tekanSama(); });
            else if (lbl=="+" || lbl=="-" || lbl=="*" || lbl=="/")
                btn->Bind(wxEVT_BUTTON, [this,lbl](wxCommandEvent&){ tekanOp(lbl); });
            else
                btn->Bind(wxEVT_BUTTON, [this,lbl](wxCommandEvent&){ tekanAngka(lbl); });
        }
        mainSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

        auto* btnC = new wxButton(this, wxID_ANY, "C");
        btnC->Bind(wxEVT_BUTTON, [this](wxCommandEvent&){
            display->SetValue("0"); nilaiA=0; op=""; inputBaru=true;
        });
        mainSizer->Add(btnC, 0, wxEXPAND | wxALL, 5);
        SetSizer(mainSizer);
    }
};

// ====================================================
// Tab 3: To-Do List
// ====================================================
class TabTodo : public wxPanel {
    wxListBox*  daftar;
    wxTextCtrl* entryTugas;
    wxStaticText* labelInfo;
public:
    TabTodo(wxNotebook* parent) : wxPanel(parent) {
        auto* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(new wxStaticText(this, wxID_ANY, "To-Do List:"), 0, wxALL, 5);
        daftar = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 200),
                               0, nullptr, wxLB_EXTENDED);
        for (const wxString& t : {"Belajar wxWidgets", "Buat aplikasi GUI", "Kuasai C++"})
            daftar->Append(t);
        sizer->Add(daftar, 1, wxEXPAND | wxALL, 5);

        auto* inputRow = new wxBoxSizer(wxHORIZONTAL);
        entryTugas = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                    wxTE_PROCESS_ENTER);
        auto* btnTambah = new wxButton(this, wxID_ANY, "+ Tambah");
        auto* btnHapus  = new wxButton(this, wxID_ANY, "Hapus");
        inputRow->Add(entryTugas, 1, wxALL, 3);
        inputRow->Add(btnTambah, 0, wxALL, 3);
        inputRow->Add(btnHapus,  0, wxALL, 3);
        sizer->Add(inputRow, 0, wxEXPAND);

        labelInfo = new wxStaticText(this, wxID_ANY, "");
        sizer->Add(labelInfo, 0, wxALL, 5);
        SetSizer(sizer);

        auto onTambah = [this](auto&) {
            wxString t = entryTugas->GetValue().Trim();
            if (!t.IsEmpty()) { daftar->Append(t); entryTugas->Clear(); update(); }
        };
        btnTambah->Bind(wxEVT_BUTTON, onTambah);
        entryTugas->Bind(wxEVT_TEXT_ENTER, onTambah);
        btnHapus->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            wxArrayInt sel; daftar->GetSelections(sel);
            for (int i = sel.size()-1; i >= 0; i--) daftar->Delete(sel[i]);
            update();
        });
        update();
    }
    void update() {
        labelInfo->SetLabel(wxString::Format("%d item", (int)daftar->GetCount()));
    }
};

// ====================================================
// Main Frame
// ====================================================
class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Belajar wxWidgets — C++ GUI",
                         wxDefaultPosition, wxSize(460, 420)) {
        auto* book = new wxNotebook(this, wxID_ANY);
        book->AddPage(new TabSapa(book),       "Sapa");
        book->AddPage(new TabKalkulator(book), "Kalkulator");
        book->AddPage(new TabTodo(book),       "To-Do");

        auto* menu = new wxMenuBar;
        auto* mFile = new wxMenu;
        mFile->Append(wxID_ABOUT, "&Tentang");
        mFile->AppendSeparator();
        mFile->Append(wxID_EXIT, "&Keluar\tAlt+F4");
        menu->Append(mFile, "&File");
        SetMenuBar(menu);

        Bind(wxEVT_MENU, [this](wxCommandEvent&){
            wxMessageBox("Belajar wxWidgets C++\nVersi 1.0", "Tentang",
                         wxOK | wxICON_INFORMATION, this);
        }, wxID_ABOUT);
        Bind(wxEVT_MENU, [this](wxCommandEvent&){ Close(true); }, wxID_EXIT);

        CreateStatusBar();
        SetStatusText("Siap");
    }
};

class MyApp : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};
wxIMPLEMENT_APP(MyApp);
