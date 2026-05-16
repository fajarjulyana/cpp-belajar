# Command Pattern — Undo / Redo / Replay

## Konsep

Command mengemas **aksi** sebagai objek. Setiap aksi punya metode `execute()` dan `undo()`. Ini memungkinkan:

- **Undo/Redo** — balik urutan command
- **Macro** — rekam & putar ulang serangkaian aksi
- **Replay** — simpan ke file, putar ulang sesi game
- **Networking** — kirim command ke server/client

```
Input Handler                Command Queue          Executor
┌─────────────┐              ┌───────────────┐      ┌──────────┐
│ Tekan W     │──→ MoveCmd ─→│ [MoveCmd]     │──→   │ execute()│
│ Klik musuh  │──→ AttCmd  ─→│ [AttCmd]      │      │ undo()   │
│ Ctrl+Z      │              │ ← undo terakhir│      └──────────┘
└─────────────┘              └───────────────┘
```

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -o command command.cpp
./command
```
