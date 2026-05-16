// ============================================================
// File  : utilitas.cpp
// Topik : Implementasi library utilitas (akan menjadi .so)
// ============================================================
#include "utilitas.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <ctime>
#include <chrono>
#include <regex>
#include <iomanip>
#include <stdexcept>

namespace Utilitas {

std::string hurufBesar(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return r;
}

std::string hurufKecil(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return r;
}

std::string balik(const std::string& s) {
    return std::string(s.rbegin(), s.rend());
}

bool palindrom(const std::string& s) {
    std::string clean;
    for (unsigned char c : s)
        if (std::isalnum(c)) clean += std::tolower(c);
    return clean == std::string(clean.rbegin(), clean.rend());
}

int hitungKata(const std::string& s) {
    std::istringstream ss(s);
    std::string kata;
    int n = 0;
    while (ss >> kata) ++n;
    return n;
}

std::vector<std::string> pisah(const std::string& s, char pemisah) {
    std::vector<std::string> hasil;
    std::istringstream ss(s);
    std::string token;
    while (std::getline(ss, token, pemisah)) hasil.push_back(token);
    return hasil;
}

std::string gabung(const std::vector<std::string>& v, const std::string& sep) {
    std::string out;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) out += sep;
        out += v[i];
    }
    return out;
}

std::string formatRibu(long long n) {
    bool negatif = n < 0;
    if (negatif) n = -n;
    std::string s = std::to_string(n);
    int pos = (int)s.size() - 3;
    while (pos > 0) { s.insert(pos, "."); pos -= 3; }
    return negatif ? "-" + s : s;
}

std::string waktuSekarang() {
    std::time_t t = std::time(nullptr);
    char buf[9];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
    return buf;
}

std::string tanggalSekarang() {
    std::time_t t = std::time(nullptr);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&t));
    return buf;
}

long long timestampSekarang() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

bool validEmail(const std::string& email) {
    std::regex pola(R"([a-zA-Z0-9._%+\-]+@[a-zA-Z0-9.\-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pola);
}

bool validAngka(const std::string& s) {
    if (s.empty()) return false;
    size_t awal = 0;
    if (s[0] == '-' || s[0] == '+') awal = 1;
    bool adaTitik = false;
    for (size_t i = awal; i < s.size(); ++i) {
        if (s[i] == '.') {
            if (adaTitik) return false;
            adaTitik = true;
        } else if (!std::isdigit((unsigned char)s[i])) {
            return false;
        }
    }
    return awal < s.size();
}

bool validURL(const std::string& url) {
    std::regex pola(R"(https?://[a-zA-Z0-9.\-]+(\.[a-zA-Z]{2,})(:[0-9]+)?(/[^\s]*)?)");
    return std::regex_match(url, pola);
}

std::string hash32(const std::string& s) {
    uint32_t h = 2166136261u;
    for (unsigned char c : s) {
        h ^= c;
        h *= 16777619u;
    }
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << h;
    return oss.str();
}

std::string versi() { return "Utilitas Library v1.0.0 (dynamic)"; }

} // namespace Utilitas
