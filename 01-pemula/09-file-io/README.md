# Bab 9 — File I/O (Input/Output ke File)

Membaca dan menulis file adalah kebutuhan dasar dalam hampir semua aplikasi nyata.

---

## Library yang Digunakan

```cpp
#include <fstream>   // file stream
#include <iostream>  // cout, cin
#include <string>    // string
#include <sstream>   // stringstream
```

---

## Jenis Stream File

| Kelas | Fungsi |
|-------|--------|
| `ofstream` | Output (menulis ke file) |
| `ifstream` | Input (membaca dari file) |
| `fstream` | Input dan Output sekaligus |

---

## Menulis ke File

```cpp
#include <fstream>
#include <iostream>
using namespace std;

int main() {
    // Buka / buat file untuk ditulis
    ofstream file("output.txt");

    if (!file.is_open()) {
        cerr << "Gagal membuka file!" << endl;
        return 1;
    }

    file << "Baris pertama\n";
    file << "Baris kedua\n";
    file << "Angka: " << 42 << "\n";
    file << "Float: " << 3.14 << "\n";

    file.close();  // PENTING: tutup file setelah selesai
    cout << "File berhasil ditulis." << endl;
    return 0;
}
```

### Mode Penulisan

```cpp
// Truncate (default) — hapus isi lama, tulis dari awal
ofstream f1("file.txt");                         // default
ofstream f2("file.txt", ios::out);               // eksplisit

// Append — tambahkan di akhir file (isi lama tetap ada)
ofstream f3("file.txt", ios::app);

// Binary — tulis data biner
ofstream f4("data.bin", ios::binary);
```

---

## Membaca dari File

### Baca Kata per Kata
```cpp
ifstream file("input.txt");
string kata;

while (file >> kata) {
    cout << kata << "\n";
}
file.close();
```

### Baca Baris per Baris
```cpp
ifstream file("input.txt");
string baris;

while (getline(file, baris)) {
    cout << baris << "\n";
}
file.close();
```

### Baca Karakter per Karakter
```cpp
ifstream file("input.txt");
char c;

while (file.get(c)) {
    cout << c;
}
file.close();
```

### Baca Semua Konten Sekaligus
```cpp
#include <iterator>

ifstream file("input.txt");
string konten((istreambuf_iterator<char>(file)),
               istreambuf_iterator<char>());
cout << konten;
```

---

## fstream — Baca & Tulis

```cpp
fstream file("data.txt", ios::in | ios::out | ios::app);

// Tulis dulu
file << "Data baru\n";

// Balik ke awal untuk membaca
file.seekg(0, ios::beg);  // seekg = seek get (posisi baca)

string baris;
while (getline(file, baris)) {
    cout << baris << "\n";
}
file.close();
```

---

## Navigasi dalam File

```cpp
fstream file("data.txt", ios::in | ios::out);

// seekg — pindah posisi baca (get)
file.seekg(0, ios::beg);   // ke awal
file.seekg(0, ios::end);   // ke akhir
file.seekg(10, ios::beg);  // 10 byte dari awal
file.seekg(-5, ios::end);  // 5 byte sebelum akhir

// seekp — pindah posisi tulis (put)
file.seekp(0, ios::beg);

// Dapatkan posisi saat ini
streampos pos = file.tellg();
cout << "Posisi saat ini: " << pos << endl;

// Ukuran file
file.seekg(0, ios::end);
streamsize ukuran = file.tellg();
cout << "Ukuran file: " << ukuran << " byte" << endl;
```

---

## File Biner

```cpp
struct Pegawai {
    char  nama[50];
    int   id;
    double gaji;
};

// Tulis biner
ofstream fileBin("pegawai.dat", ios::binary);
Pegawai p = {"Budi Santoso", 101, 5000000.0};
fileBin.write(reinterpret_cast<char*>(&p), sizeof(Pegawai));
fileBin.close();

// Baca biner
ifstream baca("pegawai.dat", ios::binary);
Pegawai dibaca;
baca.read(reinterpret_cast<char*>(&dibaca), sizeof(Pegawai));
cout << dibaca.nama << " - " << dibaca.gaji << endl;
baca.close();
```

---

## Penanganan Error

```cpp
ifstream file("tidakada.txt");

if (!file) {
    cerr << "Error: file tidak bisa dibuka!" << endl;
    return 1;
}

// Cek state
if (file.fail()) cerr << "fail bit set\n";
if (file.bad())  cerr << "bad bit set\n";
if (file.eof())  cout << "end of file\n";

file.clear();  // reset error flags
```

---

## CSV — Membaca dan Menulis

```cpp
// Menulis CSV
ofstream csv("data.csv");
csv << "nama,nim,ipk\n";
csv << "Budi,20210001,3.75\n";
csv << "Ani,20210002,3.90\n";
csv.close();

// Membaca CSV
ifstream baca("data.csv");
string baris;
getline(baca, baris);  // lewati header

while (getline(baca, baris)) {
    stringstream ss(baris);
    string nama, nim, ipk;
    getline(ss, nama, ',');
    getline(ss, nim,  ',');
    getline(ss, ipk,  ',');
    cout << "Nama: " << nama << ", NIM: " << nim << ", IPK: " << ipk << "\n";
}
```

---

## Cek Keberadaan File

```cpp
// Cara 1: coba buka
bool fileAda(const string& path) {
    ifstream f(path);
    return f.good();
}

// Cara 2: C++17 filesystem
#include <filesystem>
namespace fs = std::filesystem;
if (fs::exists("file.txt")) cout << "File ada\n";
```

---

## Latihan

1. Buat program yang menyimpan daftar nama ke file dan membacanya kembali
2. Buat program yang menghitung jumlah kata dalam sebuah file teks
3. Buat sistem penyimpanan data siswa (nama, nilai) menggunakan file CSV
4. Buat program yang menyalin isi satu file ke file lain

---

**[← Struct](../08-struct/README.md)** | **[Berikutnya → OOP Dasar](../10-oop-dasar/README.md)**
