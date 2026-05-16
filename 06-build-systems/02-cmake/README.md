# Build Systems: CMake

CMake adalah sistem build lintas platform yang paling banyak digunakan untuk proyek C++ modern. CMake menghasilkan file build native (Makefile, Visual Studio solution, Ninja, dll.) dari satu file `CMakeLists.txt`.

---

## Cara Kerja CMake

```
CMakeLists.txt
     │
     ▼
cmake (configure) ──→ Makefile / .sln / build.ninja
     │
     ▼
make / msbuild / ninja (build)
     │
     ▼
Executable / Library
```

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install cmake build-essential

# Fedora
sudo dnf install cmake gcc-c++

# macOS
brew install cmake

# Windows
# Download installer: https://cmake.org/download/
# Atau winget: winget install Kitware.CMake

# Cek versi
cmake --version
```

---

## CMakeLists.txt Minimal

```cmake
cmake_minimum_required(VERSION 3.16)
project(HelloCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello main.cpp)
```

**Cara build:**
```bash
cmake -B build         # Configure → buat folder build/
cmake --build build    # Compile
./build/hello          # Jalankan
```

---

## CMakeLists.txt Standar Proyek

```cmake
# ============================================================
# CMakeLists.txt — Proyek C++ Standar
# ============================================================
cmake_minimum_required(VERSION 3.20)

project(MyApp
    VERSION     1.2.0
    DESCRIPTION "Aplikasi C++ belajar"
    LANGUAGES   CXX
)

# ── Standar C++ ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Build type default: Release ───────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ── Warning flags ─────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Sumber file ───────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# ── Target executable ─────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES})

# ── Include directories ───────────────────────────────────────
target_include_directories(${PROJECT_NAME}
    PRIVATE include/
)

# ── Compile definitions ───────────────────────────────────────
target_compile_definitions(${PROJECT_NAME}
    PRIVATE APP_VERSION="${PROJECT_VERSION}"
)

# ── Install rules ─────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# ── Info diagnostik ───────────────────────────────────────────
message(STATUS "Project   : ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler  : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
```

---

## Struktur Proyek Multi-File

```
MyApp/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── kalkulator.cpp
│   └── database.cpp
├── include/
│   ├── kalkulator.h
│   └── database.h
└── build/           ← dibuat oleh cmake, jangan di-commit
```

```cmake
# Tambahkan semua file .cpp di src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(MyApp ${SOURCES})
target_include_directories(MyApp PRIVATE include/)
```

---

## Library (Static & Shared)

```cmake
# Static library
add_library(Matematika STATIC
    src/matematika.cpp
    src/trigonometri.cpp
)
target_include_directories(Matematika PUBLIC include/)

# Shared library (.so / .dll)
add_library(Matematika SHARED
    src/matematika.cpp
)
set_target_properties(Matematika PROPERTIES
    VERSION   1.0.0
    SOVERSION 1
)

# Link library ke executable
add_executable(App main.cpp)
target_link_libraries(App PRIVATE Matematika)
```

---

## Proyek Multi-Target (Sub-directory)

```
MyProject/
├── CMakeLists.txt        ← root
├── lib/
│   ├── CMakeLists.txt    ← library
│   └── src/
├── app/
│   ├── CMakeLists.txt    ← executable
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt    ← tests
    └── test_main.cpp
```

```cmake
# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(MyProject LANGUAGES CXX)

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tests)
```

```cmake
# lib/CMakeLists.txt
add_library(mylib STATIC src/mylib.cpp)
target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# app/CMakeLists.txt
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE mylib)
```

---

## Mencari Dependensi Eksternal

```cmake
# 1. Package yang mendukung CMake modern (find_package)
find_package(OpenSSL REQUIRED)
target_link_libraries(MyApp PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# 2. pkg-config (untuk library lama)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
target_include_directories(MyApp PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(MyApp PRIVATE ${GTK3_LIBRARIES})

# 3. Manual find_library
find_library(SQLITE3_LIB sqlite3 REQUIRED)
target_link_libraries(MyApp PRIVATE ${SQLITE3_LIB})

# 4. FetchContent — download otomatis dari internet
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)
target_link_libraries(MyApp PRIVATE nlohmann_json::nlohmann_json)
```

---

## Testing dengan CTest

```cmake
# Di CMakeLists.txt
enable_testing()

add_executable(test_kalkulator tests/test_kalkulator.cpp)
target_link_libraries(test_kalkulator PRIVATE mylib)

add_test(NAME TestKalkulator COMMAND test_kalkulator)
add_test(NAME TestIntegrasi  COMMAND myapp --self-test)
```

```bash
# Jalankan semua test
cd build
ctest --output-on-failure

# Jalankan test tertentu
ctest -R TestKalkulator -V
```

---

## Build Type & Konfigurasi

```bash
# Debug build (dengan debug info)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release build (optimasi penuh)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -- -j$(nproc)  # paralel

# RelWithDebInfo (release + info debug)
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

```cmake
# Dalam CMakeLists.txt — beda flag per build type
target_compile_options(MyApp PRIVATE
    $<$<CONFIG:Debug>:   -g -O0 -DDEBUG -fsanitize=address>
    $<$<CONFIG:Release>: -O3 -DNDEBUG -march=native>
)
```

---

## Cross-Compile & Toolchain

```cmake
# toolchain-arm.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

```bash
cmake -B build-arm -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake
cmake --build build-arm
```

---

## CMake Generator (IDE)

```bash
# Ninja (lebih cepat dari Make)
cmake -B build -G Ninja
ninja -C build

# Visual Studio 2022 (Windows)
cmake -B build -G "Visual Studio 17 2022" -A x64

# Xcode (macOS)
cmake -B build -G Xcode

# CodeBlocks
cmake -B build -G "CodeBlocks - Unix Makefiles"
```

---

## CPack — Packaging & Distribusi

```cmake
include(CPack)

set(CPACK_PACKAGE_NAME        "MyApp")
set(CPACK_PACKAGE_VERSION     "${PROJECT_VERSION}")
set(CPACK_GENERATOR           "DEB;RPM;ZIP")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "developer@example.com")
```

```bash
cd build
cpack           # buat semua package
cpack -G DEB    # hanya DEB
```

---

## Latihan

1. Buat CMakeLists.txt untuk proyek dengan 3 file `.cpp` dan 1 library statis
2. Tambahkan `find_package(SQLite3)` dan link ke executable
3. Buat konfigurasi debug yang mengaktifkan AddressSanitizer
4. Gunakan `FetchContent` untuk download nlohmann/json dan pakai di proyek

---

**[← Makefile](../01-makefile/README.md)** | **[Berikutnya → CLion](../03-clion/README.md)**
