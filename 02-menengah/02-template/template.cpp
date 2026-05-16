// ============================================================
// File  : template.cpp
// Topik : Template C++
// Kompilasi: g++ -std=c++17 -Wall -o template_demo template.cpp
// ============================================================
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <type_traits>
using namespace std;

// ---- Function Template ----
template<typename T>
T maks(T a, T b) { return a > b ? a : b; }

template<typename T>
T mins(T a, T b) { return a < b ? a : b; }

template<typename T>
void urutkanArray(T arr[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1]) swap(arr[j], arr[j+1]);
}

// ---- Class Template: Tumpukan ----
template<typename T>
class Tumpukan {
    vector<T> data;
    int kapMaks;
public:
    explicit Tumpukan(int k = 50) : kapMaks(k) {}
    void push(const T& v) {
        if ((int)data.size() >= kapMaks) throw overflow_error("Penuh!");
        data.push_back(v);
    }
    T pop() {
        if (kosong()) throw underflow_error("Kosong!");
        T v = data.back(); data.pop_back(); return v;
    }
    T& top()   { if (kosong()) throw underflow_error("Kosong!"); return data.back(); }
    bool kosong()  const { return data.empty(); }
    int  ukuran()  const { return (int)data.size(); }
    void cetak()   const {
        cout << "[";
        for (int i = 0; i < (int)data.size(); i++) {
            cout << data[i];
            if (i < (int)data.size()-1) cout << ", ";
        }
        cout << "] (top=" << (kosong() ? "N/A" : to_string(data.back())) << ")\n";
    }
};

// ---- Class Template: Pasangan ----
template<typename F, typename S>
struct Pasangan {
    F pertama;
    S kedua;
    Pasangan(F f, S s) : pertama(f), kedua(s) {}
    void cetak() const {
        cout << "(" << pertama << ", " << kedua << ")\n";
    }
};

// ---- Template Specialization ----
template<typename T>
string tipeString() { return "unknown"; }
template<> string tipeString<int>()    { return "int"; }
template<> string tipeString<double>() { return "double"; }
template<> string tipeString<string>() { return "string"; }
template<> string tipeString<bool>()   { return "bool"; }

// ---- Non-type Template Parameter ----
template<typename T, int N>
class ArrayTetap {
    T data[N];
public:
    ArrayTetap() { fill(data, data+N, T{}); }
    T&       operator[](int i)       { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int ukuran() const { return N; }
    void isi(T v) { fill(data, data+N, v); }
    void cetak() const {
        for (int i = 0; i < N; i++) cout << data[i] << " ";
        cout << "\n";
    }
};

// ---- Variadic Template ----
void cetak() { cout << "\n"; }

template<typename T, typename... Rest>
void cetak(T v, Rest... rest) {
    cout << v;
    if (sizeof...(rest) > 0) cout << " | ";
    cetak(rest...);
}

// Jumlahkan semua argumen
template<typename T>
T jumlahkan(T v) { return v; }

template<typename T, typename... Args>
T jumlahkan(T first, Args... rest) { return first + jumlahkan(rest...); }

// ---- Template Metaprogramming ----
template<int N>
struct Faktorial {
    static constexpr long long nilai = N * Faktorial<N-1>::nilai;
};
template<>
struct Faktorial<0> { static constexpr long long nilai = 1; };

template<int N>
struct Fibonacci {
    static constexpr int nilai = Fibonacci<N-1>::nilai + Fibonacci<N-2>::nilai;
};
template<> struct Fibonacci<0> { static constexpr int nilai = 0; };
template<> struct Fibonacci<1> { static constexpr int nilai = 1; };

// ---- SFINAE: hanya untuk tipe aritmatika ----
template<typename T>
typename enable_if<is_arithmetic<T>::value, double>::type
rataRata(vector<T>& v) {
    double jumlah = 0;
    for (auto& x : v) jumlah += x;
    return jumlah / v.size();
}

int main() {
    // ---- Function Template ----
    cout << "=== Function Template ===" << endl;
    cout << "maks(3, 7)    = " << maks(3, 7)         << "\n";
    cout << "maks(3.5,2.1) = " << maks(3.5, 2.1)     << "\n";
    cout << "maks('a','z') = " << maks('a', 'z')      << "\n";
    cout << "mins(10, 4)   = " << mins(10, 4)         << "\n";

    int arr[] = {5, 2, 8, 1, 9, 3};
    urutkanArray(arr, 6);
    cout << "Array terurut: ";
    for (int x : arr) cout << x << " ";
    cout << "\n";

    // ---- Class Template: Tumpukan ----
    cout << "\n=== Class Template (Tumpukan) ===" << endl;
    Tumpukan<int> ts;
    for (int i = 10; i <= 50; i += 10) ts.push(i);
    cout << "Stack int: "; ts.cetak();
    cout << "Pop: " << ts.pop() << "\n";
    cout << "Top: " << ts.top() << "\n";

    Tumpukan<string> ss;
    ss.push("pertama"); ss.push("kedua"); ss.push("ketiga");
    cout << "Stack string: "; ss.cetak();
    while (!ss.kosong()) cout << "Pop: " << ss.pop() << "\n";

    // ---- Pasangan ----
    cout << "\n=== Class Template (Pasangan) ===" << endl;
    Pasangan<string, int>    p1("umur", 25);
    Pasangan<double, string> p2(3.14, "pi");
    Pasangan<bool, char>     p3(true, 'X');
    p1.cetak(); p2.cetak(); p3.cetak();

    // ---- Specialization ----
    cout << "\n=== Template Specialization ===" << endl;
    cout << "int    : " << tipeString<int>()    << "\n";
    cout << "double : " << tipeString<double>() << "\n";
    cout << "string : " << tipeString<string>() << "\n";
    cout << "bool   : " << tipeString<bool>()   << "\n";

    // ---- Non-type Template ----
    cout << "\n=== Non-type Template Parameter ===" << endl;
    ArrayTetap<int, 5> at5;
    for (int i = 0; i < at5.ukuran(); i++) at5[i] = (i+1) * 10;
    cout << "ArrayTetap<int,5>: "; at5.cetak();

    ArrayTetap<double, 3> at3;
    at3.isi(3.14);
    cout << "ArrayTetap<double,3> isi 3.14: "; at3.cetak();

    // ---- Variadic Template ----
    cout << "\n=== Variadic Template ===" << endl;
    cetak(1, 2.5, "tiga", 'D', true);
    cout << "jumlahkan(1,2,3,4,5) = " << jumlahkan(1,2,3,4,5) << "\n";
    cout << "jumlahkan(1.1,2.2,3.3) = " << jumlahkan(1.1,2.2,3.3) << "\n";

    // ---- Metaprogramming ----
    cout << "\n=== Template Metaprogramming ===" << endl;
    cout << "Faktorial<0>  = " << Faktorial<0>::nilai  << "\n";
    cout << "Faktorial<5>  = " << Faktorial<5>::nilai  << "\n";
    cout << "Faktorial<10> = " << Faktorial<10>::nilai << "\n";
    cout << "Fibonacci<10> = " << Fibonacci<10>::nilai << "\n";
    cout << "Fibonacci<20> = " << Fibonacci<20>::nilai << "\n";

    // ---- SFINAE ----
    cout << "\n=== SFINAE (rataRata) ===" << endl;
    vector<int>    vi = {10, 20, 30, 40, 50};
    vector<double> vd = {1.1, 2.2, 3.3};
    cout << "rataRata int   : " << rataRata(vi) << "\n";
    cout << "rataRata double: " << rataRata(vd) << "\n";

    return 0;
}
