// ============================================================
// File  : matematika.cpp
// Topik : Implementasi library matematika (akan menjadi .a)
// ============================================================
#include "matematika.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace Matematika {

double tambah(double a, double b) { return a + b; }
double kurang(double a, double b) { return a - b; }
double kali(double a, double b)   { return a * b; }

double bagi(double a, double b) {
    if (b == 0.0)
        throw std::invalid_argument("Pembagi tidak boleh nol");
    return a / b;
}

double rata2(const std::vector<double>& data) {
    if (data.empty()) throw std::invalid_argument("Data kosong");
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double median(std::vector<double> data) {
    if (data.empty()) throw std::invalid_argument("Data kosong");
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    if (n % 2 == 0)
        return (data[n/2 - 1] + data[n/2]) / 2.0;
    return data[n/2];
}

double standarDeviasi(const std::vector<double>& data) {
    if (data.size() < 2) throw std::invalid_argument("Butuh minimal 2 data");
    double m = rata2(data);
    double jumlah = 0;
    for (double x : data) jumlah += (x - m) * (x - m);
    return std::sqrt(jumlah / data.size());
}

double min(const std::vector<double>& data) {
    if (data.empty()) throw std::invalid_argument("Data kosong");
    return *std::min_element(data.begin(), data.end());
}

double max(const std::vector<double>& data) {
    if (data.empty()) throw std::invalid_argument("Data kosong");
    return *std::max_element(data.begin(), data.end());
}

bool adalahPrima(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

long long faktorial(int n) {
    if (n < 0) throw std::invalid_argument("Faktorial tidak terdefinisi untuk negatif");
    if (n > 20) throw std::overflow_error("Faktorial > 20 overflow long long");
    long long hasil = 1;
    for (int i = 2; i <= n; ++i) hasil *= i;
    return hasil;
}

long long fibonacci(int n) {
    if (n < 0) throw std::invalid_argument("Fibonacci tidak terdefinisi untuk negatif");
    if (n == 0) return 0;
    if (n == 1) return 1;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) { long long c = a + b; a = b; b = c; }
    return b;
}

long long gcd(long long a, long long b) {
    while (b) { a %= b; std::swap(a, b); }
    return a;
}

long long lcm(long long a, long long b) {
    return (a / gcd(a, b)) * b;
}

double derajatKeRadian(double d) { return d * M_PI / 180.0; }
double radianKeDerjat(double r)  { return r * 180.0 / M_PI; }

std::string versi() { return "Matematika Library v1.0.0 (static)"; }

} // namespace Matematika
