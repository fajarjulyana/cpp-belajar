# Database: SQLite3

SQLite adalah database yang **tertanam langsung di aplikasi** — tidak perlu server terpisah. File database adalah satu file `.db`.

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install libsqlite3-dev sqlite3

# Fedora
sudo dnf install sqlite-devel sqlite

# macOS (sudah termasuk di macOS)
brew install sqlite

# Windows — download sqlite-amalgamation dari https://sqlite.org/download.html
# Atau MSYS2: pacman -S mingw-w64-x86_64-sqlite3
```

---

## Kompilasi

```bash
g++ -std=c++17 -Wall -o sqlite_demo sqlite_demo.cpp -lsqlite3
```

---

## API Dasar SQLite3

```cpp
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

sqlite3* db;
int rc;

// 1. Buka database
rc = sqlite3_open("data.db", &db);
if (rc != SQLITE_OK) {
    cerr << "Error: " << sqlite3_errmsg(db) << "\n";
    return;
}

// 2. Eksekusi SQL sederhana (tanpa hasil)
const char* sql = "CREATE TABLE IF NOT EXISTS mahasiswa("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "nama TEXT NOT NULL,"
                  "nilai REAL,"
                  "aktif INTEGER DEFAULT 1);";
char* errMsg = nullptr;
rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
if (rc != SQLITE_OK) {
    cerr << "SQL Error: " << errMsg << "\n";
    sqlite3_free(errMsg);
}

// 3. Prepared statement (WAJIB untuk data user — mencegah SQL injection!)
const char* insertSQL = "INSERT INTO mahasiswa(nama, nilai) VALUES (?, ?);";
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);

sqlite3_bind_text(stmt, 1, "Budi Santoso", -1, SQLITE_STATIC);
sqlite3_bind_double(stmt, 2, 85.5);
sqlite3_step(stmt);
sqlite3_finalize(stmt);

// 4. Query
const char* selectSQL = "SELECT id, nama, nilai FROM mahasiswa ORDER BY nilai DESC;";
rc = sqlite3_prepare_v2(db, selectSQL, -1, &stmt, nullptr);
while (sqlite3_step(stmt) == SQLITE_ROW) {
    int    id    = sqlite3_column_int(stmt, 0);
    const char* nama = (const char*)sqlite3_column_text(stmt, 1);
    double nilai = sqlite3_column_double(stmt, 2);
    cout << id << " | " << nama << " | " << nilai << "\n";
}
sqlite3_finalize(stmt);

// 5. Tutup database
sqlite3_close(db);
```

---

## Tipe Data SQLite

| SQLite | C++ |
|--------|-----|
| `INTEGER` | `int`, `long long` |
| `REAL` | `double` |
| `TEXT` | `string`, `const char*` |
| `BLOB` | `void*`, `vector<uint8_t>` |
| `NULL` | `nullptr` |

---

## Transaksi

```cpp
// Transaksi untuk operasi batch (jauh lebih cepat!)
sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

// ... insert banyak baris ...
for (int i = 0; i < 10000; i++) {
    sqlite3_bind_text(stmt, 1, ("Mahasiswa-" + to_string(i)).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, 70.0 + (i % 30));
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
}

sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
// Rollback jika ada error:
// sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
```

---

## Callback Style Query

```cpp
int callback(void* data, int kolom, char** nilai, char** namaKolom) {
    for (int i = 0; i < kolom; i++)
        cout << namaKolom[i] << ": " << (nilai[i] ? nilai[i] : "NULL") << "  ";
    cout << "\n";
    return 0;
}

sqlite3_exec(db, "SELECT * FROM mahasiswa;", callback, nullptr, &errMsg);
```

---

## Latihan

1. Buat database sistem perpustakaan: buku, anggota, peminjaman
2. Implementasikan CRUD (Create, Read, Update, Delete) untuk satu tabel
3. Buat sistem login sederhana dengan tabel `users` (username, password_hash)
4. Gunakan transaksi untuk import 10000 data dan bandingkan performa

---

**[← Win32 API](../../04-gui/05-win32api/README.md)** | **[Berikutnya → PostgreSQL](../02-postgresql/README.md)**
