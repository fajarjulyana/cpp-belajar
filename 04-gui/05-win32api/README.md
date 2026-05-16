# GUI dengan Win32 API

Win32 API adalah antarmuka pemrograman Windows tingkat rendah. Memberikan kontrol penuh atas jendela, pesan, dan resource.

---

## Kompilasi

```bash
# MinGW (Windows)
g++ -std=c++17 -mwindows -o hello_win32.exe hello_win32.cpp -lgdi32 -lcomctl32 -lcomdlg32

# MSVC (Developer Command Prompt)
cl /std:c++17 hello_win32.cpp /link user32.lib gdi32.lib comctl32.lib

# Cross-compile dari Linux
x86_64-w64-mingw32-g++ -std=c++17 -mwindows -o hello_win32.exe hello_win32.cpp
```

---

## Struktur Program Win32

```cpp
#include <windows.h>

// Procedure: menangani semua message untuk window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // Window baru dibuat — inisialisasi widget di sini
        return 0;
    case WM_COMMAND:
        // Tombol diklik, menu dipilih, dll.
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 10, 10, L"Hello, Win32!", 13);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);  // Kirim WM_QUIT
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 1. Daftarkan class window
    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"MyWindow";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // 2. Buat window
    HWND hwnd = CreateWindowEx(0, L"MyWindow", L"Judul",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        nullptr, nullptr, hInstance, nullptr);

    // 3. Tampilkan
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 4. Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
```

---

## Membuat Control (Widget)

```cpp
// Di dalam WM_CREATE handler:
HWND hButton = CreateWindow(L"BUTTON", L"Klik Saya",
    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
    10, 10, 100, 30, hwnd, (HMENU)ID_BUTTON, hInst, nullptr);

HWND hEdit = CreateWindow(L"EDIT", L"",
    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
    10, 50, 200, 25, hwnd, (HMENU)ID_EDIT, hInst, nullptr);

HWND hLabel = CreateWindow(L"STATIC", L"Halo Win32!",
    WS_VISIBLE | WS_CHILD,
    10, 90, 300, 25, hwnd, nullptr, hInst, nullptr);

// Di WM_COMMAND:
if (LOWORD(wParam) == ID_BUTTON) {
    wchar_t buf[256];
    GetWindowText(hEdit, buf, 256);
    MessageBox(hwnd, buf, L"Input", MB_OK);
}
```

---

## Resource dan Icon

```cpp
// resource.h
#define IDI_ICON1  101
#define IDR_MENU1  201
#define IDD_DIALOG 301

// resource.rc
// IDI_ICON1 ICON "icon.ico"
// IDR_MENU1 MENU { ... }

// Load icon
HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
```

---

## GDI — Menggambar

```cpp
// Di WM_PAINT:
PAINTSTRUCT ps;
HDC hdc = BeginPaint(hwnd, &ps);

// Buat pen dan brush
HPEN   hPen   = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 255));
SelectObject(hdc, hPen);
SelectObject(hdc, hBrush);

// Gambar bentuk
Rectangle(hdc, 10, 10, 100, 60);
Ellipse(hdc, 120, 10, 200, 60);

// Gambar teks
SetTextColor(hdc, RGB(0, 0, 0));
SetBkMode(hdc, TRANSPARENT);
TextOut(hdc, 10, 80, L"Hello GDI!", 10);

// Bersihkan resource!
DeleteObject(hPen);
DeleteObject(hBrush);
EndPaint(hwnd, &ps);
```

---

## Latihan

1. Buat kalkulator dengan tombol grid dan display
2. Buat notepad sederhana dengan `EDIT` multiline dan menu File
3. Buat paint app dengan GDI dan mouse event
4. Buat dialog pengaturan dengan `DialogBox`

---

**[← wxWidgets](../04-wxwidgets/README.md)** | **[→ Database: SQLite](../../05-database/01-sqlite/README.md)**
