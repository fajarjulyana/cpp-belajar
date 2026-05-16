// ============================================================
// File  : oscillator_standalone.cpp
// Topik : Oscillator JUCE — contoh mandiri (pseudo-code / referensi)
// 
// CATATAN: File ini adalah referensi kode untuk belajar JUCE.
//          JUCE tidak bisa dikompilasi sebagai file tunggal biasa.
//          Gunakan Projucer atau CMake dengan library JUCE terinstal.
//
// Cara setup:
//   1. Download JUCE dari https://juce.com/get-juce/
//   2. Buka Projucer → New Project → Audio Application
//   3. Salin kode dari file ini ke Main.cpp / MainComponent.cpp
//   4. Build & Run
//
// Kompilasi bagian praktik:
//   g++ -std=c++17 -o osc oscillator_standalone.cpp
// ============================================================

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <utility>
#include <iomanip>
#include <random>
#include <algorithm>

// ====================================================
// Bagian 1: Konsep AudioBuffer
// ====================================================
// AudioBuffer<float> menyimpan sampel audio dalam memori.
// Setiap channel adalah array float dari -1.0f hingga 1.0f.

namespace Konsep {

// Ilustrasi AudioBuffer
struct AudioBufferContoh {
    // buffer[channel][sample]
    float data[2][512]; // 2 channel, 512 sampel
    int numChannels = 2;
    int numSamples  = 512;

    // Menulis sine wave ke buffer
    void isiSineWave(double frekuensi, double sampleRate, double& sudut) {
        const double deltaSudut = frekuensi * 6.283185307 / sampleRate;
        for (int s = 0; s < numSamples; s++) {
            float sampel = (float)std::sin(sudut);
            for (int ch = 0; ch < numChannels; ch++)
                data[ch][s] = sampel;
            sudut += deltaSudut;
            if (sudut > 6.283185307) sudut -= 6.283185307;
        }
    }
};

// ====================================================
// Bagian 2: Waveform Generator
// ====================================================
class OscillatorContoh {
public:
    enum class Waveform { Sine, Square, Sawtooth, Triangle };

    void setFrekuensi(double hz, double sampleRate) {
        angleDelta = hz * 6.283185307 / sampleRate;
    }

    void setWaveform(Waveform wf) { waveform = wf; }

    float nextSample() {
        float output = 0.0f;

        switch (waveform) {
        case Waveform::Sine:
            output = (float)std::sin(currentAngle);
            break;

        case Waveform::Square:
            output = (currentAngle < 3.141592653) ? 1.0f : -1.0f;
            break;

        case Waveform::Sawtooth:
            output = (float)(currentAngle / 3.141592653) - 1.0f;
            break;

        case Waveform::Triangle:
            if (currentAngle < 3.141592653)
                output = (float)(currentAngle / 1.570796327) - 1.0f;
            else
                output = 3.0f - (float)(currentAngle / 1.570796327);
            break;
        }

        currentAngle += angleDelta;
        if (currentAngle >= 6.283185307)
            currentAngle -= 6.283185307;

        return output;
    }

private:
    double currentAngle = 0.0;
    double angleDelta   = 0.0;
    Waveform waveform   = Waveform::Sine;
};

// ====================================================
// Bagian 3: ADSR Envelope
// ====================================================
struct ADSRContoh {
    float attack  = 0.01f;  // detik
    float decay   = 0.1f;   // detik
    float sustain = 0.7f;   // level 0.0 - 1.0
    float release = 0.3f;   // detik

    enum State { Idle, Attack, Decay, Sustain, Release };
    State state = Idle;

    float level      = 0.0f;
    float sampleRate = 44100.0f;

    void noteOn()  { state = Attack; }
    void noteOff() { state = Release; }

    float nextSample() {
        switch (state) {
        case Attack:
            level += 1.0f / (attack * sampleRate);
            if (level >= 1.0f) { level = 1.0f; state = Decay; }
            break;
        case Decay:
            level -= (1.0f - sustain) / (decay * sampleRate);
            if (level <= sustain) { level = sustain; state = Sustain; }
            break;
        case Sustain:
            level = sustain;
            break;
        case Release:
            level -= sustain / (release * sampleRate);
            if (level <= 0.0f) { level = 0.0f; state = Idle; }
            break;
        case Idle:
            level = 0.0f;
            break;
        }
        return level;
    }
};

// ====================================================
// Bagian 4: Simple Mixer
// ====================================================
class MixerContoh {
public:
    struct Track {
        float volume = 1.0f;
        float pan    = 0.0f;  // -1 (kiri) hingga +1 (kanan)
        bool  mute   = false;
    };

    // Menghitung gain left/right dari pan
    static std::pair<float,float> panGain(float pan) {
        float left  = (pan <= 0.0f) ? 1.0f : 1.0f - pan;
        float right = (pan >= 0.0f) ? 1.0f : 1.0f + pan;
        return {left, right};
    }
};

} // namespace Konsep


// ====================================================
// Bagian 5: Contoh MainComponent lengkap (JUCE API)
// ====================================================
/*
   Salin ke project JUCE Anda:

#pragma once
#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer {
public:
    MainComponent() {
        // Setup slider frekuensi
        sliderFreq.setRange(50.0, 2000.0, 1.0);
        sliderFreq.setValue(440.0);
        sliderFreq.setSliderStyle(juce::Slider::RotaryKnob);
        sliderFreq.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        sliderFreq.onValueChange = [this] { updateAngleDelta(); };
        addAndMakeVisible(sliderFreq);

        // Label frekuensi
        labelFreq.setText("Frekuensi (Hz)", juce::dontSendNotification);
        labelFreq.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(labelFreq);

        // Slider volume
        sliderVol.setRange(0.0, 1.0);
        sliderVol.setValue(0.5);
        sliderVol.setSliderStyle(juce::Slider::LinearVertical);
        sliderVol.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(sliderVol);

        // Pilihan waveform
        waveformBox.addItem("Sine",     1);
        waveformBox.addItem("Square",   2);
        waveformBox.addItem("Sawtooth", 3);
        waveformBox.addItem("Triangle", 4);
        waveformBox.setSelectedId(1);
        addAndMakeVisible(waveformBox);

        // Tombol power
        tombolPlay.setClickingTogglesState(true);
        tombolPlay.setButtonText("▶ ON");
        tombolPlay.onClick = [this] {
            aktif = tombolPlay.getToggleState();
            tombolPlay.setButtonText(aktif ? "■ OFF" : "▶ ON");
        };
        addAndMakeVisible(tombolPlay);

        setSize(500, 350);
        setAudioChannels(0, 2);
        startTimerHz(30); // update meter 30x/detik
    }

    ~MainComponent() override { shutdownAudio(); stopTimer(); }

    void prepareToPlay(int, double sampleRate) override {
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override {
        if (!aktif) { bufferToFill.clearActiveBufferRegion(); return; }

        float vol = (float)sliderVol.getValue();
        int   wf  = waveformBox.getSelectedId();

        auto* L = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* R = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (int i = 0; i < bufferToFill.numSamples; ++i) {
            float s = 0.0f;
            switch (wf) {
            case 1: s = (float)std::sin(currentAngle); break;
            case 2: s = (currentAngle < juce::MathConstants<double>::pi) ? 1.0f : -1.0f; break;
            case 3: s = (float)(currentAngle / juce::MathConstants<double>::pi) - 1.0f; break;
            case 4:
                s = (currentAngle < juce::MathConstants<double>::pi)
                    ? (float)(currentAngle / juce::MathConstants<double>::halfPi) - 1.0f
                    : 3.0f - (float)(currentAngle / juce::MathConstants<double>::halfPi);
                break;
            }
            L[i] = R[i] = s * vol;
            currentAngle += angleDelta;
            if (currentAngle >= juce::MathConstants<double>::twoPi)
                currentAngle -= juce::MathConstants<double>::twoPi;

            currentLevel = std::max(currentLevel.load(), std::abs(s * vol));
        }
    }

    void releaseResources() override {}

    void timerCallback() override { levelMeterLevel = currentLevel.exchange(0.0f); repaint(); }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour(0xff1e1e2e));
        g.setColour(juce::Colours::white);
        g.setFont(18.0f);
        g.drawText("JUCE Oscillator", getLocalBounds().removeFromTop(40),
                   juce::Justification::centred);

        // Level meter
        auto meterArea = getLocalBounds().removeFromRight(40).reduced(10, 50);
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(meterArea);
        g.setColour(juce::Colours::limegreen);
        float mH = meterArea.getHeight() * levelMeterLevel;
        g.fillRect(meterArea.getX(), meterArea.getBottom() - (int)mH,
                   meterArea.getWidth(), (int)mH);
        g.setColour(juce::Colours::white);
        g.drawText("VU", meterArea.withTop(meterArea.getBottom() + 2).withHeight(16),
                   juce::Justification::centred);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(50);

        auto top = area.removeFromTop(180);
        sliderFreq.setBounds(top.removeFromLeft(180).reduced(10));
        labelFreq.setBounds (top.removeFromLeft(0).withWidth(0)); // handled by slider
        sliderVol.setBounds (top.removeFromLeft(80).reduced(10));

        auto row = area.removeFromTop(40);
        waveformBox.setBounds(row.removeFromLeft(200));
        row.removeFromLeft(20);
        tombolPlay.setBounds(row.removeFromLeft(100));
    }

private:
    void updateAngleDelta() {
        angleDelta = sliderFreq.getValue() *
                     juce::MathConstants<double>::twoPi / currentSampleRate;
    }

    juce::Slider     sliderFreq, sliderVol;
    juce::Label      labelFreq;
    juce::ComboBox   waveformBox;
    juce::TextButton tombolPlay;

    double currentSampleRate = 44100.0;
    double currentAngle = 0.0, angleDelta = 0.0;
    std::atomic<float> currentLevel { 0.0f };
    float  levelMeterLevel = 0.0f;
    bool   aktif = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
*/

// ====================================================
// Bagian 6: Praktik tanpa JUCE (simulasi)
// Kode di bawah ini bisa dikompilasi langsung.
// ====================================================

struct Oscillator {
    enum class Bentuk { Sine, Square, Sawtooth, Triangle };

    double frekuensi  = 440.0;
    double sampleRate = 44100.0;
    double sudut      = 0.0;
    Bentuk bentuk     = Bentuk::Sine;

    float nextSample() {
        double delta = frekuensi * 2.0 * M_PI / sampleRate;
        float out = 0.0f;
        switch (bentuk) {
        case Bentuk::Sine:
            out = (float)std::sin(sudut); break;
        case Bentuk::Square:
            out = (sudut < M_PI) ? 1.0f : -1.0f; break;
        case Bentuk::Sawtooth:
            out = (float)(sudut / M_PI) - 1.0f; break;
        case Bentuk::Triangle:
            out = (sudut < M_PI)
                ? (float)(sudut / (M_PI/2.0)) - 1.0f
                : 3.0f - (float)(sudut / (M_PI/2.0));
            break;
        }
        sudut += delta;
        if (sudut >= 2.0 * M_PI) sudut -= 2.0 * M_PI;
        return out;
    }

    // Generate N sampel dan simpan
    std::vector<float> generate(int numSamples) {
        std::vector<float> buf(numSamples);
        for (auto& s : buf) s = nextSample();
        return buf;
    }

    // Visualisasi ASCII sederhana
    void visualize(int width = 60, int height = 11) {
        auto sampels = generate(width);
        std::cout << "\n  Waveform (frekuensi=" << frekuensi << " Hz):\n";
        for (int row = 0; row < height; row++) {
            float threshold = 1.0f - (float)(row * 2) / (height - 1);
            std::cout << "  |";
            for (float s : sampels)
                std::cout << (s >= threshold - 0.2f && s < threshold + 0.2f ? '*' : ' ');
            std::cout << "\n";
        }
        std::cout << "  +" << std::string(width, '-') << "\n";
    }
};

struct ADSR {
    float attack  = 0.01f;
    float decay   = 0.10f;
    float sustain = 0.70f;
    float release = 0.30f;
    float sr      = 44100.0f;

    enum State { Idle, Attack, Decay, Sustain, Release } state = Idle;
    float level = 0.0f;

    void noteOn()  { state = Attack; }
    void noteOff() { if (state != Idle) state = Release; }

    float next() {
        switch (state) {
        case Attack:
            level += 1.0f / (attack * sr);
            if (level >= 1.0f) { level = 1.0f; state = Decay; }
            break;
        case Decay:
            level -= (1.0f - sustain) / (decay * sr);
            if (level <= sustain) { level = sustain; state = Sustain; }
            break;
        case Sustain: break;
        case Release:
            level -= sustain / (release * sr);
            if (level <= 0.0f) { level = 0.0f; state = Idle; }
            break;
        case Idle: level = 0.0f; break;
        }
        return level;
    }
};

int main() {
    std::cout << "===================================================\n";
    std::cout << " Simulasi Oscillator JUCE (tanpa library JUCE)\n";
    std::cout << "===================================================\n";

    // 1. Tampilkan berbagai waveform
    Oscillator osc;
    osc.sampleRate = 44100.0;

    struct { Oscillator::Bentuk bentuk; std::string nama; double freq; } tests[] = {
        { Oscillator::Bentuk::Sine,     "SINE    ",  440.0 },
        { Oscillator::Bentuk::Square,   "SQUARE  ",  440.0 },
        { Oscillator::Bentuk::Sawtooth, "SAWTOOTH",  880.0 },
        { Oscillator::Bentuk::Triangle, "TRIANGLE",  440.0 },
    };

    for (auto& t : tests) {
        osc.bentuk    = t.bentuk;
        osc.frekuensi = t.freq;
        osc.sudut     = 0.0;
        std::cout << "\n[" << t.nama << " @ " << t.freq << " Hz]";
        osc.visualize(50, 9);
    }

    // 2. Simulasi ADSR
    std::cout << "\n[ADSR Envelope — 200 sampel, note on=0, off=150]\n  ";
    ADSR adsr;
    adsr.sr = 200.0f;  // rendah supaya terlihat
    adsr.attack = 0.1f; adsr.decay = 0.2f;
    adsr.sustain = 0.6f; adsr.release = 0.3f;

    std::vector<float> envelope;
    for (int i = 0; i < 200; i++) {
        if (i == 0)   adsr.noteOn();
        if (i == 150) adsr.noteOff();
        envelope.push_back(adsr.next());
    }

    // Visualisasi envelope
    for (int row = 9; row >= 0; row--) {
        float threshold = (float)row / 9.0f;
        std::cout << "  |";
        for (int i = 0; i < 60; i++) {
            int idx = i * 200 / 60;
            std::cout << (envelope[idx] >= threshold - 0.05f ? '#' : ' ');
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(62, '-') << "+\n";
    std::cout << "    A   D        S              R\n";

    // 3. Level meter simulasi
    std::cout << "\n[Simulasi Level Meter — 20 buffer]\n";
    osc.bentuk = Oscillator::Bentuk::Sine;
    osc.frekuensi = 440.0;
    osc.sudut = 0.0;
    for (int buf = 0; buf < 20; buf++) {
        auto sampels = osc.generate(512);
        float peak = 0.0f;
        for (float s : sampels) peak = std::max(peak, std::abs(s));
        int bars = (int)(peak * 40);
        std::cout << "  Buffer " << std::setw(2) << buf << ": |";
        for (int b = 0; b < 40; b++)
            std::cout << (b < bars ? (b < 28 ? '=' : (b < 36 ? '!' : 'X')) : ' ');
        std::cout << "| " << std::fixed << std::setprecision(3) << peak << "\n";
    }

    std::cout << "\n[SELESAI] Konsep di atas diimplementasikan penuh dengan JUCE API.\n";
    std::cout << "Lihat komentar /* */ di atas untuk kode MainComponent.cpp lengkap.\n\n";
    return 0;
}
