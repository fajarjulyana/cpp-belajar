// ============================================================
// File  : sqlite_demo.cpp
// Topik : Database SQLite3 dalam C++
// Kompilasi: g++ -std=c++17 -Wall -o sqlite_demo sqlite_demo.cpp -lsqlite3
// ============================================================
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
using namespace std;
using namespace chrono;

// ---- Wrapper C++ untuk SQLite ----
class Database {
    sqlite3* db = nullptr;
public:
    Database(const string& file) {
        if (sqlite3_open(file.c_str(), &db) != SQLITE_OK)
            throw runtime_error("Gagal buka DB: " + string(sqlite3_errmsg(db)));
        exec("PRAGMA journal_mode=WAL;");  // tulis lebih cepat
        exec("PRAGMA foreign_keys=ON;");
    }
    ~Database() { if (db) sqlite3_close(db); }

    void exec(const string& sql) {
        char* err = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
            string msg = err ? err : "unknown";
            sqlite3_free(err);
            throw runtime_error("SQL Error: " + msg + "\n  SQL: " + sql);
        }
    }

    sqlite3_stmt* prepare(const string& sql) {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
            throw runtime_error("Prepare error: " + string(sqlite3_errmsg(db)));
        return stmt;
    }

    long long lastInsertId() { return sqlite3_last_insert_rowid(db); }
    int       changesCount() { return sqlite3_changes(db); }
    sqlite3*  raw()          { return db; }
};

// ---- Struct data ----
struct Mahasiswa {
    int    id = 0;
    string nama, prodi;
    double ipk = 0.0;
    int    angkatan = 0;
};

struct MataKuliah {
    int    id = 0;
    string kode, nama;
    int    sks = 0;
};

// ---- CRUD Mahasiswa ----
class MahasiswaRepo {
    Database& db;
public:
    MahasiswaRepo(Database& d) : db(d) {
        db.exec(R"(
            CREATE TABLE IF NOT EXISTS mahasiswa (
                id       INTEGER PRIMARY KEY AUTOINCREMENT,
                nama     TEXT    NOT NULL,
                prodi    TEXT    NOT NULL,
                ipk      REAL    DEFAULT 0.0,
                angkatan INTEGER NOT NULL
            );
        )");
    }

    Mahasiswa insert(const Mahasiswa& m) {
        auto* s = db.prepare("INSERT INTO mahasiswa(nama,prodi,ipk,angkatan) VALUES(?,?,?,?)");
        sqlite3_bind_text(s, 1, m.nama.c_str(),  -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(s, 2, m.prodi.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(s, 3, m.ipk);
        sqlite3_bind_int(s, 4, m.angkatan);
        sqlite3_step(s);
        sqlite3_finalize(s);
        Mahasiswa r = m; r.id = (int)db.lastInsertId();
        return r;
    }

    void update(const Mahasiswa& m) {
        auto* s = db.prepare("UPDATE mahasiswa SET nama=?,prodi=?,ipk=?,angkatan=? WHERE id=?");
        sqlite3_bind_text(s,   1, m.nama.c_str(),  -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(s,   2, m.prodi.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(s, 3, m.ipk);
        sqlite3_bind_int(s,    4, m.angkatan);
        sqlite3_bind_int(s,    5, m.id);
        sqlite3_step(s); sqlite3_finalize(s);
    }

    void hapus(int id) {
        auto* s = db.prepare("DELETE FROM mahasiswa WHERE id=?");
        sqlite3_bind_int(s, 1, id);
        sqlite3_step(s); sqlite3_finalize(s);
    }

    vector<Mahasiswa> semua(const string& filter = "") {
        string sql = "SELECT id,nama,prodi,ipk,angkatan FROM mahasiswa";
        if (!filter.empty()) sql += " WHERE " + filter;
        sql += " ORDER BY nama;";
        auto* s = db.prepare(sql);
        vector<Mahasiswa> hasil;
        while (sqlite3_step(s) == SQLITE_ROW) {
            Mahasiswa m;
            m.id       = sqlite3_column_int(s, 0);
            m.nama     = (const char*)sqlite3_column_text(s, 1);
            m.prodi    = (const char*)sqlite3_column_text(s, 2);
            m.ipk      = sqlite3_column_double(s, 3);
            m.angkatan = sqlite3_column_int(s, 4);
            hasil.push_back(m);
        }
        sqlite3_finalize(s);
        return hasil;
    }

    // Statistik
    void statistik() {
        auto* s = db.prepare(R"(
            SELECT prodi,
                   COUNT(*) AS jml,
                   ROUND(AVG(ipk),2) AS avg_ipk,
                   MAX(ipk) AS max_ipk,
                   MIN(ipk) AS min_ipk
            FROM mahasiswa
            GROUP BY prodi
            ORDER BY jml DESC;
        )");
        cout << "  Prodi            | Jml | Avg IPK | Max  | Min\n";
        cout << "  " << string(55, '-') << "\n";
        while (sqlite3_step(s) == SQLITE_ROW) {
            printf("  %-18s| %-4d| %-8.2f| %-5.2f| %.2f\n",
                   (const char*)sqlite3_column_text(s,0),
                   sqlite3_column_int(s,1),
                   sqlite3_column_double(s,2),
                   sqlite3_column_double(s,3),
                   sqlite3_column_double(s,4));
        }
        sqlite3_finalize(s);
    }

    // Insert batch dengan transaksi
    int insertBatch(int jumlah) {
        auto t1 = high_resolution_clock::now();
        db.exec("BEGIN TRANSACTION;");
        auto* s = db.prepare("INSERT INTO mahasiswa(nama,prodi,ipk,angkatan) VALUES(?,?,?,?)");
        for (int i = 0; i < jumlah; i++) {
            string nama  = "Mhs-" + to_string(i);
            string prodi = (i % 3 == 0) ? "Informatika" : (i % 3 == 1) ? "Elektro" : "Sipil";
            double ipk   = 2.0 + (i % 20) * 0.1;
            int    ang   = 2020 + (i % 4);
            sqlite3_bind_text(s,   1, nama.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(s,   2, prodi.c_str(),-1, SQLITE_TRANSIENT);
            sqlite3_bind_double(s, 3, ipk);
            sqlite3_bind_int(s,    4, ang);
            sqlite3_step(s);
            sqlite3_reset(s);
        }
        sqlite3_finalize(s);
        db.exec("COMMIT;");
        auto t2 = high_resolution_clock::now();
        double ms = duration<double,milli>(t2-t1).count();
        cout << "  Insert " << jumlah << " baris: " << ms << " ms ("
             << (int)(jumlah / ms * 1000) << " baris/detik)\n";
        return jumlah;
    }
};

int main() {
    // Hapus DB lama
    remove("mahasiswa.db");

    cout << "=== Demo SQLite3 ===" << endl;
    Database db("mahasiswa.db");
    MahasiswaRepo repo(db);

    // ---- INSERT ----
    cout << "\n--- Insert data ---\n";
    vector<Mahasiswa> data = {
        {0,"Budi Santoso",   "Informatika",  3.75, 2022},
        {0,"Ani Wulandari",  "Elektro",      3.90, 2021},
        {0,"Candra Putra",   "Sipil",        3.20, 2022},
        {0,"Dewi Rahayu",    "Informatika",  3.85, 2021},
        {0,"Eko Prasetyo",   "Mesin",        3.10, 2023},
        {0,"Fitri Handayani","Elektro",      3.65, 2022},
    };
    for (auto& m : data) {
        auto ins = repo.insert(m);
        cout << "  [" << ins.id << "] " << ins.nama << " (" << ins.prodi << ")\n";
    }

    // ---- SELECT ----
    cout << "\n--- Semua mahasiswa ---\n";
    for (const auto& m : repo.semua()) {
        printf("  %2d | %-20s | %-12s | %.2f | %d\n",
               m.id, m.nama.c_str(), m.prodi.c_str(), m.ipk, m.angkatan);
    }

    // ---- FILTER ----
    cout << "\n--- Filter Informatika IPK >= 3.7 ---\n";
    for (const auto& m : repo.semua("prodi='Informatika' AND ipk>=3.7")) {
        printf("  %s — %.2f\n", m.nama.c_str(), m.ipk);
    }

    // ---- UPDATE ----
    cout << "\n--- Update Eko IPK 3.50 ---\n";
    Mahasiswa eko = repo.semua("nama='Eko Prasetyo'")[0];
    eko.ipk = 3.50;
    repo.update(eko);
    eko = repo.semua("nama='Eko Prasetyo'")[0];
    cout << "  " << eko.nama << " → IPK=" << eko.ipk << "\n";

    // ---- DELETE ----
    cout << "\n--- Hapus Candra ---\n";
    Mahasiswa candra = repo.semua("nama='Candra Putra'")[0];
    repo.hapus(candra.id);
    cout << "  Tersisa: " << repo.semua().size() << " mahasiswa\n";

    // ---- STATISTIK ----
    cout << "\n--- Statistik per Prodi ---\n";
    repo.statistik();

    // ---- BATCH INSERT ----
    cout << "\n--- Batch Insert (Transaksi) ---\n";
    repo.insertBatch(10000);
    cout << "  Total rows: " << repo.semua().size() << "\n";

    remove("mahasiswa.db");
    cout << "\nDatabase dihapus.\n";
    return 0;
}
