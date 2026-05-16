# Design Patterns Lanjut — Game & Sistem

## Apa itu Design Pattern?

Design Pattern adalah solusi umum yang sudah teruji untuk masalah yang sering muncul dalam desain perangkat lunak. Bagian ini fokus pada pattern yang paling banyak dipakai di **game engine**, **simulasi**, dan **sistem berperforma tinggi**.

## Pattern yang Dipelajari

| # | Pattern | Kategori | Kegunaan Utama |
|---|---------|----------|----------------|
| 01 | **Observer** (Event System) | Behavioral | Notifikasi event antar objek tanpa coupling |
| 02 | **Command** (Undo/Redo) | Behavioral | Aksi sebagai objek, mendukung undo/redo/replay |
| 03 | **ECS** (Entity Component System) | Architectural | Arsitektur game engine modern |
| 04 | **Object Pool** | Creational | Reuse objek untuk menghindari alokasi berulang |

## Mengapa Pattern Ini Penting di Game?

```
Tanpa Pattern                    Dengan Pattern
─────────────────                ─────────────────────────────
Musuh → langsung panggil         Musuh → emit Event("mati")
  Player.tambahSkor()              ↓
  HUD.updateTampilan()           EventSystem broadcasts →
  AchievementSystem.cek()          Player.onMusuhMati()
  AudioSystem.putar("die.wav")     HUD.onMusuhMati()
                                   Achievement.onMusuhMati()
                                   Audio.onMusuhMati()
  (Musuh tahu semua sistem!)      (Musuh tidak tahu siapa yang mendengar)
```

## Perbandingan

| Pattern | Masalah | Solusi |
|---------|---------|--------|
| Observer | Objek perlu memberi tahu banyak pihak | Event/Callback system |
| Command | Perlu undo, replay, atau scripting | Kemas aksi sebagai objek |
| ECS | Inheritance hierarchy terlalu dalam | Komposisi via komponen |
| Object Pool | `new`/`delete` sering = lambat | Daur ulang objek yang sudah ada |

## Relasi Antar Pattern

```
ECS ──────────────────────────── Object Pool
│  Entity punya banyak           │  Komponen di-pool agar tidak
│  Component                     │  ada alokasi saat runtime
│
└── Observer ──────────────────── Command
    Event antar System            Input System menghasilkan Command
    di dalam ECS                  yang bisa di-undo/replay
```
