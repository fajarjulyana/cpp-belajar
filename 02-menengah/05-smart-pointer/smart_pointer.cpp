// ============================================================
// File  : smart_pointer.cpp
// Topik : Smart Pointer (unique_ptr, shared_ptr, weak_ptr)
// Kompilasi: g++ -std=c++17 -Wall -o smart_ptr smart_pointer.cpp
// ============================================================
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- Base class ----
struct Bentuk {
    string nama;
    Bentuk(const string& n) : nama(n) {
        cout << "  [+] " << nama << " dibuat\n";
    }
    virtual ~Bentuk() {
        cout << "  [-] " << nama << " dihapus\n";
    }
    virtual double luas() const = 0;
};

struct Lingkaran : Bentuk {
    double r;
    Lingkaran(double r) : Bentuk("Lingkaran(r=" + to_string(r) + ")"), r(r) {}
    double luas() const override { return 3.14159 * r * r; }
};

struct Persegi : Bentuk {
    double s;
    Persegi(double s) : Bentuk("Persegi(s=" + to_string(s) + ")"), s(s) {}
    double luas() const override { return s * s; }
};

// ---- Linked List dengan unique_ptr ----
struct Node {
    int nilai;
    unique_ptr<Node> berikut;
    Node(int v) : nilai(v) {}
};

class LinkedList {
    unique_ptr<Node> kepala;
public:
    void tambahDepan(int v) {
        auto baru = make_unique<Node>(v);
        baru->berikut = move(kepala);
        kepala = move(baru);
    }
    void cetak() const {
        Node* curr = kepala.get();
        while (curr) { cout << curr->nilai << " → "; curr = curr->berikut.get(); }
        cout << "null\n";
    }
};

// ---- Circular Reference Demo ----
struct NodeSiklik {
    int id;
    shared_ptr<NodeSiklik> berikut;
    weak_ptr<NodeSiklik>   sebelum;  // weak mencegah circular
    NodeSiklik(int i) : id(i) { cout << "  [+] Node" << id << "\n"; }
    ~NodeSiklik() { cout << "  [-] Node" << id << "\n"; }
};

// ---- Observer Pattern dengan shared_ptr/weak_ptr ----
class Subjek;

class Pengamat {
    string nama;
public:
    Pengamat(const string& n) : nama(n) {}
    void update(const string& acara) {
        cout << "  " << nama << " menerima: " << acara << "\n";
    }
    string getNama() const { return nama; }
};

class Subjek {
    vector<weak_ptr<Pengamat>> pengamat;
public:
    void daftar(shared_ptr<Pengamat> p) { pengamat.push_back(p); }
    void notifikasi(const string& acara) {
        auto it = pengamat.begin();
        while (it != pengamat.end()) {
            if (auto sp = it->lock()) {
                sp->update(acara);
                ++it;
            } else {
                it = pengamat.erase(it);  // hapus yang sudah mati
            }
        }
    }
    int jumlahPengamat() const { return (int)pengamat.size(); }
};

int main() {
    // ---- unique_ptr ----
    cout << "=== unique_ptr ===" << endl;
    {
        auto p = make_unique<int>(42);
        cout << "Nilai: " << *p << "\n";
        *p = 100;
        cout << "Setelah ubah: " << *p << "\n";
        // auto p2 = p;   // ERROR: tidak bisa copy
        auto p2 = move(p);  // OK: move
        cout << "p null? " << boolalpha << (p == nullptr) << "\n";
        cout << "p2: " << *p2 << "\n";
    }  // p2 otomatis dihapus

    // ---- unique_ptr untuk Polimorfisme ----
    cout << "\n=== unique_ptr + Polimorfisme ===" << endl;
    {
        vector<unique_ptr<Bentuk>> bentuk;
        bentuk.push_back(make_unique<Lingkaran>(5.0));
        bentuk.push_back(make_unique<Persegi>(4.0));
        bentuk.push_back(make_unique<Lingkaran>(3.0));
        for (const auto& b : bentuk)
            cout << "  " << b->nama << " → luas=" << b->luas() << "\n";
    }  // semua dihapus otomatis

    // ---- Linked List dengan unique_ptr ----
    cout << "\n=== LinkedList dengan unique_ptr ===" << endl;
    LinkedList ll;
    for (int i = 1; i <= 5; i++) ll.tambahDepan(i * 10);
    ll.cetak();

    // ---- shared_ptr ----
    cout << "\n=== shared_ptr ===" << endl;
    {
        auto sp1 = make_shared<string>("Halo, shared_ptr!");
        cout << "ref_count=" << sp1.use_count() << "\n";  // 1
        {
            auto sp2 = sp1;
            auto sp3 = sp1;
            cout << "ref_count=" << sp1.use_count() << "\n";  // 3
            cout << "nilai: " << *sp1 << "\n";
        }
        cout << "ref_count setelah sp2,sp3 mati=" << sp1.use_count() << "\n";  // 1
    }

    // ---- weak_ptr — Circular Reference ----
    cout << "\n=== weak_ptr (Circular Reference) ===" << endl;
    {
        auto n1 = make_shared<NodeSiklik>(1);
        auto n2 = make_shared<NodeSiklik>(2);
        auto n3 = make_shared<NodeSiklik>(3);
        n1->berikut = n2;
        n2->berikut = n3;
        n3->sebelum = n2;   // weak_ptr: tidak menambah ref count
        n2->sebelum = n1;

        cout << "n1.use_count=" << n1.use_count() << "\n";  // 1 (hanya n1 yang pegang)
        cout << "n2.use_count=" << n2.use_count() << "\n";  // 2 (n1->berikut + n2)

        if (auto sp = n3->sebelum.lock())
            cout << "n3 sebelumnya: Node" << sp->id << "\n";
    }  // semua dihapus karena tidak ada circular

    // ---- Observer dengan weak_ptr ----
    cout << "\n=== Observer Pattern ===" << endl;
    Subjek subjek;
    {
        auto p1 = make_shared<Pengamat>("Pengamat-A");
        auto p2 = make_shared<Pengamat>("Pengamat-B");
        subjek.daftar(p1);
        subjek.daftar(p2);
        subjek.notifikasi("Acara-1");
        // p1 keluar scope (mati)
        auto p3 = make_shared<Pengamat>("Pengamat-C");
        subjek.daftar(p3);
        subjek.notifikasi("Acara-2");  // p1 sudah mati, diabaikan
    }

    return 0;
}
