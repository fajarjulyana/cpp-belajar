// ============================================================
// File  : utilitas.h
// Topik : Header library utilitas — akan dikompilasi dynamic (.so)
// ============================================================
#pragma once
#include <string>
#include <vector>
#include <map>

// Macro agar simbol terekspos di shared library (Linux/macOS)
#ifdef _WIN32
  #define EKSPOR __declspec(dllexport)
#else
  #define EKSPOR __attribute__((visibility("default")))
#endif

namespace Utilitas {

// ── String ──
EKSPOR std::string hurufBesar(const std::string& s);
EKSPOR std::string hurufKecil(const std::string& s);
EKSPOR std::string balik(const std::string& s);
EKSPOR bool        palindrom(const std::string& s);
EKSPOR int         hitungKata(const std::string& s);
EKSPOR std::vector<std::string> pisah(const std::string& s, char pemisah);
EKSPOR std::string gabung(const std::vector<std::string>& v, const std::string& sep);
EKSPOR std::string formatRibu(long long n);     // 1000000 → "1.000.000"

// ── Waktu ──
EKSPOR std::string waktuSekarang();             // "14:35:22"
EKSPOR std::string tanggalSekarang();           // "2025-05-16"
EKSPOR long long   timestampSekarang();         // Unix timestamp ms

// ── Validasi ──
EKSPOR bool validEmail(const std::string& email);
EKSPOR bool validAngka(const std::string& s);
EKSPOR bool validURL(const std::string& url);

// ── Hash sederhana ──
EKSPOR std::string hash32(const std::string& s);  // FNV-1a 32-bit (hex string)

// ── Info library ──
EKSPOR std::string versi();

} // namespace Utilitas
