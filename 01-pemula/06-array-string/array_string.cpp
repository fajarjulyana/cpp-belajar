// ============================================================
// File  : array_string.cpp
// Topik : Array dan String dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o array_string array_string.cpp
// ============================================================
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
using namespace std;

// Fungsi bantu: cetak array
void cetakArray(const int arr[], int n) {
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << "\n";
}

// Bubble sort
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j+1]) swap(arr[j], arr[j+1]);
}

// Binary search — array harus terurut
int binarySearch(const int arr[], int n, int target) {
    int kiri = 0, kanan = n - 1;
    while (kiri <= kanan) {
        int tengah = kiri + (kanan - kiri) / 2;
        if (arr[tengah] == target) return tengah;
        if (arr[tengah] < target)  kiri  = tengah + 1;
        else                       kanan = tengah - 1;
    }
    return -1;
}

// Balik array
void balikArray(int arr[], int n) {
    for (int i = 0; i < n / 2; i++) swap(arr[i], arr[n-1-i]);
}

int main() {
    // ---- Array C-style ----
    cout << "=== Array C-style ===" << endl;
    int angka[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    int n = sizeof(angka) / sizeof(angka[0]);
    cout << "Array asli: ";
    cetakArray(angka, n);

    int maks = *max_element(angka, angka + n);
    int mins = *min_element(angka, angka + n);
    cout << "Maks: " << maks << ", Min: " << mins << endl;

    balikArray(angka, n);
    cout << "Setelah dibalik: ";
    cetakArray(angka, n);

    bubbleSort(angka, n);
    cout << "Setelah diurutkan: ";
    cetakArray(angka, n);

    int cari = 7;
    int pos  = binarySearch(angka, n, cari);
    cout << "Cari " << cari << ": indeks " << pos << endl;

    // ---- Array 2D ----
    cout << "\n=== Array 2D (Matriks 3x3) ===" << endl;
    int mat[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) cout << mat[i][j] << "\t";
        cout << "\n";
    }

    // ---- std::array ----
    cout << "\n=== std::array ===" << endl;
    array<int, 5> arr5 = {30, 10, 50, 20, 40};
    cout << "Sebelum sort: ";
    for (int x : arr5) cout << x << " ";
    sort(arr5.begin(), arr5.end());
    cout << "\nSesudah sort: ";
    for (int x : arr5) cout << x << " ";
    cout << "\nfront=" << arr5.front() << " back=" << arr5.back() << "\n";

    // ---- std::vector ----
    cout << "\n=== std::vector ===" << endl;
    vector<int> vec;
    for (int i = 1; i <= 5; i++) vec.push_back(i * 10);
    cout << "Vector: ";
    for (int v : vec) cout << v << " ";
    cout << "\nsize=" << vec.size() << "\n";
    vec.insert(vec.begin() + 2, 99);
    vec.erase(vec.end() - 1);
    cout << "Setelah insert(2,99) & erase(akhir): ";
    for (int v : vec) cout << v << " ";
    cout << "\n";

    // ---- std::string ----
    cout << "\n=== std::string ===" << endl;
    string s = "Halo, Dunia C++!";
    cout << "String: " << s << "\n";
    cout << "Panjang: " << s.length() << "\n";
    cout << "Karakter ke-7: " << s[7] << "\n";

    string sub = s.substr(7, 5);
    cout << "Substring(7,5): " << sub << "\n";

    size_t p = s.find("Dunia");
    cout << "Posisi 'Dunia': " << p << "\n";

    string upper = s;
    transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    cout << "Uppercase: " << upper << "\n";

    // Balik string
    string terbalik = s;
    reverse(terbalik.begin(), terbalik.end());
    cout << "Reversed: " << terbalik << "\n";

    // ---- Konversi ----
    cout << "\n=== Konversi String ↔ Angka ===" << endl;
    string numStr = "12345";
    int num = stoi(numStr);
    double dbl = stod("3.14159");
    string fromInt = to_string(9876);
    cout << "stoi(\"12345\") = " << num << "\n";
    cout << "stod(\"3.14159\") = " << dbl << "\n";
    cout << "to_string(9876) = " << fromInt << "\n";

    // ---- stringstream ----
    cout << "\n=== stringstream ===" << endl;
    string kalimat = "satu dua tiga empat lima";
    stringstream ss(kalimat);
    string kata;
    int hitungan = 0;
    while (ss >> kata) {
        cout << "[" << ++hitungan << "] " << kata << "\n";
    }

    // ---- Anagram check ----
    cout << "\n=== Cek Anagram ===" << endl;
    auto isAnagram = [](string a, string b) {
        sort(a.begin(), a.end());
        sort(b.begin(), b.end());
        return a == b;
    };
    cout << boolalpha;
    cout << "\"listen\" & \"silent\": " << isAnagram("listen","silent") << "\n";
    cout << "\"hello\" & \"world\": "   << isAnagram("hello","world")   << "\n";

    return 0;
}
