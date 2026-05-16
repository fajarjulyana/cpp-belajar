cmake_minimum_required(VERSION 3.16)
project(HelloQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aktifkan Qt AUTOMOC (penting untuk signal/slot)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Cari Qt6 dulu, lalu fallback ke Qt5
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets)
if (Qt6_FOUND)
    set(QT_FOUND TRUE)
    set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)
    message(STATUS "Menggunakan Qt6")
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_FOUND TRUE)
    set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
    message(STATUS "Menggunakan Qt5")
endif()

add_executable(hello_qt hello_qt.cpp)
target_link_libraries(hello_qt PRIVATE ${QT_LIBS})

# Khusus Windows: sembunyikan console
if (WIN32)
    set_target_properties(hello_qt PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

message(STATUS "Build: mkdir build && cd build && cmake .. && cmake --build .")
