# Entity Component System (ECS)

## Konsep

ECS adalah arsitektur alternatif untuk OOP inheritance yang dalam. Dipakai di Unreal Engine, Unity (DOTS), EnTT, dan hampir semua game engine modern.

```
OOP Tradisional:               ECS:
─────────────────              ──────────────────────────────
GameObject                     Entity = hanya sebuah ID (uint32)
  └── Character
        └── Enemy              Component = data murni
              └── Boss           Position { x, y }
                                 Health { hp, maxHp }
Masalah: inheritance kaku       Velocity { vx, vy }
"Apakah Boss bisa berenang?"    Renderable { char simbol }
→ Harus ubah hierarchy!
                               System = logika yang memproses component
                                 MovementSystem → semua entity dengan Position+Velocity
                                 RenderSystem   → semua entity dengan Position+Renderable
                                 HealthSystem   → semua entity dengan Health
```

## Keuntungan ECS

| Aspek | OOP Inheritance | ECS |
|-------|----------------|-----|
| Fleksibilitas | Kaku (hierarki tetap) | Sangat fleksibel |
| Cache performance | Buruk (vtable, pointer chase) | Sangat baik (data berdekatan) |
| Composability | Sulit | Tambah/hapus component kapan saja |
| Debugging | Sulit (deep hierarchy) | Mudah (query component) |

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -o ecs ecs.cpp
./ecs
```
