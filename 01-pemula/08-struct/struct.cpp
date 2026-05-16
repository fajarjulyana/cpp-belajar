// ============================================================
// File  : struct.cpp
// Topik : Struct dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o struct_demo struct.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

// ---- Definisi Struct ----
struct Mahasiswa {
    string nama;
    int    nim;
    double ipk;
    bool   aktif;
};

struct Tanggal {
    int hari, bulan, tahun;
    string toString() const {
        return to_string(hari) + "/" + to_string(bulan) + "/" + to_string(tahun);
    }
};

struct Pegawai {
    string  nama;
    int     id;
    Tanggal tanggalMasuk;
    double  gaji;
};

struct Titik {
    double x, y;
    double jarakKe(const Titik& lain) const {
        double dx = x - lain.x, dy = y - lain.y;
        return sqrt(dx*dx + dy*dy);
    }
    string toString() const {
        return "(" + to_string(x) + ", " + to_string(y) + ")";
    }
};

struct Lingkaran {
    Titik  pusat;
    double radius;
    double luas()     const { return 3.14159265 * radius * radius; }
    double keliling() const { return 2.0 * 3.14159265 * radius; }
};

// ---- Fungsi untuk Struct ----
void tampilkanMhs(const Mahasiswa& m) {
    cout << "  NIM   : " << m.nim << "\n"
         << "  Nama  : " << m.nama << "\n"
         << "  IPK   : " << m.ipk << "\n"
         << "  Aktif : " << boolalpha << m.aktif << "\n";
}

Mahasiswa buatMhs(const string& nama, int nim, double ipk) {
    return {nama, nim, ipk, true};
}

int main() {
    // ---- Struct Mahasiswa ----
    cout << "=== Struct Mahasiswa ===" << endl;
    Mahasiswa mhs1 = {"Budi Santoso", 20210001, 3.75, true};
    Mahasiswa mhs2 = buatMhs("Ani Rahayu", 20210002, 3.90);
    tampilkanMhs(mhs1);
    cout << "---\n";
    tampilkanMhs(mhs2);

    // ---- Array of Struct ----
    cout << "\n=== Daftar Kelas ===" << endl;
    vector<Mahasiswa> kelas = {
        {"Alfa",    1001, 3.8, true},
        {"Beta",    1002, 3.5, true},
        {"Gamma",   1003, 3.9, false},
        {"Delta",   1004, 3.2, true},
        {"Epsilon", 1005, 3.7, true}
    };
    // Urutkan berdasarkan IPK (descending)
    sort(kelas.begin(), kelas.end(), [](const Mahasiswa& a, const Mahasiswa& b){
        return a.ipk > b.ipk;
    });
    cout << "Diurutkan berdasarkan IPK:\n";
    for (int i = 0; i < (int)kelas.size(); i++) {
        cout << i+1 << ". " << kelas[i].nama << " - IPK: " << kelas[i].ipk << "\n";
    }

    // ---- Struct Bersarang ----
    cout << "\n=== Struct Bersarang (Pegawai) ===" << endl;
    Pegawai peg = {"Dewi Lestari", 501, {15, 6, 2020}, 8500000.0};
    cout << "ID   : " << peg.id << "\n";
    cout << "Nama : " << peg.nama << "\n";
    cout << "Masuk: " << peg.tanggalMasuk.toString() << "\n";
    cout << "Gaji : Rp " << peg.gaji << "\n";

    // ---- Pointer ke Struct ----
    cout << "\n=== Pointer ke Struct ===" << endl;
    Mahasiswa* ptr = &mhs1;
    cout << "Via pointer (->) : " << ptr->nama << ", IPK: " << ptr->ipk << "\n";
    ptr->ipk = 3.85;
    cout << "Setelah ubah IPK : " << mhs1.ipk << "\n";

    // ---- Struct dengan Method (Titik & Lingkaran) ----
    cout << "\n=== Struct Titik dan Lingkaran ===" << endl;
    Titik A = {0.0, 0.0};
    Titik B = {3.0, 4.0};
    cout << "A = " << A.toString() << "\n";
    cout << "B = " << B.toString() << "\n";
    cout << "Jarak A ke B = " << A.jarakKe(B) << "\n";

    Lingkaran l = {Titik{2.0, 3.0}, 5.0};
    cout << "\nLingkaran:\n";
    cout << "  Pusat    : " << l.pusat.toString() << "\n";
    cout << "  Radius   : " << l.radius << "\n";
    cout << "  Luas     : " << l.luas() << "\n";
    cout << "  Keliling : " << l.keliling() << "\n";

    // ---- Struct Stack Sederhana ----
    cout << "\n=== Stack Sederhana menggunakan Struct ===" << endl;
    struct Stack {
        int data[100];
        int top = -1;
        bool kosong() const { return top == -1; }
        bool penuh()  const { return top == 99; }
        void push(int v) { if (!penuh()) data[++top] = v; }
        int  pop()       { return kosong() ? -1 : data[top--]; }
        int  peek() const{ return kosong() ? -1 : data[top]; }
        int  size() const{ return top + 1; }
    };

    Stack s;
    for (int i = 1; i <= 5; i++) s.push(i * 10);
    cout << "Stack (top=" << s.peek() << ", size=" << s.size() << "): ";
    while (!s.kosong()) cout << s.pop() << " ";
    cout << "\n";

    return 0;
}
