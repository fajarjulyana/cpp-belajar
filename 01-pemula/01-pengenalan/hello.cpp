// ============================================================
// File  : hello.cpp
// Topik : Program pertama C++
// Kompilasi: g++ -std=c++17 -o hello hello.cpp
// ============================================================
#include <iostream>
#include <string>

int main() {
    // Mencetak teks ke konsol
    std::cout << "Halo, Dunia!" << std::endl;

    // Memakai variabel string
    std::string nama = "Belajar C++";
    std::cout << "Selamat datang di: " << nama << std::endl;

    // Membaca input dari pengguna
    std::cout << "Masukkan nama Anda: ";
    std::string inputNama;
    std::cin >> inputNama;
    std::cout << "Halo, " << inputNama << "! Selamat belajar C++." << std::endl;

    return 0;
}
