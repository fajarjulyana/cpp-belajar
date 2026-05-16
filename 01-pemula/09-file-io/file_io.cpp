// ============================================================
// File  : file_io.cpp
// Topik : File I/O dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o file_io file_io.cpp
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Siswa {
    string nama;
    int    nim;
    double nilai;
};

void tulisFileTeks(const string& path) {
    ofstream f(path);
    if (!f) { cerr << "Gagal buka " << path << "\n"; return; }
    f << "Laporan Nilai Siswa\n";
    f << "==================\n";
    f << "Alfa,  NIM=1001, Nilai=85.5\n";
    f << "Beta,  NIM=1002, Nilai=90.0\n";
    f << "Gamma, NIM=1003, Nilai=78.5\n";
    f.close();
    cout << "Berhasil menulis: " << path << "\n";
}

void bacaFileTeks(const string& path) {
    ifstream f(path);
    if (!f) { cerr << "Gagal buka " << path << "\n"; return; }
    string baris;
    int nomor = 1;
    cout << "\n--- Isi " << path << " ---\n";
    while (getline(f, baris)) {
        cout << nomor++ << ": " << baris << "\n";
    }
}

void tulisCSV(const string& path, const vector<Siswa>& siswa) {
    ofstream f(path);
    f << "nama,nim,nilai\n";
    for (const auto& s : siswa) {
        f << s.nama << "," << s.nim << "," << s.nilai << "\n";
    }
    cout << "Berhasil menulis CSV: " << path << "\n";
}

vector<Siswa> bacaCSV(const string& path) {
    vector<Siswa> hasil;
    ifstream f(path);
    string baris;
    getline(f, baris);  // lewati header
    while (getline(f, baris)) {
        stringstream ss(baris);
        string nama, nimStr, nilaiStr;
        getline(ss, nama,    ',');
        getline(ss, nimStr,  ',');
        getline(ss, nilaiStr,',');
        hasil.push_back({nama, stoi(nimStr), stod(nilaiStr)});
    }
    return hasil;
}

void tulisBiner(const string& path) {
    ofstream f(path, ios::binary);
    int    arr[] = {10, 20, 30, 40, 50};
    double dbl   = 3.14159;
    f.write(reinterpret_cast<char*>(arr), sizeof(arr));
    f.write(reinterpret_cast<char*>(&dbl), sizeof(dbl));
    cout << "Berhasil menulis biner: " << path << "\n";
}

void bacaBiner(const string& path) {
    ifstream f(path, ios::binary);
    int    arr[5];
    double dbl;
    f.read(reinterpret_cast<char*>(arr), sizeof(arr));
    f.read(reinterpret_cast<char*>(&dbl), sizeof(dbl));
    cout << "Array biner: ";
    for (int x : arr) cout << x << " ";
    cout << "\nDouble biner: " << dbl << "\n";
}

long long ukuranFile(const string& path) {
    ifstream f(path, ios::ate | ios::binary);
    return f.tellg();
}

int hitungKata(const string& path) {
    ifstream f(path);
    string kata;
    int count = 0;
    while (f >> kata) count++;
    return count;
}

bool salinFile(const string& src, const string& dst) {
    ifstream masuk(src, ios::binary);
    ofstream keluar(dst, ios::binary);
    if (!masuk || !keluar) return false;
    keluar << masuk.rdbuf();
    return true;
}

int main() {
    const string fileTeks = "latihan.txt";
    const string fileCSV  = "siswa.csv";
    const string fileBin  = "data.bin";

    // ---- Tulis dan Baca Teks ----
    cout << "=== File Teks ===" << endl;
    tulisFileTeks(fileTeks);
    bacaFileTeks(fileTeks);
    cout << "Jumlah kata: " << hitungKata(fileTeks) << "\n";
    cout << "Ukuran file: " << ukuranFile(fileTeks) << " byte\n";

    // ---- Append ----
    cout << "\n=== Append ke File ===" << endl;
    {
        ofstream fa(fileTeks, ios::app);
        fa << "Delta, NIM=1004, Nilai=92.0\n";
        fa << "Epsilon, NIM=1005, Nilai=88.5\n";
    }
    cout << "Setelah append:\n";
    bacaFileTeks(fileTeks);

    // ---- CSV ----
    cout << "\n=== File CSV ===" << endl;
    vector<Siswa> daftarSiswa = {
        {"Alfa",    2001, 85.5},
        {"Beta",    2002, 90.0},
        {"Gamma",   2003, 78.5},
        {"Delta",   2004, 92.0},
        {"Epsilon", 2005, 88.5}
    };
    tulisCSV(fileCSV, daftarSiswa);
    auto dibaca = bacaCSV(fileCSV);
    cout << "\nData dari CSV:\n";
    for (const auto& s : dibaca) {
        cout << "  " << s.nama << " (NIM=" << s.nim << ") Nilai=" << s.nilai << "\n";
    }

    // ---- Cari nilai tertinggi ----
    auto terbaik = *max_element(dibaca.begin(), dibaca.end(),
        [](const Siswa& a, const Siswa& b){ return a.nilai < b.nilai; });
    cout << "Nilai tertinggi: " << terbaik.nama << " = " << terbaik.nilai << "\n";

    // ---- File Biner ----
    cout << "\n=== File Biner ===" << endl;
    tulisBiner(fileBin);
    bacaBiner(fileBin);

    // ---- Salin File ----
    cout << "\n=== Salin File ===" << endl;
    bool ok = salinFile(fileTeks, "salinan.txt");
    cout << "Salin " << fileTeks << " → salinan.txt: " << boolalpha << ok << "\n";

    // ---- fstream (baca dan tulis) ----
    cout << "\n=== fstream (Baca & Tulis) ===" << endl;
    {
        fstream fRW("catatan.txt", ios::out | ios::in | ios::trunc);
        fRW << "Catatan pertama\nCatatan kedua\nCatatan ketiga\n";
        fRW.seekg(0, ios::beg);
        string baris;
        cout << "Isi catatan:\n";
        while (getline(fRW, baris)) cout << "  " << baris << "\n";
    }

    return 0;
}
