// ============================================================
// File  : postgres_demo.cpp
// Topik : Database PostgreSQL dengan libpq
// Kompilasi: g++ -std=c++17 -Wall -o pg_demo postgres_demo.cpp -lpq
// Jalankan PostgreSQL dulu dan sesuaikan string koneksi di bawah.
// ============================================================
#include <libpq-fe.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
using namespace std;

// ---- Wrapper RAII untuk koneksi dan result ----
class PGConn {
    PGconn* conn;
public:
    PGConn(const string& info) : conn(PQconnectdb(info.c_str())) {
        if (PQstatus(conn) != CONNECTION_OK)
            throw runtime_error("Koneksi gagal: " + string(PQerrorMessage(conn)));
        cout << "  Terkoneksi ke: " << PQdb(conn) << "@" << PQhost(conn) << "\n";
    }
    ~PGConn() { PQfinish(conn); }
    PGconn* raw() { return conn; }

    // Eksekusi tanpa hasil
    void exec(const string& sql) {
        PGresult* r = PQexec(conn, sql.c_str());
        auto status = PQresultStatus(r);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
            string msg = PQerrorMessage(conn);
            PQclear(r);
            throw runtime_error("SQL error: " + msg);
        }
        PQclear(r);
    }

    // Eksekusi dengan parameter ($1, $2, ...)
    PGresult* execParams(const string& sql, vector<string> params) {
        vector<const char*> cparams(params.size());
        for (size_t i = 0; i < params.size(); i++) cparams[i] = params[i].c_str();
        PGresult* r = PQexecParams(conn, sql.c_str(), (int)params.size(),
                                   nullptr, cparams.data(), nullptr, nullptr, 0);
        auto status = PQresultStatus(r);
        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
            string msg = PQerrorMessage(conn);
            PQclear(r);
            throw runtime_error("Param error: " + msg);
        }
        return r;
    }
};

// ---- Struct ----
struct Produk {
    int    id = 0;
    string nama, kategori;
    double harga = 0;
    int    stok = 0;
};

// ---- Repository ----
class ProdukRepo {
    PGConn& db;
public:
    ProdukRepo(PGConn& d) : db(d) {
        db.exec(R"(
            CREATE TABLE IF NOT EXISTS produk (
                id        SERIAL PRIMARY KEY,
                nama      TEXT         NOT NULL,
                kategori  TEXT         NOT NULL,
                harga     NUMERIC(15,2) NOT NULL,
                stok      INTEGER       DEFAULT 0,
                dibuat    TIMESTAMP     DEFAULT CURRENT_TIMESTAMP
            );
        )");
    }

    Produk insert(const Produk& p) {
        auto* r = db.execParams(
            "INSERT INTO produk(nama,kategori,harga,stok) VALUES($1,$2,$3,$4) RETURNING id",
            {p.nama, p.kategori, to_string(p.harga), to_string(p.stok)});
        Produk hasil = p;
        hasil.id = stoi(PQgetvalue(r, 0, 0));
        PQclear(r);
        return hasil;
    }

    void updateStok(int id, int stokBaru) {
        auto* r = db.execParams("UPDATE produk SET stok=$1 WHERE id=$2",
                                {to_string(stokBaru), to_string(id)});
        PQclear(r);
    }

    void hapus(int id) {
        auto* r = db.execParams("DELETE FROM produk WHERE id=$1", {to_string(id)});
        PQclear(r);
    }

    vector<Produk> cariSemua(const string& filter = "") {
        string sql = "SELECT id,nama,kategori,harga,stok FROM produk";
        if (!filter.empty()) sql += " WHERE " + filter;
        sql += " ORDER BY nama";
        PGresult* r = PQexec(db.raw(), sql.c_str());
        vector<Produk> hasil;
        int rows = PQntuples(r);
        for (int i = 0; i < rows; i++) {
            Produk p;
            p.id       = stoi(PQgetvalue(r,i,0));
            p.nama     = PQgetvalue(r,i,1);
            p.kategori = PQgetvalue(r,i,2);
            p.harga    = stod(PQgetvalue(r,i,3));
            p.stok     = stoi(PQgetvalue(r,i,4));
            hasil.push_back(p);
        }
        PQclear(r);
        return hasil;
    }

    void statistik() {
        auto* r = PQexec(db.raw(), R"(
            SELECT kategori,
                   COUNT(*) AS jml,
                   ROUND(AVG(harga)::numeric, 2) AS avg_harga,
                   SUM(stok) AS total_stok,
                   MAX(harga) AS harga_max
            FROM produk
            GROUP BY kategori
            ORDER BY jml DESC;
        )");
        cout << "\n  Kategori      | Jml | Avg Harga      | Total Stok | Max Harga\n";
        cout << "  " << string(65, '-') << "\n";
        for (int i = 0; i < PQntuples(r); i++) {
            printf("  %-14s| %-4s| %-15s| %-11s| %s\n",
                   PQgetvalue(r,i,0), PQgetvalue(r,i,1), PQgetvalue(r,i,2),
                   PQgetvalue(r,i,3), PQgetvalue(r,i,4));
        }
        PQclear(r);
    }
};

int main() {
    // Gunakan variabel environment atau ubah string di bawah
    const string connStr =
        "host=localhost dbname=devdb user=devuser password=rahasia "
        "connect_timeout=3";

    cout << "=== Demo PostgreSQL ===" << endl;

    try {
        PGConn db(connStr);
        ProdukRepo repo(db);

        // Hapus data lama jika ada
        db.exec("DELETE FROM produk;");

        // ---- INSERT ----
        cout << "\n--- Insert produk ---\n";
        vector<Produk> produkData = {
            {0,"Laptop ASUS Vivobook","Elektronik",  12500000, 15},
            {0,"Mouse Logitech MX",   "Elektronik",   850000,  50},
            {0,"Keyboard Mechanical", "Elektronik",   1200000, 30},
            {0,"Meja Kerja Kayu",     "Furnitur",      750000,   8},
            {0,"Kursi Gaming",        "Furnitur",     2300000,  12},
            {0,"Monitor 27 inch 4K",  "Elektronik",   5800000,   7},
        };

        for (auto& p : produkData) {
            auto ins = repo.insert(p);
            printf("  [%d] %s — Rp%.0f (stok=%d)\n",
                   ins.id, ins.nama.c_str(), ins.harga, ins.stok);
        }

        // ---- SELECT ----
        cout << "\n--- Semua produk ---\n";
        for (const auto& p : repo.cariSemua()) {
            printf("  %2d | %-25s | %-12s | Rp%-10.0f | stok=%d\n",
                   p.id, p.nama.c_str(), p.kategori.c_str(), p.harga, p.stok);
        }

        // ---- Filter ----
        cout << "\n--- Filter Elektronik harga > 1jt ---\n";
        for (const auto& p : repo.cariSemua("kategori='Elektronik' AND harga > 1000000")) {
            printf("  %s — Rp%.0f\n", p.nama.c_str(), p.harga);
        }

        // ---- UPDATE ----
        cout << "\n--- Update stok Mouse ---\n";
        auto mouse = repo.cariSemua("nama LIKE '%Mouse%'");
        if (!mouse.empty()) {
            repo.updateStok(mouse[0].id, 75);
            cout << "  Mouse stok baru: " << repo.cariSemua("nama LIKE '%Mouse%'")[0].stok << "\n";
        }

        // ---- TRANSAKSI: Jual produk ----
        cout << "\n--- Transaksi: Jual 3 Laptop ---\n";
        db.exec("BEGIN");
        try {
            auto laptops = repo.cariSemua("nama LIKE '%Laptop%'");
            if (!laptops.empty() && laptops[0].stok >= 3) {
                repo.updateStok(laptops[0].id, laptops[0].stok - 3);
                // Simulasi: catat ke log penjualan
                cout << "  Terjual 3 unit " << laptops[0].nama << "\n";
                db.exec("COMMIT");
                cout << "  Transaksi COMMIT berhasil\n";
            } else {
                db.exec("ROLLBACK");
                cout << "  Stok tidak cukup, ROLLBACK\n";
            }
        } catch (...) {
            db.exec("ROLLBACK");
            throw;
        }

        // ---- STATISTIK ----
        cout << "\n--- Statistik per Kategori ---";
        repo.statistik();

        // ---- WINDOW FUNCTION ----
        cout << "\n--- Ranking Harga per Kategori ---\n";
        auto* r = PQexec(db.raw(), R"(
            SELECT nama, kategori, harga,
                   RANK() OVER (PARTITION BY kategori ORDER BY harga DESC) AS ranking
            FROM produk
            ORDER BY kategori, ranking;
        )");
        printf("  %-28s| %-12s| %-12s| Rank\n", "Nama", "Kategori", "Harga");
        cout << "  " << string(60, '-') << "\n";
        for (int i = 0; i < PQntuples(r); i++) {
            printf("  %-28s| %-12s| Rp%-10s| %s\n",
                   PQgetvalue(r,i,0), PQgetvalue(r,i,1),
                   PQgetvalue(r,i,2), PQgetvalue(r,i,3));
        }
        PQclear(r);

    } catch (const exception& e) {
        cerr << "\n[ERROR] " << e.what() << "\n";
        cerr << "Pastikan PostgreSQL berjalan dan string koneksi benar.\n";
        cerr << "Setup: sudo -u postgres createuser devuser --createdb\n";
        cerr << "       sudo -u postgres createdb devdb --owner=devuser\n";
        return 1;
    }

    return 0;
}
