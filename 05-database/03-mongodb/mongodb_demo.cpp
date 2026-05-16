// ============================================================
// File  : mongodb_demo.cpp
// Topik : MongoDB dengan mongocxx C++ Driver
// Kompilasi: g++ -std=c++17 -Wall -o mongo_demo mongodb_demo.cpp \
//            $(pkg-config --cflags --libs libmongocxx)
// CATATAN: Butuh mongocxx driver dan MongoDB server terinstal
// ============================================================

// Karena mongocxx membutuhkan instalasi library khusus,
// file ini menunjukkan pola LENGKAP dengan compile guard.
// Jika library tersedia, hapus #ifdef di bawah.

#ifdef MONGOCXX_AVAILABLE

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/pipeline.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/oid.hpp>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace bsoncxx::builder::stream;

// ---- Helper: cetak dokumen ----
void cetakDoc(const bsoncxx::document::view& d) {
    cout << "  " << bsoncxx::to_json(d) << "\n";
}

int main() {
    // 1. Instance (wajib, satu per program)
    mongocxx::instance inst{};

    // 2. Koneksi
    try {
        mongocxx::uri     uri("mongodb://localhost:27017");
        mongocxx::client  client(uri);
        cout << "=== Demo MongoDB ===" << endl;
        cout << "  Terkoneksi ke: " << uri.to_string() << "\n";

        // 3. Pilih database dan collection
        auto db       = client["toko_cppbelajar"];
        auto produk   = db["produk"];
        auto pelanggan = db["pelanggan"];

        // Bersihkan data lama
        produk.drop();
        pelanggan.drop();

        // ---- INSERT ONE ----
        cout << "\n--- Insert Produk ---\n";
        auto r1 = produk.insert_one(document{}
            << "nama"     << "Laptop ASUS Vivobook"
            << "kategori" << "elektronik"
            << "harga"    << 12500000.0
            << "stok"     << 15
            << "tag" << open_array << "laptop" << "windows" << "asus" << close_array
            << finalize);
        cout << "  ID: " << r1->inserted_id().get_oid().value.to_string() << "\n";

        // ---- INSERT MANY ----
        vector<bsoncxx::document::value> docs;
        docs.push_back(document{}
            << "nama" << "Mouse Logitech MX" << "kategori" << "elektronik"
            << "harga" << 850000.0 << "stok" << 50
            << "tag" << open_array << "mouse" << "wireless" << close_array << finalize);
        docs.push_back(document{}
            << "nama" << "Keyboard Mechanical" << "kategori" << "elektronik"
            << "harga" << 1200000.0 << "stok" << 30
            << "tag" << open_array << "keyboard" << "mechanical" << close_array << finalize);
        docs.push_back(document{}
            << "nama" << "Kursi Gaming" << "kategori" << "furnitur"
            << "harga" << 2300000.0 << "stok" << 12
            << "tag" << open_array << "kursi" << "gaming" << "ergonomis" << close_array << finalize);
        docs.push_back(document{}
            << "nama" << "Meja Kayu Minimalis" << "kategori" << "furnitur"
            << "harga" << 750000.0 << "stok" << 8
            << "tag" << open_array << "meja" << "kayu" << close_array << finalize);
        docs.push_back(document{}
            << "nama" << "Monitor 27in 4K" << "kategori" << "elektronik"
            << "harga" << 5800000.0 << "stok" << 7
            << "tag" << open_array << "monitor" << "4K" << close_array << finalize);

        auto r2 = produk.insert_many(docs);
        cout << "  Inserted " << r2->inserted_count() << " dokumen\n";

        // ---- FIND ALL ----
        cout << "\n--- Semua Produk ---\n";
        for (auto& d : produk.find({})) {
            cout << "  " << d["nama"].get_string().value
                 << " — Rp" << d["harga"].get_double().value
                 << " (stok=" << d["stok"].get_int32().value << ")\n";
        }

        // ---- FIND WITH FILTER ----
        cout << "\n--- Filter: Elektronik harga > 1jt ---\n";
        auto filter = document{} << "kategori" << "elektronik"
                                  << "harga" << open_document
                                  << "$gt" << 1000000.0 << close_document << finalize;
        auto opts = mongocxx::options::find{};
        opts.sort(document{} << "harga" << -1 << finalize);
        for (auto& d : produk.find(filter.view(), opts)) {
            cout << "  " << d["nama"].get_string().value
                 << " — Rp" << d["harga"].get_double().value << "\n";
        }

        // ---- UPDATE ----
        cout << "\n--- Update stok Mouse ---\n";
        auto uFilter = document{} << "nama" << open_document
                                   << "$regex" << "Mouse" << close_document << finalize;
        auto update  = document{} << "$set"
                                   << open_document << "stok" << 75 << close_document
                                   << "$inc" << open_document << "harga" << 50000.0 << close_document
                                   << finalize;
        auto ru = produk.update_one(uFilter.view(), update.view());
        cout << "  Modified: " << ru->modified_count() << "\n";

        // ---- AGGREGATION ----
        cout << "\n--- Aggregation: Statistik per Kategori ---\n";
        mongocxx::pipeline pipeline;
        pipeline.group(document{}
            << "_id"      << "$kategori"
            << "total"    << open_document << "$sum" << 1 << close_document
            << "avgHarga" << open_document << "$avg" << "$harga" << close_document
            << "maxHarga" << open_document << "$max" << "$harga" << close_document
            << "sumStok"  << open_document << "$sum" << "$stok" << close_document
            << finalize);
        pipeline.sort(document{} << "total" << -1 << finalize);
        for (auto& d : produk.aggregate(pipeline)) {
            cout << "  " << d["_id"].get_string().value
                 << ": " << d["total"].get_int32().value << " produk"
                 << ", avgHarga=Rp" << (int)d["avgHarga"].get_double().value
                 << ", stok=" << d["sumStok"].get_int32().value << "\n";
        }

        // ---- ARRAY QUERY ----
        cout << "\n--- Query tag 'wireless' ---\n";
        auto tagFilter = document{} << "tag" << "wireless" << finalize;
        for (auto& d : produk.find(tagFilter.view())) {
            cout << "  " << d["nama"].get_string().value << "\n";
        }

        // ---- INDEX ----
        cout << "\n--- Buat Index ---\n";
        produk.create_index(document{} << "nama" << 1 << finalize);
        produk.create_index(document{} << "kategori" << 1 << "harga" << -1 << finalize);
        cout << "  Index 'nama' dan compound 'kategori+harga' dibuat\n";

        // ---- DELETE ----
        cout << "\n--- Hapus produk Meja ---\n";
        auto delFilter = document{} << "nama" << open_document
                                     << "$regex" << "Meja" << close_document << finalize;
        auto rd = produk.delete_one(delFilter.view());
        cout << "  Dihapus: " << rd->deleted_count() << " dokumen\n";

        cout << "\n--- Total Produk Tersisa ---\n";
        cout << "  " << produk.count_documents({}) << " produk\n";

        // Bersihkan
        db.drop();
        cout << "\nDatabase toko_cppbelajar dihapus.\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

#else // Tanpa library mongocxx — tampilkan instruksi

#include <iostream>
int main() {
    std::cout << "=== MongoDB C++ Driver Demo ===" << std::endl;
    std::cout << "\nFile ini membutuhkan library mongocxx.\n\n";
    std::cout << "Instalasi di Ubuntu/Debian:\n";
    std::cout << "  sudo apt install libmongocxx-dev libbsoncxx-dev\n\n";
    std::cout << "Kompilasi setelah instal:\n";
    std::cout << "  g++ -std=c++17 -DMONGOCXX_AVAILABLE mongodb_demo.cpp \\\n";
    std::cout << "      $(pkg-config --cflags --libs libmongocxx) \\\n";
    std::cout << "      -o mongo_demo\n\n";
    std::cout << "Jalankan MongoDB dulu:\n";
    std::cout << "  sudo systemctl start mongod\n";
    std::cout << "  mongosh  # buka shell MongoDB\n\n";
    std::cout << "Lihat README.md untuk panduan lengkap.\n";
    return 0;
}

#endif
