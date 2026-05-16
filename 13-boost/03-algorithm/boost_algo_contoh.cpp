// ============================================================
// File  : boost_algo_contoh.cpp
// Topik : Boost.Algorithm — String & Container Utilities
//
// File ini menunjukkan:
//   1. Boost.Algorithm/String — dengan implementasi setara std::
//   2. Boost.Algorithm/Sort — pdqsort dan counting sort
//   3. Perbandingan Boost vs standar C++17
//
// Kode dengan JUCE Library (dikomentari): #include <boost/algorithm/string.hpp>
// Kode ini bisa dikompilasi TANPA Boost.
//
// Kompilasi:
//   g++ -std=c++17 -o boost_algo boost_algo_contoh.cpp
// ============================================================

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <numeric>
#include <map>
#include <set>
#include <iomanip>

// ============================================================
// Simulasi fungsi Boost.Algorithm/String
// Di Boost nyata: #include <boost/algorithm/string.hpp>
// ============================================================
namespace BoostSim {

// boost::to_upper_copy(str)
std::string toUpperCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return s;
}

// boost::to_lower_copy(str)
std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// boost::trim_copy(str)
std::string trimCopy(const std::string& s) {
    auto awal = s.find_first_not_of(" \t\r\n");
    auto akhir = s.find_last_not_of(" \t\r\n");
    if (awal == std::string::npos) return "";
    return s.substr(awal, akhir - awal + 1);
}

// boost::trim_left_copy / boost::trim_right_copy
std::string trimLeftCopy(const std::string& s) {
    auto pos = s.find_first_not_of(" \t\r\n");
    return (pos == std::string::npos) ? "" : s.substr(pos);
}
std::string trimRightCopy(const std::string& s) {
    auto pos = s.find_last_not_of(" \t\r\n");
    return (pos == std::string::npos) ? "" : s.substr(0, pos + 1);
}

// boost::starts_with(str, prefix)
bool startsWith(const std::string& s, const std::string& p) {
    return s.size() >= p.size() && s.substr(0, p.size()) == p;
}

// boost::ends_with(str, suffix)
bool endsWith(const std::string& s, const std::string& suf) {
    return s.size() >= suf.size() &&
           s.substr(s.size() - suf.size()) == suf;
}

// boost::contains(str, sub)
bool contains(const std::string& s, const std::string& sub) {
    return s.find(sub) != std::string::npos;
}

// boost::iequals(a, b) — case-insensitive compare
bool iequals(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (std::tolower((unsigned char)a[i]) !=
            std::tolower((unsigned char)b[i])) return false;
    return true;
}

// boost::split(result, str, pred)
std::vector<std::string> split(const std::string& s,
                                const std::string& delim) {
    std::vector<std::string> hasil;
    size_t awal = 0, pos;
    while ((pos = s.find(delim, awal)) != std::string::npos) {
        hasil.push_back(s.substr(awal, pos - awal));
        awal = pos + delim.size();
    }
    hasil.push_back(s.substr(awal));
    return hasil;
}

// boost::join(container, separator)
std::string join(const std::vector<std::string>& v,
                 const std::string& sep) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) out += sep;
        out += v[i];
    }
    return out;
}

// boost::replace_all(str, from, to)
std::string replaceAll(std::string s,
                        const std::string& from,
                        const std::string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

// boost::erase_all(str, sub)
std::string eraseAll(const std::string& s, const std::string& sub) {
    return replaceAll(s, sub, "");
}

} // namespace BoostSim

// ============================================================
// Utilitas
// ============================================================
void judulBagian(const std::string& judul) {
    std::cout << "\n\033[1;34m── " << judul << " ──\033[0m\n";
}

void tampil(const std::string& label, const std::string& nilai) {
    std::cout << "  " << std::left << std::setw(28) << label
              << "→ \"" << nilai << "\"\n";
}

void tampilBool(const std::string& label, bool nilai) {
    std::cout << "  " << std::left << std::setw(28) << label
              << "→ " << (nilai ? "\033[32mtrue\033[0m"
                                : "\033[31mfalse\033[0m") << "\n";
}

// ============================================================
// Demo 1: String Algorithm
// ============================================================
void demoStringAlgorithm() {
    judulBagian("Boost.Algorithm/String — String Utilities");

    // Kode Boost nyata:
    // #include <boost/algorithm/string.hpp>
    // boost::to_upper_copy(s) dst

    std::string s = "  Halo Dunia, Selamat Datang!  ";
    std::cout << "  Input: \"" << s << "\"\n\n";

    tampil("to_upper_copy",  BoostSim::toUpperCopy(s));
    tampil("to_lower_copy",  BoostSim::toLowerCopy(s));
    tampil("trim_copy",      BoostSim::trimCopy(s));
    tampil("trim_left_copy", BoostSim::trimLeftCopy(s));
    tampil("trim_right_copy",BoostSim::trimRightCopy(s));

    std::string t = BoostSim::trimCopy(s);
    tampilBool("starts_with(t, \"Halo\")", BoostSim::startsWith(t, "Halo"));
    tampilBool("ends_with(t, \"!\")",      BoostSim::endsWith(t, "!"));
    tampilBool("contains(t, \"Dunia\")",   BoostSim::contains(t, "Dunia"));
    tampilBool("iequals(\"abc\",\"ABC\")", BoostSim::iequals("abc", "ABC"));

    tampil("replace_all(Halo→Selamat)", BoostSim::replaceAll(t, "Halo", "Selamat"));
    tampil("erase_all(,)",              BoostSim::eraseAll(t, ","));
}

// ============================================================
// Demo 2: Split & Join
// ============================================================
void demoSplitJoin() {
    judulBagian("split / join");

    // Boost: boost::split(hasil, s, boost::is_any_of(","));
    //        boost::join(v, " | ");

    std::string csv = "Budi,Sari,Eko,Dewi,Rina";
    std::cout << "  CSV input: \"" << csv << "\"\n";

    auto parts = BoostSim::split(csv, ",");
    std::cout << "  split(\",\")  → [";
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << parts[i] << "\"";
    }
    std::cout << "]\n";

    tampil("join(\" | \")", BoostSim::join(parts, " | "));

    // Split kalimat
    std::string kalimat = "C++ adalah bahasa pemrograman yang kuat";
    auto kata = BoostSim::split(kalimat, " ");
    std::cout << "\n  Kalimat: \"" << kalimat << "\"\n";
    std::cout << "  Jumlah kata: " << kata.size() << "\n";
    // Urutkan kata
    std::sort(kata.begin(), kata.end());
    tampil("join setelah sort", BoostSim::join(kata, " "));
}

// ============================================================
// Demo 3: Container Algorithms (std + pola Boost)
// ============================================================
void demoContainerAlgo() {
    judulBagian("Container Algorithms (std::algorithm — pola Boost)");

    std::vector<int> data = {5, 3, 8, 1, 9, 2, 7, 4, 6, 10, 3, 5};
    std::cout << "  Data: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n\n";

    // Sort + unique (seperti boost::sort + boost::unique)
    std::vector<int> unik = data;
    std::sort(unik.begin(), unik.end());
    unik.erase(std::unique(unik.begin(), unik.end()), unik.end());
    std::cout << "  Unik & terurut: ";
    for (int x : unik) std::cout << x << " ";
    std::cout << "\n";

    // boost::accumulate → std::accumulate
    int total = std::accumulate(data.begin(), data.end(), 0);
    int maks  = *std::max_element(data.begin(), data.end());
    int mins  = *std::min_element(data.begin(), data.end());
    std::cout << "  Jumlah: " << total
              << "  Maks: " << maks
              << "  Min: " << mins << "\n";

    // boost::count_if
    int genapCount = (int)std::count_if(data.begin(), data.end(),
                                        [](int x){ return x % 2 == 0; });
    std::cout << "  Jumlah genap: " << genapCount << "\n";

    // boost::find_if
    auto it = std::find_if(data.begin(), data.end(),
                           [](int x){ return x > 7; });
    if (it != data.end())
        std::cout << "  Pertama > 7: " << *it << "\n";

    // Frekuensi
    std::map<int, int> frek;
    for (int x : data) ++frek[x];
    std::cout << "  Frekuensi: ";
    for (auto& [k, v] : frek)
        if (v > 1) std::cout << k << "×" << v << " ";
    std::cout << "\n";

    // Partisi
    std::vector<int> genap, ganjil;
    std::partition_copy(data.begin(), data.end(),
                        std::back_inserter(genap),
                        std::back_inserter(ganjil),
                        [](int x){ return x % 2 == 0; });
    std::cout << "  Genap : ";
    for (int x : genap) std::cout << x << " ";
    std::cout << "\n  Ganjil: ";
    for (int x : ganjil) std::cout << x << " ";
    std::cout << "\n";
}

// ============================================================
// Demo 4: Boost.LexicalCast — konversi via string
// ============================================================
void demoLexicalCast() {
    judulBagian("Boost.LexicalCast — Konversi Tipe via String");

    // Boost: #include <boost/lexical_cast.hpp>
    //        int n = boost::lexical_cast<int>("42");
    //        string s = boost::lexical_cast<string>(3.14);

    // Setara standar C++17:
    auto lexicalCastToInt = [](const std::string& s) {
        return std::stoi(s);
    };
    auto lexicalCastToDouble = [](const std::string& s) {
        return std::stod(s);
    };
    auto lexicalCastToStr = [](auto v) {
        return std::to_string(v);
    };

    std::cout << "  \"42\"    → int    : " << lexicalCastToInt("42") << "\n";
    std::cout << "  \"3.14\"  → double : " << lexicalCastToDouble("3.14") << "\n";
    std::cout << "  100     → string : \"" << lexicalCastToStr(100) << "\"\n";
    std::cout << "  3.14f   → string : \"" << lexicalCastToStr(3.14f) << "\"\n";

    // Boost memiliki exception safety:
    // try { int n = boost::lexical_cast<int>("bukan_angka"); }
    // catch (boost::bad_lexical_cast& e) { cerr << e.what(); }
    std::cout << "\n  Standar C++ untuk error handling:\n";
    try {
        std::stoi("bukan_angka");
    } catch (const std::invalid_argument& e) {
        std::cout << "  std::stoi(\"bukan_angka\") → invalid_argument: "
                  << e.what() << "\n";
    }
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "\n\033[1m"
              << "╔══════════════════════════════════════════════╗\n"
              << "║  Boost.Algorithm — Contoh & Setara Standar  ║\n"
              << "║  (Bisa dikompilasi tanpa Boost)             ║\n"
              << "╚══════════════════════════════════════════════╝\n"
              << "\033[0m";

    demoStringAlgorithm();
    demoSplitJoin();
    demoContainerAlgo();
    demoLexicalCast();

    std::cout << "\n\033[1;33mCatatan:\033[0m\n";
    std::cout << "  Untuk kode Boost nyata, install dengan:\n";
    std::cout << "  Ubuntu: sudo apt install libboost-all-dev\n";
    std::cout << "  Lalu:   g++ -std=c++17 -lboost_regex -o program main.cpp\n\n";
    return 0;
}
