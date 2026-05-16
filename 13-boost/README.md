# Boost C++ Libraries

## Apa itu Boost?

Boost adalah kumpulan library C++ berkualitas tinggi, peer-reviewed, dan portable. Banyak fitur Boost yang kemudian masuk ke standar C++ (filesystem, regex, optional, variant, dll).

## Instalasi

### Ubuntu/Debian
```bash
sudo apt install libboost-all-dev
```

### macOS
```bash
brew install boost
```

### Windows
Download dari https://www.boost.org/users/download/ atau gunakan vcpkg:
```bash
vcpkg install boost
```

## Library Boost yang Paling Populer

| Library | Header | Setara Standar | Keterangan |
|---------|--------|----------------|------------|
| **Filesystem** | `boost/filesystem.hpp` | `std::filesystem` (C++17) | Operasi file & direktori |
| **Regex** | `boost/regex.hpp` | `std::regex` (C++11) | Ekspresi reguler |
| **Asio** | `boost/asio.hpp` | — | Async networking & I/O |
| **Algorithm** | `boost/algorithm/string.hpp` | — | String utilities |
| **Optional** | `boost/optional.hpp` | `std::optional` (C++17) | Nilai opsional |
| **Variant** | `boost/variant.hpp` | `std::variant` (C++17) | Tagged union |
| **Signals2** | `boost/signals2.hpp` | — | Event/Observer pattern |
| **Lexical Cast** | `boost/lexical_cast.hpp` | — | Konversi tipe via string |
| **Format** | `boost/format.hpp` | `std::format` (C++20) | String formatting |
| **Log** | `boost/log/trivial.hpp` | — | Logging framework |

## Kompilasi dengan Boost

```bash
# Header-only (Filesystem, Algorithm, dll)
g++ -std=c++17 -o program main.cpp

# Dengan library (.so) — Boost.Regex, Boost.Log
g++ -std=c++17 -o program main.cpp -lboost_regex -lboost_log

# Boost.Asio (header-only, tapi butuh pthread)
g++ -std=c++17 -pthread -o program main.cpp
```

## Subfolder

| Folder | Isi |
|--------|-----|
| `01-filesystem/` | Operasi file, direktori, path |
| `02-asio/` | Async I/O dan networking |
| `03-algorithm/` | String algorithm, sorting, searching |

## Boost vs Standar C++

Jika sudah pakai C++17, banyak yang bisa diganti standar:

```cpp
// Boost (lama)                    // Standar C++17
boost::filesystem::path p("a.txt") → std::filesystem::path p("a.txt")
boost::optional<int> x             → std::optional<int> x
boost::variant<int,string> v       → std::variant<int,string> v
boost::regex r("\\d+")             → std::regex r("\\d+")
```

Gunakan Boost jika:
- Butuh fitur yang belum ada di standar (Asio, Signals2, Spirit)
- Perlu kompatibel dengan compiler lama (C++11/14)
- Butuh performa maksimal (Boost sering lebih cepat dari std)
