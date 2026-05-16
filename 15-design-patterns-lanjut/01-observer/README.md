# Observer Pattern — Event / Signal System

## Konsep

Observer memisahkan **publisher** (yang menghasilkan event) dari **subscriber** (yang merespons). Publisher tidak perlu tahu siapa yang mendengarkan.

```
Subject/Publisher               Observer/Subscriber
┌──────────────┐                ┌──────────────────┐
│  EventBus    │ ─── notify ──→ │  PlayerHUD        │
│              │                ├──────────────────┤
│  subscribe() │                │  ScoreSystem      │
│  emit()      │                ├──────────────────┤
│  unsubscribe │                │  AchievementSys   │
└──────────────┘                └──────────────────┘
```

## Tiga Variasi yang Dipelajari

1. **Observer Klasik** — interface `IObserver`, pointer ke subscriber
2. **Event Bus** — map dari event type ke list callback
3. **Signal/Slot (Qt-style)** — `Signal<Args...>` yang type-safe dengan template

## Kompilasi & Jalankan

```bash
g++ -std=c++17 -o observer observer.cpp
./observer
```
