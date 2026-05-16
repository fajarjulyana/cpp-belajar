// ============================================================
// File  : pointer.cpp
// Topik : Pointer dan Referensi dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o pointer pointer.cpp
// ============================================================
#include <iostream>
#include <string>
using namespace std;

// Tukar menggunakan pointer
void tukarPointer(int* a, int* b) {
    int tmp = *a; *a = *b; *b = tmp;
}

// Tukar menggunakan referensi
void tukarRef(int& a, int& b) {
    int tmp = a; a = b; b = tmp;
}

// Cari elemen terbesar, kembalikan pointer-nya
int* cariMaks(int arr[], int n) {
    int* maks = &arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > *maks) maks = &arr[i];
    return maks;
}

// Salin string dengan pointer
void salinString(char* tujuan, const char* sumber) {
    while (*sumber) { *tujuan++ = *sumber++; }
    *tujuan = '\0';
}

int main() {
    // ---- Dasar Pointer ----
    cout << "=== Dasar Pointer ===" << endl;
    int x = 42;
    int* p = &x;
    cout << "x    = " << x    << endl;
    cout << "&x   = " << &x   << " (alamat)" << endl;
    cout << "p    = " << p    << " (nilai pointer)" << endl;
    cout << "*p   = " << *p   << " (dereference)" << endl;
    *p = 100;
    cout << "x setelah *p=100: " << x << endl;

    // ---- Pointer dan Array ----
    cout << "\n=== Pointer dan Array ===" << endl;
    int arr[] = {10, 20, 30, 40, 50};
    int* pa = arr;
    for (int i = 0; i < 5; i++) {
        cout << "arr[" << i << "] = " << *(pa + i)
             << " (alamat: " << (pa + i) << ")\n";
    }

    // ---- Pointer ke Pointer ----
    cout << "\n=== Pointer ke Pointer ===" << endl;
    int val = 7;
    int*  ptr1 = &val;
    int** ptr2 = &ptr1;
    cout << "val = " << val    << endl;
    cout << "*ptr1 = " << *ptr1  << endl;
    cout << "**ptr2 = " << **ptr2 << endl;
    **ptr2 = 99;
    cout << "val setelah **ptr2=99: " << val << endl;

    // ---- Referensi ----
    cout << "\n=== Referensi ===" << endl;
    int r = 10;
    int& ref = r;
    cout << "r = " << r << ", ref = " << ref << endl;
    ref = 50;
    cout << "r setelah ref=50: " << r << endl;
    cout << "alamat sama? " << boolalpha << (&r == &ref) << endl;

    // ---- Tukar ----
    cout << "\n=== Tukar dengan Pointer vs Referensi ===" << endl;
    int a = 3, b = 7;
    cout << "Sebelum: a=" << a << " b=" << b << endl;
    tukarPointer(&a, &b);
    cout << "Pointer: a=" << a << " b=" << b << endl;
    tukarRef(a, b);
    cout << "Ref:     a=" << a << " b=" << b << endl;

    // ---- Cari Maks ----
    cout << "\n=== Pointer ke Elemen Terbesar ===" << endl;
    int data[] = {4, 7, 2, 9, 1, 5};
    int* maks = cariMaks(data, 6);
    cout << "Nilai terbesar: " << *maks
         << " di indeks " << (maks - data) << endl;

    // ---- Memori Dinamis ----
    cout << "\n=== Memori Dinamis (new/delete) ===" << endl;
    int* dp = new int(42);
    cout << "Alokasi int: " << *dp << endl;
    *dp = 99;
    cout << "Setelah ubah: " << *dp << endl;
    delete dp;
    dp = nullptr;

    int n = 5;
    int* dynArr = new int[n];
    for (int i = 0; i < n; i++) dynArr[i] = (i + 1) * 10;
    cout << "Array dinamis: ";
    for (int i = 0; i < n; i++) cout << dynArr[i] << " ";
    cout << "\n";
    delete[] dynArr;
    dynArr = nullptr;

    // ---- Matriks Dinamis ----
    cout << "\n=== Matriks Dinamis ===" << endl;
    int rows = 3, cols = 3;
    int** mat = new int*[rows];
    for (int i = 0; i < rows; i++) {
        mat[i] = new int[cols];
        for (int j = 0; j < cols; j++)
            mat[i][j] = i * cols + j + 1;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) cout << mat[i][j] << "\t";
        cout << "\n";
    }
    for (int i = 0; i < rows; i++) delete[] mat[i];
    delete[] mat;

    // ---- Pointer ke Fungsi ----
    cout << "\n=== Pointer ke Fungsi ===" << endl;
    auto tambah = [](int a, int b) { return a + b; };
    auto kurang = [](int a, int b) { return a - b; };
    int (*ops[])(int, int) = {
        [](int a, int b){ return a + b; },
        [](int a, int b){ return a - b; },
        [](int a, int b){ return a * b; }
    };
    string namaOps[] = {"tambah", "kurang", "kali"};
    for (int i = 0; i < 3; i++)
        cout << "10 " << namaOps[i] << " 3 = " << ops[i](10, 3) << "\n";

    // ---- const Pointer ----
    cout << "\n=== const Pointer ===" << endl;
    int cx = 10, cy = 20;
    const int* cp1 = &cx;  // pointer ke const
    int* const cp2 = &cx;  // const pointer
    cout << "*cp1 = " << *cp1 << " (tidak bisa ubah nilai)\n";
    cp1 = &cy;  // OK: pointer bisa diarahkan ulang
    cout << "*cp1 setelah cp1=&cy: " << *cp1 << "\n";
    *cp2 = 55;  // OK: nilai bisa diubah
    cout << "cx setelah *cp2=55: " << cx << "\n";

    // ---- Salin String C-style dengan Pointer ----
    cout << "\n=== Salin String C-style ===" << endl;
    const char* src = "Halo, Dunia!";
    char dst[50];
    salinString(dst, src);
    cout << "Sumber : " << src << "\n";
    cout << "Salinan: " << dst << "\n";

    return 0;
}
