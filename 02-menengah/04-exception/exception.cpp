// ============================================================
// File  : exception.cpp
// Topik : Exception Handling dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o exception_demo exception.cpp
// ============================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

// ---- Custom Exceptions ----
class DivisiNolException : public runtime_error {
public:
    DivisiNolException() : runtime_error("Pembagian dengan nol tidak diizinkan") {}
};

class SaldoKurangException : public runtime_error {
    double saldo, nominal;
public:
    SaldoKurangException(double s, double n)
        : runtime_error("Saldo tidak cukup"),
          saldo(s), nominal(n) {}
    string detail() const {
        return "Saldo=" + to_string(saldo) + ", Diminta=" + to_string(nominal);
    }
};

class InputTidakValidException : public invalid_argument {
    string field;
public:
    InputTidakValidException(const string& f, const string& msg)
        : invalid_argument(msg), field(f) {}
    string getField() const { return field; }
};

// ---- Fungsi yang melempar exception ----
double bagi(double a, double b) {
    if (b == 0.0) throw DivisiNolException();
    return a / b;
}

double akarKuadrat(double x) {
    if (x < 0) throw domain_error("Akar dari bilangan negatif tidak real: " + to_string(x));
    return sqrt(x);
}

// ---- Kelas BankAccount dengan exception ----
class BankAccount {
    string pemilik;
    double saldo;
    bool   terkunci;
public:
    BankAccount(const string& nama, double awal)
        : pemilik(nama), saldo(awal), terkunci(false) {
        if (nama.empty()) throw InputTidakValidException("nama", "Nama tidak boleh kosong");
        if (awal < 0)     throw invalid_argument("Saldo awal tidak boleh negatif");
    }

    void setor(double nominal) {
        if (terkunci)  throw runtime_error("Akun terkunci: " + pemilik);
        if (nominal <= 0) throw invalid_argument("Nominal setor harus positif");
        saldo += nominal;
    }

    void tarik(double nominal) {
        if (terkunci) throw runtime_error("Akun terkunci: " + pemilik);
        if (nominal <= 0) throw invalid_argument("Nominal tarik harus positif");
        if (nominal > saldo) throw SaldoKurangException(saldo, nominal);
        saldo -= nominal;
    }

    void kunci() { terkunci = true; }
    double getSaldo() const { return saldo; }
    string getPemilik() const { return pemilik; }
};

// ---- Antrian dengan exception ----
template<typename T>
class AntrianTerbatas {
    vector<T> data;
    int kapMaks;
public:
    AntrianTerbatas(int k) : kapMaks(k) {}
    void masuk(const T& val) {
        if ((int)data.size() >= kapMaks)
            throw overflow_error("Antrian penuh (maks=" + to_string(kapMaks) + ")");
        data.push_back(val);
    }
    T keluar() {
        if (data.empty()) throw underflow_error("Antrian kosong");
        T val = data.front();
        data.erase(data.begin());
        return val;
    }
    int ukuran() const { return (int)data.size(); }
};

// ---- Exception safety dengan RAII ----
class SumberDaya {
    string nama;
    static int jumlah;
public:
    SumberDaya(const string& n) : nama(n) {
        ++jumlah;
        cout << "  [Buka] " << nama << " (total=" << jumlah << ")\n";
    }
    ~SumberDaya() {
        --jumlah;
        cout << "  [Tutup] " << nama << " (total=" << jumlah << ")\n";
    }
};
int SumberDaya::jumlah = 0;

void operasiBerisiko(bool gagal) {
    SumberDaya s1("Koneksi DB");
    SumberDaya s2("File Log");
    if (gagal) throw runtime_error("Operasi gagal di tengah jalan!");
    cout << "  Operasi sukses\n";
}

int main() {
    // ---- Exception Dasar ----
    cout << "=== Exception Dasar ===" << endl;
    try {
        cout << "10 / 2 = " << bagi(10, 2) << "\n";
        cout << "10 / 0 = " << bagi(10, 0) << "\n";  // exception!
    } catch (const DivisiNolException& e) {
        cerr << "Caught: " << e.what() << "\n";
    }

    // ---- domain_error ----
    try {
        cout << "\nakar(16) = " << akarKuadrat(16) << "\n";
        cout << "akar(-4) = " << akarKuadrat(-4)   << "\n";  // exception!
    } catch (const domain_error& e) {
        cerr << "Caught: " << e.what() << "\n";
    }

    // ---- BankAccount ----
    cout << "\n=== BankAccount dengan Exception ===" << endl;
    try {
        BankAccount akun("Budi", 500000);
        akun.setor(100000);
        akun.tarik(200000);
        cout << "Saldo " << akun.getPemilik() << ": " << akun.getSaldo() << "\n";
        akun.tarik(999999);  // SaldoKurangException!
    } catch (const SaldoKurangException& e) {
        cerr << "Caught SaldoKurang: " << e.what() << "\n  " << e.detail() << "\n";
    } catch (const exception& e) {
        cerr << "Caught: " << e.what() << "\n";
    }

    // Akun terkunci
    try {
        BankAccount akun2("Ani", 200000);
        akun2.kunci();
        akun2.setor(50000);  // runtime_error (terkunci)
    } catch (const runtime_error& e) {
        cerr << "Caught RuntimeError: " << e.what() << "\n";
    }

    // Input tidak valid
    try {
        BankAccount akun3("", 100000);
    } catch (const InputTidakValidException& e) {
        cerr << "Caught InputTidakValid: field=" << e.getField() << " — " << e.what() << "\n";
    }

    // ---- AntrianTerbatas ----
    cout << "\n=== AntrianTerbatas dengan Exception ===" << endl;
    AntrianTerbatas<string> antrian(3);
    try {
        antrian.masuk("A"); antrian.masuk("B"); antrian.masuk("C");
        antrian.masuk("D");  // overflow_error!
    } catch (const overflow_error& e) {
        cerr << "Caught Overflow: " << e.what() << "\n";
    }
    while (antrian.ukuran() > 0) cout << "  Keluar: " << antrian.keluar() << "\n";
    try {
        antrian.keluar();  // underflow_error!
    } catch (const underflow_error& e) {
        cerr << "Caught Underflow: " << e.what() << "\n";
    }

    // ---- RAII (Exception Safety) ----
    cout << "\n=== RAII — Exception Safety ===" << endl;
    cout << "Operasi sukses:\n";
    try { operasiBerisiko(false); }
    catch (const exception& e) { cerr << e.what() << "\n"; }

    cout << "Operasi gagal:\n";
    try { operasiBerisiko(true); }
    catch (const exception& e) { cerr << "Caught: " << e.what() << "\n"; }
    // Perhatikan: SumberDaya tetap ditutup (destruktor dipanggil)!

    // ---- vector::at() ----
    cout << "\n=== std::exception out_of_range ===" << endl;
    try {
        vector<int> v = {1,2,3};
        cout << v.at(10) << "\n";
    } catch (const out_of_range& e) {
        cerr << "Caught out_of_range: " << e.what() << "\n";
    }

    return 0;
}
