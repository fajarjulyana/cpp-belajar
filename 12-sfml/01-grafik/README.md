# SFML — Simple and Fast Multimedia Library

## Apa itu SFML?

SFML (Simple and Fast Multimedia Library) adalah library C++ untuk membuat aplikasi multimedia: grafik 2D, audio, input (keyboard, mouse, gamepad), dan jaringan. Cocok untuk membuat game 2D, visualisasi, dan aplikasi interaktif.

## Modul SFML

| Modul | Header | Fungsi |
|-------|--------|--------|
| **System** | `<SFML/System.hpp>` | Clock, Thread, Vector |
| **Window** | `<SFML/Window.hpp>` | Window, Event, Input |
| **Graphics** | `<SFML/Graphics.hpp>` | Sprite, Shape, Text, Shader |
| **Audio** | `<SFML/Audio.hpp>` | Sound, Music, SoundBuffer |
| **Network** | `<SFML/Network.hpp>` | Socket, Packet, HTTP |

## Instalasi

### Ubuntu/Debian
```bash
sudo apt install libsfml-dev
```

### macOS
```bash
brew install sfml
```

### Windows
Download dari https://www.sfml-dev.org/download.php, ekstrak, atur path di IDE.

## Kompilasi dengan SFML

```bash
# Kompilasi
g++ -std=c++17 -o program main.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system

# Jika SFML tidak di path standar
g++ -std=c++17 -o program main.cpp \
    -I/usr/include/SFML \
    -lsfml-graphics -lsfml-window -lsfml-system
```

## Contoh Kode Dasar

### Jendela & Loop Utama
```cpp
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Halo SFML!");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::Black);
        // Gambar sesuatu di sini
        window.display();
    }
    return 0;
}
```

### Bentuk Dasar
```cpp
// Lingkaran
sf::CircleShape lingkaran(50.f);
lingkaran.setFillColor(sf::Color::Green);
lingkaran.setPosition(100.f, 100.f);
window.draw(lingkaran);

// Persegi
sf::RectangleShape kotak(sf::Vector2f(200.f, 100.f));
kotak.setFillColor(sf::Color::Blue);
kotak.setOutlineColor(sf::Color::White);
kotak.setOutlineThickness(2.f);
window.draw(kotak);
```

### Teks
```cpp
sf::Font font;
font.loadFromFile("arial.ttf");

sf::Text teks;
teks.setFont(font);
teks.setString("Halo Dunia!");
teks.setCharacterSize(30);
teks.setFillColor(sf::Color::Yellow);
window.draw(teks);
```

### Gambar & Sprite
```cpp
sf::Texture texture;
texture.loadFromFile("gambar.png");

sf::Sprite sprite;
sprite.setTexture(texture);
sprite.setPosition(200.f, 150.f);
sprite.setScale(2.f, 2.f);  // perbesar 2x
window.draw(sprite);
```

### Input Keyboard & Mouse
```cpp
// Dalam loop event
if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Space)
        melompat = true;
    if (event.key.code == sf::Keyboard::Escape)
        window.close();
}

// Real-time (di luar event loop)
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    sprite.move(-5.f, 0.f);
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    sprite.move(5.f, 0.f);

// Mouse
sf::Vector2i posMouse = sf::Mouse::getPosition(window);
```

### Audio
```cpp
#include <SFML/Audio.hpp>

// Sound pendek (efek suara)
sf::SoundBuffer buffer;
buffer.loadFromFile("ledakan.wav");
sf::Sound sound;
sound.setBuffer(buffer);
sound.play();

// Musik panjang (streaming)
sf::Music musik;
musik.openFromFile("bgm.ogg");
musik.setLoop(true);
musik.play();
```

## Lihat juga

- `sfml_simulasi.cpp` — Simulasi game loop yang bisa dikompilasi tanpa SFML (menggunakan terminal ASCII)
- `game_loop_sim.cpp` — Pola game loop lengkap dengan delta time
