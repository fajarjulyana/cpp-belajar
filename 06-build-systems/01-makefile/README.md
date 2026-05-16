# Build Systems: Makefile

Makefile adalah sistem build tradisional Unix. Sederhana, cepat, tersedia di mana-mana.

---

## Konsep Dasar Makefile

```makefile
# Sintaks aturan
target: dependencies
\tcommand    # HARUS menggunakan TAB, bukan spasi!

# Contoh minimal
hello: main.cpp
\tg++ -o hello main.cpp
```

---

## Makefile Standar untuk Proyek C++

```makefile
# ============================================================
# Makefile: Proyek C++ Standar
# ============================================================
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS  =
LDLIBS   =

TARGET   = program
SRCDIR   = src
OBJDIR   = obj
INCDIR   = include

SRCS     = $(wildcard $(SRCDIR)/*.cpp)
OBJS     = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS     = $(OBJS:.o=.d)   # file dependency otomatis

# Aturan utama
all: $(TARGET)

$(TARGET): $(OBJS)
\t$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Kompilasi setiap .cpp → .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
\t$(CXX) $(CXXFLAGS) -I$(INCDIR) -MMD -MP -c $< -o $@

# Buat folder obj jika belum ada
$(OBJDIR):
\tmkdir -p $@

# Include dependency files
-include $(DEPS)

# Target utilitas
clean:
\trm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
\t./$(TARGET)

# Target phony (bukan file)
.PHONY: all clean run
```

---

## Variabel Otomatis

| Variabel | Arti |
|----------|------|
| `$@` | Nama target |
| `$<` | Dependency pertama |
| `$^` | Semua dependency |
| `$*` | Stem (bagian yang cocok dengan `%`) |
| `$?` | Dependency yang lebih baru dari target |

---

## Pattern Rules

```makefile
# % adalah wildcard
%.o: %.cpp
\t$(CXX) $(CXXFLAGS) -c $< -o $@

# Semua .cpp → .o di folder berbeda
build/%.o: src/%.cpp
\t$(CXX) $(CXXFLAGS) -c $< -o $@
```

---

## Fungsi Makefile

```makefile
SRCS   = $(wildcard src/*.cpp)          # daftar file .cpp
OBJS   = $(patsubst src/%.cpp,obj/%.o,$(SRCS))  # ganti path+ekstensi
NAMES  = $(notdir $(SRCS))              # hanya nama file
DIRS   = $(sort $(dir $(SRCS)))         # direktori unik
UPPER  = $(shell echo $(VAR) | tr a-z A-Z)  # jalankan shell
```

---

## Makefile Multi-Target

```makefile
BINDIR = bin

PROGRAMS = kalkulator todo manajemen

all: $(PROGRAMS)

kalkulator: bin/kalkulator
todo:       bin/todo
manajemen:  bin/manajemen

bin/%: src/%.cpp | bin
\t$(CXX) $(CXXFLAGS) -o $@ $<

bin:
\tmkdir -p bin

clean:
\trm -rf bin

.PHONY: all clean $(PROGRAMS)
```

---

## Dependency Otomatis (Modern)

```makefile
# -MMD: buat file .d yang berisi dependency header
# -MP:  tambahkan target dummy untuk header (hindari error jika header dihapus)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
\t$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)  # load semua file .d
```

---

## Latihan

1. Buat Makefile untuk proyek dengan 3 file .cpp dan 2 header
2. Tambahkan target `test`, `install`, dan `uninstall`
3. Tambahkan support debug (`make debug`) dengan flag `-g -O0 -DDEBUG`
4. Buat Makefile yang bisa cross-compile untuk ARM

---

**[← MongoDB](../../05-database/03-mongodb/README.md)** | **[Berikutnya → CMake](../02-cmake/README.md)**
