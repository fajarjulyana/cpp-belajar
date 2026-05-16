# Pengenalan JUCE Framework

JUCE (Jules' Utility Class Extensions) adalah framework C++ open-source untuk membangun aplikasi dan plugin audio profesional. Digunakan oleh ribuan developer audio di seluruh dunia.

---

## Mengapa JUCE?

| Fitur | Keterangan |
|-------|------------|
| Cross-platform | Windows, macOS, Linux, iOS, Android |
| Plugin format | VST3, AU, AAX, LV2, Standalone |
| GUI toolkit | Bawaan, tanpa ketergantungan eksternal |
| Audio engine | DSP, MIDI, format audio lengkap |
| Komunitas | Aktif, dokumentasi lengkap |

---

## Instalasi & Setup

### 1. Download JUCE
```bash
# Clone dari GitHub
git clone https://github.com/juce-framework/JUCE.git ~/JUCE

# Atau download ZIP dari https://juce.com/get-juce/
```

### 2. Buat Projector (IDE)
```bash
# Jalankan Projucer (GUI tool JUCE)
cd ~/JUCE
./Projucer   # macOS/Linux
Projucer.exe # Windows
```

### 3. Setup dengan CMake (Modern)
```bash
# CMakeLists.txt minimal
cmake_minimum_required(VERSION 3.22)
project(JuceApp VERSION 1.0)

add_subdirectory(JUCE)

juce_add_gui_app(JuceApp PRODUCT_NAME "Belajar JUCE")
target_sources(JuceApp PRIVATE Main.cpp MainComponent.cpp)
target_compile_features(JuceApp PRIVATE cxx_std_17)
target_link_libraries(JuceApp
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
```

---

## Struktur Project JUCE

```
MeuProyekJUCE/
├── Source/
│   ├── Main.cpp           ← Entry point aplikasi
│   ├── MainComponent.h    ← Komponen utama (header)
│   └── MainComponent.cpp  ← Implementasi
├── CMakeLists.txt
└── JuceLibraryCode/       ← Auto-generated oleh Projucer
```

---

## Konsep Inti JUCE

### AudioProcessor — Jantung Plugin
```
AudioProcessor
├── prepareToPlay()    ← dipanggil sebelum audio mulai
├── processBlock()     ← diproses tiap buffer audio
├── releaseResources() ← pembersihan saat audio berhenti
└── createEditor()     ← membuat GUI editor plugin
```

### Component — Sistem GUI
```
Component (kelas dasar semua widget)
├── paint()            ← menggambar widget
├── resized()          ← ukuran berubah
├── mouseDown()        ← event mouse
└── addAndMakeVisible() ← tambah child component
```

### Message Thread vs Audio Thread
```
┌─────────────────────────────────────────┐
│  GUI Thread (Message Thread)            │
│  - paint(), resized(), mouseDown()      │
│  - TIDAK BOLEH di audio thread!         │
├─────────────────────────────────────────┤
│  Audio Thread                           │
│  - processBlock() ← real-time!          │
│  - TIDAK BOLEH: alloc, mutex, cout      │
└─────────────────────────────────────────┘
```

---

## Hello World JUCE — Aplikasi GUI Pertama

### Main.cpp
```cpp
#include <JuceHeader.h>
#include "MainComponent.h"

class BelajarJuceApplication : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override
        { return "Belajar JUCE"; }
    const juce::String getApplicationVersion() override
        { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& /*commandLine*/) override {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override { mainWindow = nullptr; }

    void systemRequestedQuit() override { quit(); }

    class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance().getDefaultLookAndFeel()
                    .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(BelajarJuceApplication)
```

### MainComponent.h
```cpp
#pragma once
#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent {
public:
    MainComponent();
    ~MainComponent() override;

    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::TextButton tombolPlay{"▶ Mulai"};
    juce::TextButton tombolStop{"■ Stop"};
    juce::Slider     sliderVolume;
    juce::Label      labelStatus;

    bool sedangBermain = false;
    double currentSampleRate = 0.0;
    double currentAngle = 0.0, angleDelta = 0.0;
    float  volume = 0.5f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
```

### MainComponent.cpp
```cpp
#include "MainComponent.h"

MainComponent::MainComponent() {
    // Tombol Play
    tombolPlay.onClick = [this] {
        if (!sedangBermain) {
            setAudioChannels(0, 2);
            sedangBermain = true;
            tombolPlay.setEnabled(false);
            tombolStop.setEnabled(true);
            labelStatus.setText("Memainkan 440 Hz (A4)...", juce::dontSendNotification);
        }
    };

    // Tombol Stop
    tombolStop.setEnabled(false);
    tombolStop.onClick = [this] {
        if (sedangBermain) {
            shutdownAudio();
            sedangBermain = false;
            tombolPlay.setEnabled(true);
            tombolStop.setEnabled(false);
            labelStatus.setText("Berhenti.", juce::dontSendNotification);
        }
    };

    // Slider Volume
    sliderVolume.setRange(0.0, 1.0);
    sliderVolume.setValue(0.5);
    sliderVolume.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    sliderVolume.onValueChange = [this] {
        volume = (float)sliderVolume.getValue();
    };

    // Label
    labelStatus.setText("Siap. Tekan Play untuk mendengar nada A4.", juce::dontSendNotification);
    labelStatus.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(tombolPlay);
    addAndMakeVisible(tombolStop);
    addAndMakeVisible(sliderVolume);
    addAndMakeVisible(labelStatus);

    setSize(400, 200);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() { shutdownAudio(); }

void MainComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) {
    currentSampleRate = sampleRate;
    // Hitung angleDelta untuk 440 Hz (nada A4)
    double frequencyHz = 440.0;
    angleDelta = frequencyHz * juce::MathConstants<double>::twoPi / sampleRate;
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    auto* leftChannel  = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    for (int sample = 0; sample < bufferToFill.numSamples; ++sample) {
        float currentSample = (float)(std::sin(currentAngle) * volume);
        currentAngle += angleDelta;
        if (currentAngle > juce::MathConstants<double>::twoPi)
            currentAngle -= juce::MathConstants<double>::twoPi;

        leftChannel[sample]  = currentSample;
        rightChannel[sample] = currentSample;
    }
}

void MainComponent::releaseResources() {
    currentAngle = 0.0;
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f));
    g.drawText("Belajar JUCE — Oscillator Sederhana",
               getLocalBounds().removeFromTop(50),
               juce::Justification::centred, true);
}

void MainComponent::resized() {
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(50);   // ruang untuk judul

    labelStatus.setBounds(area.removeFromTop(30));
    area.removeFromTop(10);

    auto row1 = area.removeFromTop(40);
    tombolPlay.setBounds(row1.removeFromLeft(120));
    row1.removeFromLeft(10);
    tombolStop.setBounds(row1.removeFromLeft(120));

    area.removeFromTop(10);
    sliderVolume.setBounds(area.removeFromTop(30));
}
```

---

## Modul JUCE Utama

| Modul | Kegunaan |
|-------|----------|
| `juce_audio_basics` | Buffer audio, format MIDI |
| `juce_audio_devices` | I/O device (soundcard, MIDI) |
| `juce_audio_formats` | WAV, AIFF, MP3, OGG |
| `juce_audio_processors` | AudioProcessor (plugin) |
| `juce_audio_utils` | AudioDeviceManager, player |
| `juce_dsp` | Filter, FFT, convolution, oscillator |
| `juce_gui_basics` | Component, Desktop, Graphics |
| `juce_gui_extra` | Dialog, CodeEditor, dll |
| `juce_core` | String, File, Thread, Timer |

---

## AudioDeviceManager — Mengatur Perangkat Audio

```cpp
// Buka dialog pengaturan audio
juce::AudioDeviceManager deviceManager;
deviceManager.initialiseWithDefaultDevices(0, 2); // 0 input, 2 output

// Dialog untuk memilih soundcard
juce::AudioDeviceSelectorComponent selector(
    deviceManager,
    0, 2,    // min/max input channels
    0, 2,    // min/max output channels
    true,    // tampilkan MIDI input
    false,   // tampilkan MIDI output
    false,
    false
);
```

---

## Tips Penting JUCE

```cpp
// ✅ BENAR — Komunikasi thread-safe dari audio ke GUI
juce::MessageManager::callAsync([this, value]() {
    labelLevel.setText(juce::String(value), juce::dontSendNotification);
});

// ❌ SALAH — Jangan sentuh GUI dari audio thread!
// labelLevel.setText(...);   // ← crash!

// ✅ BENAR — Gunakan atomic untuk data thread-shared
std::atomic<float> levelMeter { 0.0f };

// ✅ BENAR — FIFO lock-free untuk audio↔GUI
juce::AbstractFifo fifo { 256 };
```

---

## Latihan

1. Buat oscillator yang bisa mengganti frekuensi (220, 440, 880 Hz) dengan tombol
2. Tambahkan pilihan waveform: Sine, Square, Sawtooth
3. Buat ADSR envelope sederhana (Attack, Decay, Sustain, Release)
4. Tampilkan level meter (VU meter) pada GUI

---

**[← Build Systems](../../06-build-systems/01-makefile/README.md)** | **[Berikutnya → Plugin Efek](../02-plugin-efek/README.md)**
