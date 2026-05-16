// ============================================================
// File  : matematika.h
// Topik : Header library matematika — akan dikompilasi static
// ============================================================
#pragma once
#include <string>
#include <vector>

namespace Matematika {

// Operasi dasar
double tambah(double a, double b);
double kurang(double a, double b);
double kali(double a, double b);
double bagi(double a, double b);  // throws std::invalid_argument jika b==0

// Statistik
double rata2(const std::vector<double>& data);
double median(std::vector<double> data);          // sort dalam, perlu copy
double standarDeviasi(const std::vector<double>& data);
double min(const std::vector<double>& data);
double max(const std::vector<double>& data);

// Bilangan
bool  adalahPrima(long long n);
long long faktorial(int n);         // throws jika n < 0 atau n > 20
long long fibonacci(int n);         // throws jika n < 0
long long gcd(long long a, long long b);
long long lcm(long long a, long long b);

// Konversi
double derajatKeRadian(double derajat);
double radianKeDerjat(double radian);

// Info library
std::string versi();

} // namespace Matematika
