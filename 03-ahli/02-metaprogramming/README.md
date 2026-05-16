# Bab 20 — Template Metaprogramming

Template Metaprogramming (TMP) adalah teknik di mana **program menghasilkan atau memanipulasi program lain saat kompilasi**.

---

## Type Traits

```cpp
#include <type_traits>

// Cek tipe saat kompilasi
static_assert(is_integral_v<int>);
static_assert(is_floating_point_v<double>);
static_assert(is_same_v<int, int>);
static_assert(!is_same_v<int, double>);

// Transformasi tipe
using ConstInt = add_const_t<int>;       // const int
using IntPtr   = add_pointer_t<int>;     // int*
using BaseInt  = remove_const_t<const int>; // int
using BasePtr  = remove_pointer_t<int*>; // int

// Kondisional
using T = conditional_t<is_integral_v<int>, string, double>;
// T = string (karena int adalah integral)
```

---

## Compile-Time Computation

```cpp
// Faktorial saat kompilasi
template<int N>
struct Faktorial {
    static constexpr long long nilai = N * Faktorial<N-1>::nilai;
};
template<> struct Faktorial<0> { static constexpr long long nilai = 1; };

// GCD saat kompilasi
template<int A, int B>
struct GCD { static constexpr int nilai = GCD<B, A%B>::nilai; };
template<int A>
struct GCD<A, 0> { static constexpr int nilai = A; };

// Cek bilangan prima saat kompilasi
template<int N, int D = N-1>
struct IsPrime {
    static constexpr bool nilai = (N % D != 0) && IsPrime<N, D-1>::nilai;
};
template<int N>
struct IsPrime<N, 1> { static constexpr bool nilai = (N > 1); };
```

---

## `constexpr` Functions (C++11/14/17)

```cpp
// constexpr: dievaluasi saat kompilasi ATAU runtime
constexpr long long faktorial(int n) {
    return n <= 1 ? 1 : n * faktorial(n-1);
}

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

// Gunakan saat kompilasi
constexpr auto f10 = faktorial(10);  // 3628800 — tidak ada runtime cost
static_assert(faktorial(5) == 120);

// Gunakan saat runtime
int n; cin >> n;
cout << faktorial(n) << "\n";  // dihitung saat runtime
```

---

## SFINAE (Substitution Failure Is Not An Error)

```cpp
#include <type_traits>

// Hanya untuk tipe numerik
template<typename T>
typename enable_if<is_arithmetic_v<T>, double>::type
rataRata(vector<T>& v) {
    double sum = 0;
    for (auto& x : v) sum += x;
    return sum / v.size();
}

// C++17 — if constexpr (lebih bersih dari SFINAE)
template<typename T>
void proses(T val) {
    if constexpr (is_integral_v<T>) {
        cout << "Integer: " << val << " (hex: " << hex << val << dec << ")\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float: " << fixed << val << "\n";
    } else if constexpr (is_same_v<T, string>) {
        cout << "String: \"" << val << "\" (len=" << val.length() << ")\n";
    } else {
        cout << "Tipe lain\n";
    }
}
```

---

## Concepts (C++20)

```cpp
#include <concepts>

// Definisi concept
template<typename T>
concept Numerik = is_arithmetic_v<T>;

template<typename T>
concept Kontainer = requires(T c) {
    c.begin();
    c.end();
    c.size();
};

template<typename T>
concept Printable = requires(T t, ostream& os) {
    os << t;
};

// Gunakan concept
template<Numerik T>
T tambah(T a, T b) { return a + b; }

template<Kontainer C>
void cetakSemua(const C& c) {
    for (const auto& x : c) cout << x << " ";
    cout << "\n";
}
```

---

## Variadic Templates Lanjut

```cpp
// Tuple custom
template<typename... Types>
struct Tuple {};

template<typename First, typename... Rest>
struct Tuple<First, Rest...> : Tuple<Rest...> {
    First nilai;
    Tuple(First f, Rest... r) : Tuple<Rest...>(r...), nilai(f) {}
};

// Print tuple
template<int I, typename... T>
void printTupleImpl(const tuple<T...>& t) {
    if constexpr (I < sizeof...(T)) {
        cout << get<I>(t);
        if constexpr (I + 1 < sizeof...(T)) cout << ", ";
        printTupleImpl<I+1>(t);
    }
}

template<typename... T>
void printTuple(const tuple<T...>& t) {
    cout << "(";
    printTupleImpl<0>(t);
    cout << ")\n";
}

// Fold Expressions (C++17)
template<typename... Args>
auto sum(Args... args) { return (args + ...); }  // fold expression

template<typename... Args>
bool allTrue(Args... args) { return (args && ...); }  // AND fold

template<typename... Args>
void cetakSemua(Args... args) { ((cout << args << " "), ...); cout << "\n"; }
```

---

## Latihan

1. Implementasikan `type_list` untuk menyimpan daftar tipe saat kompilasi
2. Buat `StaticArray` yang menggunakan nilai template non-tipe untuk ukuran
3. Implementasikan `FunctionTraits` yang mengekstrak return type dan parameter types dari function pointer
4. Buat `constexpr` untuk mengurutkan array saat kompilasi

---

**[← Design Patterns](../01-design-patterns/README.md)** | **[Berikutnya → Concurrency Lanjut](../03-concurrency/README.md)**
