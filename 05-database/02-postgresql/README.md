# Database: PostgreSQL dengan libpq

PostgreSQL adalah database relasional enterprise-grade. Libpq adalah library C resmi untuk koneksi dari C/C++.

---

## Instalasi

```bash
# Ubuntu/Debian
sudo apt install libpq-dev postgresql postgresql-client

# Fedora
sudo dnf install libpq-devel postgresql postgresql-server

# macOS
brew install postgresql libpq

# Windows
# Download dari: https://www.postgresql.org/download/windows/
# Atau MSYS2: pacman -S mingw-w64-x86_64-postgresql
```

---

## Setup Database

```bash
# Jalankan service PostgreSQL
sudo systemctl start postgresql   # Linux
brew services start postgresql    # macOS

# Buat user dan database
sudo -u postgres psql
CREATE USER devuser WITH PASSWORD 'rahasia';
CREATE DATABASE devdb OWNER devuser;
GRANT ALL PRIVILEGES ON DATABASE devdb TO devuser;
\q

# Variabel environment koneksi
export PGHOST=localhost
export PGPORT=5432
export PGDATABASE=devdb
export PGUSER=devuser
export PGPASSWORD=rahasia
```

---

## Kompilasi

```bash
g++ -std=c++17 -Wall -o pg_demo postgres_demo.cpp -lpq

# Atau dengan pg_config
g++ -std=c++17 -o pg_demo postgres_demo.cpp \
    $(pg_config --includedir | xargs -I{} echo -I{}) \
    $(pg_config --libdir | xargs -I{} echo -L{}) -lpq
```

---

## Pola Koneksi & Query

```cpp
#include <libpq-fe.h>
#include <iostream>
using namespace std;

// 1. Koneksi
const char* conninfo = "host=localhost dbname=devdb user=devuser password=rahasia";
PGconn* conn = PQconnectdb(conninfo);

if (PQstatus(conn) != CONNECTION_OK) {
    cerr << "Koneksi gagal: " << PQerrorMessage(conn);
    PQfinish(conn);
    return 1;
}

// 2. Eksekusi query sederhana
PGresult* res = PQexec(conn,
    "CREATE TABLE IF NOT EXISTS produk("
    "  id SERIAL PRIMARY KEY,"
    "  nama TEXT NOT NULL,"
    "  harga NUMERIC(12,2),"
    "  stok INTEGER DEFAULT 0)");
if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    cerr << "Error: " << PQerrorMessage(conn);
}
PQclear(res);

// 3. Query dengan parameterized (cegah SQL injection!)
const char* sql = "INSERT INTO produk(nama, harga, stok) VALUES($1, $2, $3) RETURNING id";
const char* params[3] = {"Laptop ASUS", "12500000.00", "10"};
res = PQexecParams(conn, sql, 3, nullptr, params, nullptr, nullptr, 0);
if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    cout << "ID baru: " << PQgetvalue(res, 0, 0) << "\n";
}
PQclear(res);

// 4. SELECT dan iterasi
res = PQexec(conn, "SELECT id, nama, harga, stok FROM produk ORDER BY nama");
if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        cout << PQgetvalue(res, i, 0) << " | "  // id
             << PQgetvalue(res, i, 1) << " | "  // nama
             << PQgetvalue(res, i, 2) << " | "  // harga
             << PQgetvalue(res, i, 3) << "\n";  // stok
    }
}
PQclear(res);

// 5. Tutup koneksi
PQfinish(conn);
```

---

## Transaksi

```cpp
PQexec(conn, "BEGIN");

res = PQexecParams(conn, "UPDATE akun SET saldo=saldo-$1 WHERE id=$2",
                   2, nullptr, new const char*{"100000", "1"}, nullptr, nullptr, 0);
if (PQresultStatus(res) != PGRES_COMMAND_OK) { PQexec(conn, "ROLLBACK"); }

res = PQexecParams(conn, "UPDATE akun SET saldo=saldo+$1 WHERE id=$2",
                   2, nullptr, new const char*{"100000", "2"}, nullptr, nullptr, 0);
if (PQresultStatus(res) != PGRES_COMMAND_OK) { PQexec(conn, "ROLLBACK"); }
else { PQexec(conn, "COMMIT"); }
```

---

## Fitur PostgreSQL yang Berguna

```sql
-- JSON/JSONB
CREATE TABLE config (id SERIAL, data JSONB);
INSERT INTO config(data) VALUES ('{"tema": "gelap", "bahasa": "id"}');
SELECT data->>'tema' FROM config;

-- Array
CREATE TABLE tags (id SERIAL, nama TEXT, label TEXT[]);
INSERT INTO tags(nama, label) VALUES ('Post-1', ARRAY['cpp', 'tutorial', 'database']);
SELECT * FROM tags WHERE 'cpp' = ANY(label);

-- Full-text search
SELECT * FROM produk WHERE to_tsvector('indonesian', nama) @@ plainto_tsquery('laptop');

-- Window functions
SELECT nama, harga,
       RANK() OVER (ORDER BY harga DESC) AS peringkat,
       AVG(harga) OVER () AS avg_harga
FROM produk;
```

---

## Latihan

1. Buat schema toko online: produk, pelanggan, pesanan, detail_pesanan
2. Implementasikan transfer saldo antar akun dengan transaksi atomic
3. Buat laporan penjualan dengan GROUP BY, HAVING, dan window function
4. Gunakan JSONB untuk menyimpan metadata produk yang fleksibel

---

**[← SQLite](../01-sqlite/README.md)** | **[Berikutnya → MongoDB](../03-mongodb/README.md)**
