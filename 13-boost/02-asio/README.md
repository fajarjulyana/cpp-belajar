# Boost.Asio — Asynchronous I/O

## Apa itu Boost.Asio?

Boost.Asio adalah library untuk **asynchronous I/O** — networking, timer, serial port, dan lebih. Ia mendukung dua gaya pemrograman:

1. **Synchronous** — seperti socket biasa, blokir sampai selesai
2. **Asynchronous** — callback/coroutine, tidak blokir

```
Synchronous:
  read(socket, buffer);    ← blokir sampai data tiba
  process(buffer);

Asynchronous:
  async_read(socket, buffer, [](error_code e, size_t n) {
      process(buffer);     ← dipanggil nanti oleh io_context
  });
  io_context.run();        ← event loop
```

## io_context — Jantung Asio

`io_context` adalah event loop yang mengelola semua operasi async.

```cpp
boost::asio::io_context io;

// Daftarkan operasi async...
async_read(..., handler);
async_write(..., handler);

// Jalankan event loop (blokir sampai semua selesai)
io.run();
```

## TCP Echo Server (Asio)

```cpp
#include <boost/asio.hpp>
using namespace boost::asio;

class Session : public std::enable_shared_from_this<Session> {
    tcp::socket socket_;
    char data_[1024];
public:
    Session(tcp::socket s) : socket_(std::move(s)) {}

    void start() { doRead(); }

    void doRead() {
        auto self = shared_from_this();
        socket_.async_read_some(buffer(data_),
            [this, self](error_code e, size_t n) {
                if (!e) doWrite(n);
            });
    }

    void doWrite(size_t n) {
        auto self = shared_from_this();
        async_write(socket_, buffer(data_, n),
            [this, self](error_code e, size_t) {
                if (!e) doRead();
            });
    }
};

class Server {
    tcp::acceptor acceptor_;
public:
    Server(io_context& io, short port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        doAccept();
    }

    void doAccept() {
        acceptor_.async_accept([this](error_code e, tcp::socket s) {
            if (!e) std::make_shared<Session>(std::move(s))->start();
            doAccept();
        });
    }
};

int main() {
    io_context io;
    Server server(io, 9000);
    io.run();
}
```

## Timer

```cpp
io_context io;
steady_timer timer(io, std::chrono::seconds(3));

timer.async_wait([](const error_code&) {
    std::cout << "Timer 3 detik selesai!\n";
});

io.run();
```

## Coroutine (C++20 / Boost.Coroutine2)

```cpp
awaitable<void> echo(tcp::socket socket) {
    char data[1024];
    for (;;) {
        size_t n = co_await socket.async_read_some(
                       buffer(data), use_awaitable);
        co_await async_write(socket,
                       buffer(data, n), use_awaitable);
    }
}
```

## Kompilasi

```bash
# Header-only mode (Asio standalone — tanpa Boost!)
# Download asio.hpp dari https://think-async.com/Asio/
g++ -std=c++17 -pthread -DASIO_STANDALONE -o server server.cpp

# Dengan Boost
g++ -std=c++17 -pthread -o server server.cpp \
    -lboost_system -lboost_thread

# Dengan Boost + coroutine
g++ -std=c++20 -pthread -o server server.cpp \
    -lboost_system -lboost_coroutine
```

## Kapan Pakai Asio?

| Kasus | Rekomendasi |
|-------|-------------|
| Server simple | POSIX socket + thread (seperti contoh kita) |
| Banyak koneksi (10k+) | **Boost.Asio** (event-driven, efisien) |
| Game networking | **Boost.Asio** |
| Protokol kustom | **Boost.Asio** |
| High-performance API | **Boost.Beast** (di atas Asio) |

Asio menggunakan **reactor pattern** — satu thread bisa menangani ribuan koneksi tanpa blocking.
