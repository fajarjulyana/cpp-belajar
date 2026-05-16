// ============================================================
// File  : synthesizer_sim.cpp
// Topik : Simulasi Synthesizer (bisa dikompilasi tanpa JUCE)
//
// Kompilasi:
//   g++ -std=c++17 -o synth synthesizer_sim.cpp
//
// Fitur simulasi:
//   - Oscillator (Sine, Square, Sawtooth, Triangle)
//   - ADSR Envelope
//   - LFO modulasi
//   - Simple Filter
//   - Voice Manager (polifoni)
//   - Render ke WAV (opsional)
// ============================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <string>
#include <array>
#include <random>
#include <map>

// ============================================================
// Konstanta & Utilitas
// ============================================================
static constexpr double SR        = 44100.0;  // Sample rate
static constexpr double TWO_PI    = 6.283185307179586;
static constexpr int    BLOCK_SIZE = 256;     // Sampel per buffer

float dbToLinear(float dB) { return std::pow(10.0f, dB / 20.0f); }
float linearToDb(float lin) { return 20.0f * std::log10(lin + 1e-9f); }

// MIDI note number → frekuensi (A4=69=440Hz)
double midiToHz(int note) {
    return 440.0 * std::pow(2.0, (note - 69) / 12.0);
}

// ============================================================
// Oscillator
// ============================================================
class Oscillator {
public:
    enum class Bentuk { Sine, Square, Sawtooth, Triangle, Noise };

    void setFrekuensi(double hz) {
        freq = hz;
        delta = hz / SR;
    }

    void setBentuk(Bentuk b) { bentuk = b; }

    void setPulseWidth(float pw) {
        pulseWidth = std::clamp(pw, 0.05f, 0.95f);
    }

    float next() {
        float out = 0.0f;
        switch (bentuk) {
        case Bentuk::Sine:
            out = std::sin(phase * TWO_PI); break;
        case Bentuk::Square:
            out = (phase < pulseWidth) ? 1.0f : -1.0f; break;
        case Bentuk::Sawtooth:
            out = 2.0f * (float)phase - 1.0f; break;
        case Bentuk::Triangle:
            out = (phase < 0.5f) ? (4.0f*(float)phase - 1.0f)
                                 : (3.0f - 4.0f*(float)phase); break;
        case Bentuk::Noise:
            out = rng(engine); break;
        }
        phase += delta;
        if (phase >= 1.0) phase -= 1.0;
        return out;
    }

    void reset() { phase = 0.0; }

private:
    double freq = 440.0, delta = 440.0 / SR, phase = 0.0;
    float  pulseWidth = 0.5f;
    Bentuk bentuk = Bentuk::Sine;
    std::mt19937 engine { std::random_device{}() };
    std::uniform_real_distribution<float> rng { -1.0f, 1.0f };
};

// ============================================================
// ADSR Envelope
// ============================================================
class ADSR {
public:
    struct Params {
        float attack  = 0.01f;
        float decay   = 0.1f;
        float sustain = 0.7f;
        float release = 0.3f;
    };

    void setParams(const Params& p) { params = p; }
    void noteOn()  { state = State::Attack; }
    void noteOff() { if (state != State::Idle) { releaseFrom = level; state = State::Release; } }
    bool isActive() const { return state != State::Idle; }
    float getLevel() const { return level; }

    float next() {
        const float srf = (float)SR;
        switch (state) {
        case State::Attack:
            level += 1.0f / (params.attack * srf);
            if (level >= 1.0f) { level = 1.0f; state = State::Decay; }
            break;
        case State::Decay:
            level -= (1.0f - params.sustain) / (params.decay * srf);
            if (level <= params.sustain) { level = params.sustain; state = State::Sustain; }
            break;
        case State::Sustain:
            break;
        case State::Release:
            level -= releaseFrom / (params.release * srf);
            if (level <= 0.001f) { level = 0.0f; state = State::Idle; }
            break;
        case State::Idle:
            level = 0.0f;
            break;
        }
        return level;
    }

private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State state = State::Idle;
    float level = 0.0f, releaseFrom = 0.7f;
    Params params;
};

// ============================================================
// LFO
// ============================================================
class LFO {
public:
    void setRate(float hz) { delta = hz / (float)SR; }
    void setDepth(float d) { depth = std::clamp(d, 0.0f, 1.0f); }

    float next() {
        float out = std::sin(phase * TWO_PI) * depth;
        phase += delta;
        if (phase >= 1.0f) phase -= 1.0f;
        return out;
    }

private:
    float phase = 0.0f, delta = 0.0f, depth = 0.5f;
};

// ============================================================
// Simple Low-Pass Filter (one-pole)
// ============================================================
class LowPassFilter {
public:
    void setCutoff(float freqHz) {
        float omega = freqHz / (float)SR;
        // Bilinear coeff
        a = std::exp(-2.0f * (float)M_PI * omega);
        b = 1.0f - a;
    }

    float process(float x) {
        y = b * x + a * y;
        return y;
    }

    void reset() { y = 0.0f; }

private:
    float a = 0.99f, b = 0.01f, y = 0.0f;
};

// ============================================================
// Voice — Satu Suara (MIDI note)
// ============================================================
class Voice {
public:
    bool isActive() const { return env.isActive(); }

    void noteOn(int midiNote, float velocity) {
        note = midiNote;
        vel  = velocity;
        double hz = midiToHz(midiNote);
        osc1.setFrekuensi(hz);
        osc2.setFrekuensi(hz * detune);  // sedikit detuned
        osc1.reset(); osc2.reset();
        env.noteOn();
        filter.setCutoff(filterCutoff);
    }

    void noteOff() { env.noteOff(); }

    float render() {
        float envLevel = env.next();
        if (!env.isActive()) return 0.0f;

        // LFO modulasi pitch
        float lfoVal = pitchLFO.next();
        double modHz = midiToHz(note) * (1.0 + lfoVal * 0.005);
        osc1.setFrekuensi(modHz);
        osc2.setFrekuensi(modHz * detune);

        // Mix oscillator
        float raw = osc1.next() * osc1Mix + osc2.next() * osc2Mix;

        // Filter
        float filtered = filter.process(raw);

        return filtered * envLevel * vel * volume;
    }

    // Konfigurasi
    void setADSR(const ADSR::Params& p) { env.setParams(p); }
    void setOsc1Bentuk(Oscillator::Bentuk b) { osc1.setBentuk(b); }
    void setOsc2Bentuk(Oscillator::Bentuk b) { osc2.setBentuk(b); }
    void setDetune(float d) { detune = d; }
    void setMix(float m1, float m2) { osc1Mix = m1; osc2Mix = m2; }
    void setFilterCutoff(float hz) { filterCutoff = hz; filter.setCutoff(hz); }
    void setPitchLFO(float rate, float depth) { pitchLFO.setRate(rate); pitchLFO.setDepth(depth); }
    void setVolume(float v) { volume = v; }

    int  getNoteNumber() const { return note; }

private:
    Oscillator osc1, osc2;
    ADSR       env;
    LowPassFilter filter;
    LFO        pitchLFO;

    int   note = 69;
    float vel  = 1.0f;
    float volume    = 0.8f;
    float detune    = 1.003f;
    float osc1Mix   = 0.6f;
    float osc2Mix   = 0.4f;
    float filterCutoff = 5000.0f;
};

// ============================================================
// VoiceManager — Polifoni
// ============================================================
class VoiceManager {
    static constexpr int MAX_VOICES = 8;
public:
    VoiceManager() {
        for (auto& v : voices) {
            v.setADSR(adsrParams);
            v.setOsc1Bentuk(osc1Bentuk);
            v.setOsc2Bentuk(osc2Bentuk);
            v.setDetune(detune);
            v.setFilterCutoff(filterCutoff);
        }
    }

    void noteOn(int note, float vel) {
        // Cari voice kosong atau steal yang paling lama
        Voice* target = nullptr;
        for (auto& v : voices)
            if (!v.isActive()) { target = &v; break; }
        if (!target) target = &voices[stealIdx++ % MAX_VOICES];
        target->noteOn(note, vel);
    }

    void noteOff(int note) {
        for (auto& v : voices)
            if (v.getNoteNumber() == note && v.isActive())
                v.noteOff();
    }

    float renderNextSample() {
        float out = 0.0f;
        for (auto& v : voices)
            if (v.isActive()) out += v.render();
        return out * masterVolume;
    }

    // Set params
    void setADSR(ADSR::Params p) {
        adsrParams = p;
        for (auto& v : voices) v.setADSR(p);
    }
    void setOsc1Bentuk(Oscillator::Bentuk b) {
        osc1Bentuk = b;
        for (auto& v : voices) v.setOsc1Bentuk(b);
    }
    void setFilterCutoff(float hz) {
        filterCutoff = hz;
        for (auto& v : voices) v.setFilterCutoff(hz);
    }
    void setMasterVolume(float v) { masterVolume = v; }
    void setDetune(float d) {
        detune = d;
        for (auto& v : voices) v.setDetune(d);
    }

private:
    std::array<Voice, MAX_VOICES> voices;
    ADSR::Params        adsrParams;
    Oscillator::Bentuk  osc1Bentuk = Oscillator::Bentuk::Sawtooth;
    Oscillator::Bentuk  osc2Bentuk = Oscillator::Bentuk::Sawtooth;
    float filterCutoff  = 5000.0f;
    float masterVolume  = 0.7f;
    float detune        = 1.003f;
    int   stealIdx      = 0;
};

// ============================================================
// Render Audio ke vektor float
// ============================================================
std::vector<float> renderSequence(VoiceManager& vm,
    const std::vector<std::tuple<int,int,float,float>>& notes) {
    // notes: (midiNote, onSample, durSamples, velocity)
    int totalSamples = 0;
    for (auto& [n, on, dur, vel] : notes)
        totalSamples = std::max(totalSamples, (int)(on + dur + SR * 0.5));

    std::vector<float> output(totalSamples, 0.0f);
    std::map<int, int> noteOffSchedule; // sample → note

    for (auto& [n, on, dur, vel] : notes)
        noteOffSchedule[(int)(on + dur)] = n;

    for (int s = 0; s < totalSamples; s++) {
        // Note on
        for (auto& [n, on, dur, vel] : notes)
            if (on == s) vm.noteOn(n, vel);
        // Note off
        auto it = noteOffSchedule.find(s);
        if (it != noteOffSchedule.end()) vm.noteOff(it->second);

        output[s] = vm.renderNextSample();
    }
    return output;
}

// ============================================================
// Visualisasi
// ============================================================
void plotWave(const std::vector<float>& buf, const std::string& judul,
              int startSample = 0, int length = 200, int height = 9) {
    float peak = 0.0f;
    for (float s : buf) peak = std::max(peak, std::abs(s));
    if (peak < 1e-6f) { std::cout << "  [" << judul << "] — senyap\n"; return; }

    std::cout << "\n  [" << judul << "] Peak=" << std::fixed
              << std::setprecision(3) << peak << "\n";

    for (int row = 0; row < height; row++) {
        float thr = 1.0f - (float)(row * 2) / (height - 1);
        std::cout << "  |";
        for (int x = 0; x < 60; x++) {
            int idx = startSample + (int)((float)x * length / 60);
            idx = std::min(idx, (int)buf.size() - 1);
            float s = buf[idx] / peak;
            char c = (s >= thr - 0.12f && s < thr + 0.12f) ? '*' : ' ';
            if (std::abs(thr) < 0.05f) c = (c == ' ' ? '-' : c);
            std::cout << c;
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(60, '-') << "+\n";
}

void plotEnvelope(int durationMs = 500) {
    ADSR adsr;
    ADSR::Params p { 0.05f, 0.15f, 0.6f, 0.20f };
    adsr.setParams(p);
    adsr.noteOn();

    int total = (int)(SR * durationMs / 1000.0);
    std::vector<float> env(total);
    int noteOffAt = total * 6 / 10;

    for (int i = 0; i < total; i++) {
        if (i == noteOffAt) adsr.noteOff();
        env[i] = adsr.next();
    }

    std::cout << "\n  [ADSR Envelope] A=" << p.attack << "s  D=" << p.decay
              << "s  S=" << p.sustain << "  R=" << p.release << "s\n";
    for (int row = 9; row >= 0; row--) {
        float thr = (float)row / 9.0f;
        std::cout << "  |";
        for (int x = 0; x < 60; x++) {
            int idx = x * total / 60;
            char c = (env[idx] >= thr - 0.055f) ? '#' : ' ';
            if (idx == noteOffAt * 60 / total) c = '|';
            std::cout << c;
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(60, '-') << "+\n";
    std::cout << "    Attack  Decay  Sustain    |NoteOff|  Release\n";
}

// ============================================================
// Main
// ============================================================
int main() {
    std::cout << "=============================================\n";
    std::cout << "  Simulasi Synthesizer JUCE (C++ Murni)\n";
    std::cout << "=============================================\n";

    // 1. ADSR Envelope
    std::cout << "\n--- 1. ADSR ENVELOPE ---\n";
    plotEnvelope(600);

    // 2. Waveform oscillator
    std::cout << "\n--- 2. WAVEFORM OSCILLATOR ---\n";
    struct { Oscillator::Bentuk b; const char* name; } waveforms[] = {
        { Oscillator::Bentuk::Sine,     "Sine    " },
        { Oscillator::Bentuk::Square,   "Square  " },
        { Oscillator::Bentuk::Sawtooth, "Sawtooth" },
        { Oscillator::Bentuk::Triangle, "Triangle" },
    };
    for (auto& wf : waveforms) {
        Oscillator osc;
        osc.setFrekuensi(220.0);
        osc.setBentuk(wf.b);
        std::vector<float> buf(300);
        for (auto& s : buf) s = osc.next();
        std::cout << "\n  " << wf.name << " @ 220 Hz:";
        // Mini plot inline
        std::cout << "\n  |";
        for (int x = 0; x < 60; x++) {
            int idx = x * 300 / 60;
            float v = buf[idx];
            char c = (v > 0.5f) ? '#' : (v < -0.5f ? '_' : (std::abs(v) < 0.1f ? '-' : '.'));
            std::cout << c;
        }
        std::cout << "|\n";
    }

    // 3. Satu Voice
    std::cout << "\n--- 3. SATU VOICE (A4 = 440 Hz) ---\n";
    {
        Voice voice;
        ADSR::Params p { 0.02f, 0.1f, 0.7f, 0.15f };
        voice.setADSR(p);
        voice.setOsc1Bentuk(Oscillator::Bentuk::Sawtooth);
        voice.setFilterCutoff(3000.0f);
        voice.noteOn(69, 0.8f);  // A4

        int dur = (int)(SR * 0.3);
        int rel = (int)(SR * 0.3);
        std::vector<float> buf(dur + rel);
        for (int i = 0; i < dur; i++) buf[i] = voice.render();
        voice.noteOff();
        for (int i = dur; i < dur + rel; i++) buf[i] = voice.render();

        plotWave(buf, "Voice A4 — Sawtooth + Filter 3kHz");
    }

    // 4. Polifoni — akord
    std::cout << "\n--- 4. POLIFONI — AKORD C MAYOR ---\n";
    {
        VoiceManager vm;
        vm.setOsc1Bentuk(Oscillator::Bentuk::Sine);
        vm.setADSR({ 0.005f, 0.08f, 0.8f, 0.2f });
        vm.setFilterCutoff(8000.0f);

        // C Mayor: C4(60), E4(64), G4(67)
        const int durS = (int)(SR * 0.4);
        const int relS = (int)(SR * 0.3);
        std::vector<float> buf(durS + relS, 0.0f);

        // Render manual (noteOn semua → render → noteOff semua → render)
        vm.noteOn(60, 0.7f);  // C4
        vm.noteOn(64, 0.7f);  // E4
        vm.noteOn(67, 0.7f);  // G4
        for (int i = 0; i < durS; i++) buf[i] = vm.renderNextSample();
        vm.noteOff(60); vm.noteOff(64); vm.noteOff(67);
        for (int i = durS; i < (int)buf.size(); i++) buf[i] = vm.renderNextSample();

        plotWave(buf, "Akord C Mayor (C4-E4-G4), Sine");

        std::cout << "\n  Level detail per 50ms:\n";
        float maxV = 0.0f;
        for (float s : buf) maxV = std::max(maxV, std::abs(s));
        for (int seg = 0; seg < 14; seg++) {
            int from = seg * (int)SR / 20;
            int to   = std::min(from + (int)SR / 20, (int)buf.size());
            float peak = 0.0f;
            for (int i = from; i < to; i++) peak = std::max(peak, std::abs(buf[i]));
            int bars = (int)(peak / maxV * 30);
            std::cout << "  " << std::setw(4) << (seg * 50) << "ms |"
                      << std::string(bars, '=')
                      << std::string(30 - bars, ' ')
                      << "| " << std::fixed << std::setprecision(3) << peak << "\n";
        }
    }

    // 5. Melodi sederhana
    std::cout << "\n--- 5. MELODI 'DO RE MI' ---\n";
    {
        VoiceManager vm;
        vm.setOsc1Bentuk(Oscillator::Bentuk::Triangle);
        vm.setADSR({ 0.01f, 0.05f, 0.8f, 0.1f });
        vm.setFilterCutoff(6000.0f);
        vm.setMasterVolume(0.6f);

        // Do Re Mi Fa Sol La Si Do = C4 D4 E4 F4 G4 A4 B4 C5
        const int noteMillis[] = {60,62,64,65,67,69,71,72};
        const int durMs = 200;

        std::cout << "\n  Nada yang dimainkan:\n";
        const char* namaNote[] = {"C4","D4","E4","F4","G4","A4","B4","C5"};
        for (int i = 0; i < 8; i++) {
            double hz = midiToHz(noteMillis[i]);
            std::cout << "  " << namaNote[i] << " (" << noteMillis[i]
                      << ") = " << std::fixed << std::setprecision(2) << hz << " Hz\n";
        }

        int stepSamples = (int)(SR * durMs / 1000.0);
        int totalSamples = stepSamples * 8 + (int)(SR * 0.5);
        std::vector<float> melody(totalSamples, 0.0f);

        for (int i = 0; i < 8; i++) {
            vm.noteOn(noteMillis[i], 0.8f);
            int startS = i * stepSamples;
            int endS   = startS + (int)(stepSamples * 0.85);
            for (int s = startS; s < endS && s < totalSamples; s++)
                melody[s] = vm.renderNextSample();
            vm.noteOff(noteMillis[i]);
            for (int s = endS; s < startS + stepSamples && s < totalSamples; s++)
                melody[s] = vm.renderNextSample();
        }
        // Release tail
        for (int s = 8 * stepSamples; s < totalSamples; s++)
            melody[s] = vm.renderNextSample();

        plotWave(melody, "Melodi Do Re Mi — Triangle Wave");
    }

    // 6. Ringkasan Synthesizer
    std::cout << "\n=============================================\n";
    std::cout << "  Komponen Synthesizer:\n";
    std::cout << "  ┌─────────────┬──────────────────────────┐\n";
    std::cout << "  │ Komponen    │ Peran                    │\n";
    std::cout << "  ├─────────────┼──────────────────────────┤\n";
    std::cout << "  │ Oscillator  │ Sumber suara (waveform)  │\n";
    std::cout << "  │ ADSR        │ Bentuk amplitudo waktu   │\n";
    std::cout << "  │ Filter      │ Warna timbre (EQ)        │\n";
    std::cout << "  │ LFO         │ Modulasi parameter       │\n";
    std::cout << "  │ VoiceMgr    │ Polifoni (note stacking) │\n";
    std::cout << "  └─────────────┴──────────────────────────┘\n\n";

    return 0;
}
