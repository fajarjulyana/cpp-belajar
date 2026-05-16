// ============================================================
// File  : hello_win32.cpp
// Topik : GUI dengan Win32 API
// Kompilasi (MinGW/Windows):
//   g++ -std=c++17 -mwindows -o hello_win32.exe hello_win32.cpp -lgdi32 -lcomctl32
// CATATAN: File ini khusus Windows. Tidak bisa dikompilasi di Linux/macOS
//          tanpa cross-compiler (x86_64-w64-mingw32-g++)
// ============================================================
#ifdef _WIN32

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <sstream>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdi32.lib")

// ID kontrol
#define ID_BTN_SAPA     101
#define ID_BTN_KELUAR   102
#define ID_EDIT_NAMA    103
#define ID_LABEL_HASIL  104
#define ID_BTN_KALK     200
#define ID_BTN_CLEAR    201
#define ID_EDIT_DISPLAY 202

static HINSTANCE g_hInst;
static int       g_hitungan = 0;

// ====================================================
// Helper: buat control
// ====================================================
HWND BuatKontrol(const wchar_t* kelas, const wchar_t* teks, DWORD style,
                  int x, int y, int w, int h, HWND parent, HMENU id) {
    return CreateWindowW(kelas, teks, WS_VISIBLE | WS_CHILD | style,
                         x, y, w, h, parent, id, g_hInst, nullptr);
}

// ====================================================
// Window Procedure: Jendela Sapa
// ====================================================
HWND g_hEditNama, g_hLabelHasil;

LRESULT CALLBACK SapaProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_hInst = ((CREATESTRUCT*)lParam)->hInstance;
        BuatKontrol(L"STATIC", L"Masukkan nama Anda:",
                    0, 10, 10, 250, 20, hwnd, nullptr);
        g_hEditNama = BuatKontrol(L"EDIT", L"",
                    WS_BORDER | ES_AUTOHSCROLL, 10, 35, 250, 25,
                    hwnd, (HMENU)ID_EDIT_NAMA);
        BuatKontrol(L"BUTTON", L"Sapa!",
                    BS_PUSHBUTTON, 10, 70, 120, 30, hwnd, (HMENU)ID_BTN_SAPA);
        BuatKontrol(L"BUTTON", L"Keluar",
                    BS_PUSHBUTTON, 140, 70, 120, 30, hwnd, (HMENU)ID_BTN_KELUAR);
        g_hLabelHasil = BuatKontrol(L"STATIC", L"Selamat datang!",
                    SS_CENTER, 10, 115, 250, 30, hwnd, (HMENU)ID_LABEL_HASIL);
        // Font lebih besar untuk label
        HFONT hFont = CreateFontW(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
        SendMessageW(g_hLabelHasil, WM_SETFONT, (WPARAM)hFont, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BTN_SAPA) {
            g_hitungan++;
            wchar_t buf[256] = {};
            GetWindowTextW(g_hEditNama, buf, 256);
            std::wstring nama = buf;
            if (nama.empty()) nama = L"Dunia";
            std::wstring hasil = L"Halo, " + nama + L"! (ke-" +
                                  std::to_wstring(g_hitungan) + L")";
            SetWindowTextW(g_hLabelHasil, hasil.c_str());
        } else if (LOWORD(wParam) == ID_BTN_KELUAR) {
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Gambar garis dekoratif
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(52, 152, 219));
        SelectObject(hdc, hPen);
        MoveToEx(hdc, 0, 160, nullptr);
        LineTo(hdc, 280, 160);
        DeleteObject(hPen);
        SetTextColor(hdc, RGB(100, 100, 100));
        SetBkMode(hdc, TRANSPARENT);
        TextOutW(hdc, 10, 170, L"Dibuat dengan Win32 API", 23);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ====================================================
// GDI Drawing Window
// ====================================================
LRESULT CALLBACK GDIProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);

        // Latar
        HBRUSH hBrushBg = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(hdc, &rc, hBrushBg);
        DeleteObject(hBrushBg);

        // Judul
        SetTextColor(hdc, RGB(50, 50, 200));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
        SelectObject(hdc, hFont);
        TextOutW(hdc, 10, 10, L"Contoh GDI Drawing", 18);
        DeleteObject(hFont);

        // Persegi panjang merah
        HPEN   hPen   = CreatePen(PS_SOLID, 2, RGB(200, 0, 0));
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 180, 180));
        SelectObject(hdc, hPen); SelectObject(hdc, hBrush);
        Rectangle(hdc, 20, 40, 120, 90);
        DeleteObject(hPen); DeleteObject(hBrush);
        SetTextColor(hdc, RGB(150, 0, 0));
        TextOutW(hdc, 25, 55, L"Persegi", 7);

        // Lingkaran biru
        hPen   = CreatePen(PS_SOLID, 2, RGB(0, 0, 200));
        hBrush = CreateSolidBrush(RGB(180, 180, 255));
        SelectObject(hdc, hPen); SelectObject(hdc, hBrush);
        Ellipse(hdc, 140, 40, 240, 90);
        DeleteObject(hPen); DeleteObject(hBrush);
        SetTextColor(hdc, RGB(0, 0, 150));
        TextOutW(hdc, 165, 55, L"Elips", 5);

        // Garis
        hPen = CreatePen(PS_DASH, 1, RGB(0, 150, 0));
        SelectObject(hdc, hPen);
        for (int i = 0; i < 5; i++) {
            MoveToEx(hdc, 20, 110 + i*20, nullptr);
            LineTo(hdc, 20 + i*40 + 40, 110 + i*20);
        }
        DeleteObject(hPen);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    g_hInst = hInstance;
    InitCommonControls();

    // Daftarkan class Sapa
    WNDCLASSW wc1 = {};
    wc1.lpfnWndProc   = SapaProc;
    wc1.hInstance     = hInstance;
    wc1.lpszClassName = L"SapaWindow";
    wc1.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc1.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc1);

    // Daftarkan class GDI
    WNDCLASSW wc2 = {};
    wc2.lpfnWndProc   = GDIProc;
    wc2.hInstance     = hInstance;
    wc2.lpszClassName = L"GDIWindow";
    wc2.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc2.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc2);

    // Buat window sapa
    HWND hwndSapa = CreateWindowExW(0, L"SapaWindow",
        L"Halo Win32! — Belajar C++",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        100, 100, 290, 220, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwndSapa, nCmdShow);
    UpdateWindow(hwndSapa);

    // Buat window GDI
    HWND hwndGDI = CreateWindowExW(0, L"GDIWindow",
        L"Contoh GDI Drawing",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        420, 100, 290, 230, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hwndGDI, nCmdShow);
    UpdateWindow(hwndGDI);

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

#else  // Non-Windows stub
#include <iostream>
int main() {
    std::cout << "hello_win32.cpp hanya bisa dikompilasi di Windows!\n";
    std::cout << "Gunakan MinGW atau MSVC di sistem Windows.\n";
    return 1;
}
#endif
