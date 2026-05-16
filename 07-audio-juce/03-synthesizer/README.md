# Synthesizer dengan JUCE

Synthesizer menghasilkan suara dari nol (tidak memproses audio yang sudah ada). Ini adalah proyek paling menantang dan paling menarik dalam pemrograman audio.

---

## Arsitektur Synthesizer

```
MIDI Input
    │
    ▼
┌─────────────────────────────────────────────┐
│  VOICE MANAGER (polifoni 8-16 suara)        │
│  ┌──────────────┐  ┌──────────────┐         │
│  │   Voice 1    │  │   Voice 2    │  ...    │
│  │ ┌──────────┐ │  │ ┌──────────┐ │         │
│  │ │Oscillator│ │  │ │Oscillator│ │         │
│  │ └────┬─────┘ │  │ └────┬─────┘ │         │
│  │ ┌────▼─────┐ │  │ ┌────▼─────┐ │         │
│  │ │  Filter  │ │  │ │  Filter  │ │         │
│  │ └────┬─────┘ │  │ └────┬─────┘ │         │
│  │ ┌────▼─────┐ │  │ ┌────▼─────┐ │         │
│  │ │  ADSR    │ │  │ │  ADSR    │ │         │
│  │ └──────────┘ │  │ └──────────┘ │         │
│  └──────────────┘  └──────────────┘         │
└─────────────────────────────────────────────┘
    │
    ▼
┌───────────────┐
│ Master Volume │
│ Reverb / FX   │
└───────────────┘
    │
    ▼
Audio Output
```

---

## Komponen Utama Synthesizer

### 1. Oscillator

```cpp
class SynthOscillator {
public:
    enum class Waveform { Sine, Square, Sawtooth, Triangle, Noise };

    void setFrequency(double hz, double sampleRate) {
        phaseDelta = hz / sampleRate;
    }

    void setWaveform(Waveform wf) { waveform = wf; }

    void setPulseWidth(float pw) { pulseWidth = pw; }  // untuk square

    float getNextSample() {
        float output = 0.0f;

        switch (waveform) {
        case Waveform::Sine:
            output = std::sin(phase * juce::MathConstants<float>::twoPi);
            break;

        case Waveform::Square:
            output = (phase < pulseWidth) ? 1.0f : -1.0f;
            break;

        case Waveform::Sawtooth:
            output = 2.0f * phase - 1.0f;
            break;

        case Waveform::Triangle:
            output = (phase < 0.5f) ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
            break;

        case Waveform::Noise:
            output = random.nextFloat() * 2.0f - 1.0f;
            break;
        }

        phase += phaseDelta;
        if (phase >= 1.0) phase -= 1.0;

        return output;
    }

    void reset() { phase = 0.0; }

private:
    double phase      = 0.0;
    double phaseDelta = 0.0;
    float  pulseWidth = 0.5f;
    Waveform waveform = Waveform::Sine;
    juce::Random random;
};
```

### 2. ADSR Envelope

```cpp
class SynthEnvelope {
public:
    struct Params {
        float attack  = 0.01f;  // detik
        float decay   = 0.1f;   // detik
        float sustain = 0.7f;   // level (0-1)
        float release = 0.3f;   // detik
    };

    void setSampleRate(double sr) { sampleRate = (float)sr; }
    void setParams(const Params& p) { params = p; }

    void noteOn()  { state = State::Attack; }
    void noteOff() { if (state != State::Idle) state = State::Release; }

    bool isActive() const { return state != State::Idle; }
    float getCurrentLevel() const { return currentLevel; }

    float getNextSample() {
        switch (state) {
        case State::Attack:
            currentLevel += 1.0f / (params.attack * sampleRate);
            if (currentLevel >= 1.0f) { currentLevel = 1.0f; state = State::Decay; }
            break;

        case State::Decay:
            currentLevel -= (1.0f - params.sustain) / (params.decay * sampleRate);
            if (currentLevel <= params.sustain) { currentLevel = params.sustain; state = State::Sustain; }
            break;

        case State::Sustain:
            break;

        case State::Release:
            currentLevel -= params.sustain / (params.release * sampleRate);
            if (currentLevel <= 0.001f) { currentLevel = 0.0f; state = State::Idle; }
            break;

        case State::Idle:
            currentLevel = 0.0f;
            break;
        }

        return currentLevel;
    }

private:
    enum class State { Idle, Attack, Decay, Sustain, Release };
    State state = State::Idle;
    float currentLevel = 0.0f;
    float sampleRate   = 44100.0f;
    Params params;
};
```

### 3. Filter (Low-Pass dengan juce::dsp)

```cpp
class SynthFilter {
public:
    void prepare(double sampleRate, int blockSize) {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate       = sampleRate;
        spec.maximumBlockSize = (juce::uint32)blockSize;
        spec.numChannels      = 1;
        filter.prepare(spec);
    }

    void setCutoff(float freqHz, float resonance) {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            currentSampleRate, freqHz, resonance);
    }

    float processSample(float input) {
        return filter.processSample(input);
    }

    void reset() { filter.reset(); }

private:
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>> filter;
    double currentSampleRate = 44100.0;
};
```

### 4. LFO (Low-Frequency Oscillator) — Modulasi

```cpp
class LFO {
public:
    void setSampleRate(double sr) {
        sampleRate = sr;
    }

    void setRate(float hz) {
        phaseDelta = hz / (float)sampleRate;
    }

    void setDepth(float d) { depth = d; }  // 0.0 - 1.0

    float getNextSample() {
        float output = std::sin(phase * juce::MathConstants<float>::twoPi) * depth;
        phase += phaseDelta;
        if (phase >= 1.0f) phase -= 1.0f;
        return output;
    }

private:
    double sampleRate  = 44100.0;
    float  phase       = 0.0f;
    float  phaseDelta  = 0.0f;
    float  depth       = 0.5f;
};
```

---

## SynthVoice — Satu Suara (Note)

```cpp
class SynthVoice : public juce::SynthesiserVoice {
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override {
        double freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        osc1.setFrequency(freq, getSampleRate());
        osc2.setFrequency(freq * 1.005, getSampleRate());  // sedikit detuned
        env.noteOn();
        currentVelocity = velocity;
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override {
        env.noteOff();
        if (!allowTailOff) clearCurrentNote();
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override {
        for (int sample = startSample; sample < startSample + numSamples; ++sample) {
            float envLevel = env.getNextSample();

            if (!env.isActive()) {
                clearCurrentNote();
                return;
            }

            // LFO modulasi pitch
            float lfoValue = lfo.getNextSample();
            osc1.setFrequency(baseFreq * (1.0 + lfoValue * 0.01), getSampleRate());

            // Mix dua oscillator
            float rawSample = (osc1.getNextSample() * 0.6f +
                               osc2.getNextSample() * 0.4f);

            // Terapkan envelope dan velocity
            float finalSample = rawSample * envLevel * currentVelocity * masterVolume;

            // Tulis ke buffer (stereo)
            for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
                outputBuffer.addSample(ch, sample, finalSample);
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void setWaveform(SynthOscillator::Waveform wf) {
        osc1.setWaveform(wf);
        osc2.setWaveform(wf);
    }

    void setADSR(const SynthEnvelope::Params& p) { env.setParams(p); }
    void setMasterVolume(float v) { masterVolume = v; }

    void prepare(double sr, int blockSize) {
        env.setSampleRate(sr);
        lfo.setSampleRate(sr);
    }

private:
    SynthOscillator osc1, osc2;
    SynthEnvelope   env;
    LFO             lfo;

    double baseFreq       = 440.0;
    float  currentVelocity = 1.0f;
    float  masterVolume   = 0.8f;
};
```

---

## SynthProcessor — Plugin/App Processor Lengkap

```cpp
class SynthProcessor : public juce::AudioProcessor {
public:
    SynthProcessor()
        : AudioProcessor(BusesProperties()
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          apvts(*this, nullptr, "PARAMS", createLayout())
    {
        // Daftarkan suara dan voice ke synthesiser
        synth.addSound(new SynthSound());
        for (int i = 0; i < MAX_VOICES; ++i)
            synth.addVoice(new SynthVoice());
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
        synth.setCurrentPlaybackSampleRate(sampleRate);
        for (int i = 0; i < synth.getNumVoices(); ++i)
            if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                voice->prepare(sampleRate, samplesPerBlock);
    }

    void processBlock(juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages) override {
        juce::ScopedNoDenormals noDenormals;
        buffer.clear();

        // Update parameter dari GUI
        updateParameters();

        // Render MIDI → Audio
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

        // Master volume
        buffer.applyGain(juce::Decibels::decibelsToGain(
            apvts.getRawParameterValue("master_vol")->load()));
    }

    // ... (getStateInformation, setStateInformation, dll)

private:
    static constexpr int MAX_VOICES = 8;
    juce::Synthesiser synth;
    juce::AudioProcessorValueTreeState apvts;

    void updateParameters() {
        float attack  = apvts.getRawParameterValue("attack") ->load();
        float decay   = apvts.getRawParameterValue("decay")  ->load();
        float sustain = apvts.getRawParameterValue("sustain")->load();
        float release = apvts.getRawParameterValue("release")->load();

        SynthEnvelope::Params envParams { attack, decay, sustain, release };
        for (int i = 0; i < synth.getNumVoices(); ++i)
            if (auto* v = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
                v->setADSR(envParams);
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout() {
        using P = juce::AudioParameterFloat;
        using NR = juce::NormalisableRange<float>;
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

        p.push_back(std::make_unique<P>("attack",     "Attack",
            NR(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
        p.push_back(std::make_unique<P>("decay",      "Decay",
            NR(0.001f, 5.0f, 0.001f, 0.3f), 0.1f));
        p.push_back(std::make_unique<P>("sustain",    "Sustain",
            NR(0.0f, 1.0f), 0.7f));
        p.push_back(std::make_unique<P>("release",    "Release",
            NR(0.001f, 10.0f, 0.001f, 0.3f), 0.3f));
        p.push_back(std::make_unique<P>("cutoff",     "Cutoff",
            NR(20.0f, 20000.0f, 1.0f, 0.3f), 5000.0f));
        p.push_back(std::make_unique<P>("resonance",  "Resonance",
            NR(0.1f, 10.0f), 0.707f));
        p.push_back(std::make_unique<P>("master_vol", "Volume",
            NR(-60.0f, 6.0f), -6.0f));

        return { p.begin(), p.end() };
    }
};
```

---

## GUI Synthesizer — Keyboard Virtual MIDI

```cpp
class PianoKeyboard : public juce::Component,
                       public juce::MidiKeyboardState::Listener {
public:
    PianoKeyboard(juce::MidiKeyboardState& state)
        : keyboardComponent(state, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible(keyboardComponent);
        keyboardComponent.setOctaveForMiddleC(4);
        keyboardComponent.setLowestVisibleKey(36);
        keyboardComponent.setAvailableRange(36, 96);
    }

    void resized() override {
        keyboardComponent.setBounds(getLocalBounds());
    }

private:
    juce::MidiKeyboardComponent keyboardComponent;
};
```

---

## Teknik Lanjutan

### Supersaw Oscillator (EDM)
```cpp
// 7 oscillator detuned sedikit — suara "rave" klasik
class SuperSaw {
    static constexpr int NUM_OSC = 7;
    SynthOscillator oscs[NUM_OSC];
    // detune: -0.1, -0.07, -0.03, 0, +0.03, +0.07, +0.1 semitone
    static constexpr float detune[] = { -0.1f, -0.07f, -0.03f, 0.0f, 0.03f, 0.07f, 0.1f };
public:
    void setFrequency(double hz, double sr) {
        for (int i = 0; i < NUM_OSC; ++i) {
            double detunedHz = hz * std::pow(2.0, detune[i] / 12.0);
            oscs[i].setFrequency(detunedHz, sr);
            oscs[i].setWaveform(SynthOscillator::Waveform::Sawtooth);
        }
    }
    float getNextSample() {
        float mix = 0.0f;
        for (auto& o : oscs) mix += o.getNextSample();
        return mix / NUM_OSC;
    }
};
```

### FM Synthesis (Frequency Modulation)
```cpp
// Carrier dimodulasi oleh Modulator
float getFMSample(double carrierFreq, double modFreq,
                   double modDepth, double sr) {
    static double modPhase = 0.0, carPhase = 0.0;

    // Modulator output
    float modOut = (float)(std::sin(modPhase * 2 * M_PI) * modDepth);
    modPhase += modFreq / sr;
    if (modPhase >= 1.0) modPhase -= 1.0;

    // Carrier dengan frekuensi yang dimodulasi
    float carOut = (float)std::sin((carPhase + modOut) * 2 * M_PI);
    carPhase += carrierFreq / sr;
    if (carPhase >= 1.0) carPhase -= 1.0;

    return carOut;
}
```

---

## Latihan

1. Implementasikan **Portamento** (glide antara nada)
2. Tambahkan **Arpeggiator** (mainkan nada akord secara berurutan)
3. Buat **Patch Preset System** — simpan/muat pengaturan synthesizer
4. Implementasikan **Wavetable Synthesis** — baca dari array waveform

---

**[← Plugin Efek](../02-plugin-efek/README.md)** | **[Berikutnya → Proyek](../../08-proyek/01-sederhana/README.md)**
