// ============================================================
// File  : metaprogramming.cpp
// Topik : Template Metaprogramming & Compile-Time Programming
// Kompilasi: g++ -std=c++17 -Wall -o tmp_demo metaprogramming.cpp
// ============================================================
#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>
#include <string>
#include <array>
#include <numeric>
using namespace std;

// ---- Compile-Time Faktorial & Fibonacci ----
template<int N> struct Faktorial { static constexpr long long nilai = N * Faktorial<N-1>::nilai; };
template<>      struct Faktorial<0> { static constexpr long long nilai = 1; };

constexpr long long faktorial(int n) { return n <= 1 ? 1 : n * faktorial(n-1); }

template<int N> struct Fib { static constexpr int nilai = Fib<N-1>::nilai + Fib<N-2>::nilai; };
template<>      struct Fib<0> { static constexpr int nilai = 0; };
template<>      struct Fib<1> { static constexpr int nilai = 1; };

// ---- GCD & LCM compile-time ----
template<int A, int B> struct GCD { static constexpr int nilai = GCD<B, A%B>::nilai; };
template<int A>        struct GCD<A,0> { static constexpr int nilai = A; };
template<int A, int B> struct LCM { static constexpr int nilai = A / GCD<A,B>::nilai * B; };

// ---- IsPrime compile-time ----
template<int N, int D> struct PrimeHelper { static constexpr bool nilai = (N%D!=0) && PrimeHelper<N,D-1>::nilai; };
template<int N>        struct PrimeHelper<N,1> { static constexpr bool nilai = (N>1); };
template<int N>        struct IsPrime { static constexpr bool nilai = PrimeHelper<N, N-1>::nilai; };

// ---- Type Traits ----
template<typename T>
void infoTipe(const string& nama) {
    cout << "  " << nama << ":\n";
    cout << "    is_integral        = " << boolalpha << is_integral_v<T> << "\n";
    cout << "    is_floating_point  = " << is_floating_point_v<T> << "\n";
    cout << "    is_signed          = " << is_signed_v<T> << "\n";
    cout << "    sizeof             = " << sizeof(T) << " byte\n";
}

// ---- SFINAE: hanya untuk tipe numerik ----
template<typename T, typename = enable_if_t<is_arithmetic_v<T>>>
T rataRata(const vector<T>& v) {
    return accumulate(v.begin(), v.end(), T(0)) / static_cast<T>(v.size());
}

// ---- if constexpr (C++17) ----
template<typename T>
void cetak(const T& val) {
    if constexpr (is_integral_v<T>) {
        cout << "  int: " << val << " (0x" << hex << val << dec << ")\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "  float: " << fixed << val << "\n";
    } else if constexpr (is_same_v<T, string>) {
        cout << "  string[" << val.size() << "]: " << val << "\n";
    } else {
        cout << "  lain: " << val << "\n";
    }
}

// ---- Fold Expressions (C++17) ----
template<typename... Args> auto sum(Args... args)  { return (args + ...); }
template<typename... Args> auto prod(Args... args) { return (args * ...); }
template<typename... Args> bool allPos(Args... args) { return ((args > 0) && ...); }
template<typename... Args> void cetakSemua(Args... args) { ((cout << "  " << args << "\n"), ...); }

// ---- Tuple Iteration ----
template<int I, typename... T>
void printTupleImpl(const tuple<T...>& t) {
    if constexpr (I < (int)sizeof...(T)) {
        if (I > 0) cout << ", ";
        cout << get<I>(t);
        printTupleImpl<I+1>(t);
    }
}
template<typename... T>
void printTuple(const tuple<T...>& t) {
    cout << "("; printTupleImpl<0>(t); cout << ")\n";
}

// ---- Static Array dengan template non-tipe ----
template<typename T, int N>
struct StaticArray {
    array<T, N> data{};
    T& operator[](int i) { return data[i]; }
    constexpr int ukuran() const { return N; }
    void cetak() const {
        cout << "[";
        for (int i = 0; i < N; i++) {
            cout << data[i];
            if (i < N-1) cout << ",";
        }
        cout << "]\n";
    }
    T jumlah() const { return accumulate(data.begin(), data.end(), T(0)); }
};

// ---- Tag Dispatch ----
struct TagInteger  {};
struct TagFloat    {};
template<typename T> struct TypeTag { using type = TagFloat; };
template<> struct TypeTag<int>   { using type = TagInteger; };
template<> struct TypeTag<long>  { using type = TagInteger; };

template<typename T>
void prosesImpl(T v, TagInteger) { cout << "  Integer: " << v << " [binary: "; for(int i=7;i>=0;i--) cout<<((v>>i)&1); cout<<"]\n"; }
template<typename T>
void prosesImpl(T v, TagFloat)   { cout << "  Float: " << v << " [floor=" << (int)v << "]\n"; }
template<typename T>
void proses(T v) { prosesImpl(v, typename TypeTag<T>::type{}); }

int main() {
    // ---- Compile-time values ----
    cout << "=== Compile-Time Computation ===" << endl;
    cout << "Faktorial<10> = " << Faktorial<10>::nilai << "\n";
    cout << "faktorial(10) = " << faktorial(10) << "\n";
    cout << "Fib<20>       = " << Fib<20>::nilai << "\n";
    cout << "GCD<48,18>    = " << GCD<48,18>::nilai << "\n";
    cout << "LCM<4,6>      = " << LCM<4,6>::nilai << "\n";

    cout << "\nBilangan Prima (compile-time check):\n";
    cout << "  IsPrime<2>  = " << boolalpha << IsPrime<2>::nilai << "\n";
    cout << "  IsPrime<7>  = " << IsPrime<7>::nilai << "\n";
    cout << "  IsPrime<9>  = " << IsPrime<9>::nilai << "\n";
    cout << "  IsPrime<17> = " << IsPrime<17>::nilai << "\n";

    // ---- Type Traits ----
    cout << "\n=== Type Traits ===" << endl;
    infoTipe<int>("int");
    infoTipe<double>("double");
    infoTipe<unsigned char>("unsigned char");

    // ---- SFINAE ----
    cout << "\n=== SFINAE (rata-rata) ===" << endl;
    vector<int>    vi = {1,2,3,4,5};
    vector<double> vd = {1.5,2.5,3.5};
    cout << "  rata-rata int:    " << rataRata(vi) << "\n";
    cout << "  rata-rata double: " << rataRata(vd) << "\n";

    // ---- if constexpr ----
    cout << "\n=== if constexpr ===" << endl;
    cetak(42);
    cetak(3.14);
    cetak(string("Halo"));

    // ---- Fold Expressions ----
    cout << "\n=== Fold Expressions ===" << endl;
    cout << "  sum(1,2,3,4,5)  = " << sum(1,2,3,4,5) << "\n";
    cout << "  prod(1,2,3,4,5) = " << prod(1,2,3,4,5) << "\n";
    cout << "  allPos(1,2,3)   = " << allPos(1,2,3) << "\n";
    cout << "  allPos(1,-1,3)  = " << allPos(1,-1,3) << "\n";
    cout << "  cetakSemua:\n"; cetakSemua(1, "dua", 3.0, 'x');

    // ---- Tuple ----
    cout << "\n=== Tuple Iteration ===" << endl;
    auto t = make_tuple(42, "Halo", 3.14, true);
    cout << "  Tuple: "; printTuple(t);
    cout << "  Size: " << tuple_size_v<decltype(t)> << "\n";

    // ---- StaticArray ----
    cout << "\n=== StaticArray<T,N> ===" << endl;
    StaticArray<int, 5> arr;
    for (int i = 0; i < arr.ukuran(); i++) arr[i] = (i+1) * (i+1);
    arr.cetak();
    cout << "  jumlah = " << arr.jumlah() << "\n";

    StaticArray<double, 3> arrf;
    arrf[0]=1.5; arrf[1]=2.5; arrf[2]=3.5;
    arrf.cetak();

    // ---- Tag Dispatch ----
    cout << "\n=== Tag Dispatch ===" << endl;
    proses(255);
    proses(3.14159);
    proses(42L);

    return 0;
}
