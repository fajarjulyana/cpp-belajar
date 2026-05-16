# Database: MongoDB dengan mongocxx

MongoDB adalah database NoSQL berbasis dokumen. Data disimpan dalam format BSON (binary JSON).

---

## Instalasi

```bash
# Ubuntu/Debian — instal mongocxx driver
sudo apt install libmongoc-dev libbson-dev

# Atau build dari source (cara resmi):
# https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/installation/

# Fedora
sudo dnf install mongo-c-driver-devel libbson-devel

# macOS
brew install mongo-cxx-driver

# Windows — lihat: https://www.mongodb.com/docs/drivers/cxx/
```

---

## Kompilasi

```bash
g++ -std=c++17 -Wall -o mongo_demo mongodb_demo.cpp \
    $(pkg-config --cflags --libs libmongocxx)

# Atau manual
g++ -std=c++17 -o mongo_demo mongodb_demo.cpp \
    -I/usr/include/mongocxx/v_noabi \
    -I/usr/include/bsoncxx/v_noabi \
    -lmongocxx -lbsoncxx
```

---

## Konsep MongoDB

| SQL | MongoDB |
|-----|---------|
| Database | Database |
| Table | Collection |
| Row | Document |
| Column | Field |
| Primary Key | `_id` |
| JOIN | `$lookup` (aggregation) |

---

## Pola Dasar mongocxx

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
using namespace bsoncxx::builder::stream;

// 1. Instance (satu per program)
mongocxx::instance inst{};

// 2. Koneksi
mongocxx::uri uri("mongodb://localhost:27017");
mongocxx::client client(uri);

// 3. Pilih database dan collection
auto db   = client["toko"];
auto coll = db["produk"];

// 4. Insert satu dokumen
bsoncxx::document::value doc = document{}
    << "nama"     << "Laptop ASUS"
    << "harga"    << 12500000.0
    << "kategori" << "elektronik"
    << "tag"      << open_array << "laptop" << "asus" << "windows" << close_array
    << finalize;
auto result = coll.insert_one(doc.view());
cout << "Inserted ID: " << result->inserted_id().get_oid().value.to_string() << "\n";

// 5. Find semua
for (auto& doc : coll.find({})) {
    cout << bsoncxx::to_json(doc) << "\n";
}

// 6. Find dengan filter
auto filter = document{} << "kategori" << "elektronik" << finalize;
for (auto& d : coll.find(filter.view())) {
    cout << d["nama"].get_string().value << "\n";
}

// 7. Update
auto updateFilter = document{} << "nama" << "Laptop ASUS" << finalize;
auto update       = document{} << "$set"
    << open_document << "harga" << 11000000.0 << close_document << finalize;
coll.update_one(updateFilter.view(), update.view());

// 8. Delete
coll.delete_one(document{} << "nama" << "Laptop ASUS" << finalize);
```

---

## Aggregation Pipeline

```cpp
// Aggregation — powerful query dan transform
mongocxx::pipeline pipeline;

// Group by kategori, hitung statistik
pipeline.group(document{}
    << "_id"     << "$kategori"
    << "total"   << open_document << "$sum" << 1 << close_document
    << "avgHarga"<< open_document << "$avg" << "$harga" << close_document
    << "maxHarga"<< open_document << "$max" << "$harga" << close_document
    << finalize);

pipeline.sort(document{} << "total" << -1 << finalize);

for (auto& d : coll.aggregate(pipeline)) {
    cout << bsoncxx::to_json(d) << "\n";
}
```

---

## Index

```cpp
// Buat index pada field "nama" (cepat untuk search)
mongocxx::options::index opts;
opts.unique(false);
coll.create_index(document{} << "nama" << 1 << finalize, opts);

// Compound index
coll.create_index(document{} << "kategori" << 1 << "harga" << -1 << finalize);

// Text index (full-text search)
coll.create_index(document{} << "nama" << "text" << "deskripsi" << "text" << finalize);
auto textFilter = document{} << "$text" << open_document << "$search" << "laptop" << close_document << finalize;
for (auto& d : coll.find(textFilter.view())) cout << d["nama"].get_string().value << "\n";
```

---

## Latihan

1. Buat database blog: koleksi `posts`, `users`, `comments`
2. Implementasikan paginasi dengan `skip()` dan `limit()`
3. Buat aggregation pipeline untuk menghitung top-10 produk terlaris
4. Implementasikan full-text search untuk mencari produk

---

**[← PostgreSQL](../02-postgresql/README.md)** | **[→ Build Systems](../../06-build-systems/01-makefile/README.md)**
