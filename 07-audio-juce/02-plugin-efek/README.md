# Plugin Efek Audio dengan JUCE

Plugin efek (Audio Effect Plugin) memproses sinyal audio yang masuk dan mengeluarkan sinyal yang sudah dimodifikasi. Contoh: Reverb, Delay, Equalizer, Compressor, Distorsi.

---

## Arsitektur Plugin JUCE

```
Host (DAW)                  Plugin
─────────          ══════════════════════════
Audio In ────────→ AudioProcessor::processBlock()
                          │
MIDI In  ────────→        ├── DSP Processing
                          │   (filter, delay, gain)
Parameter ───────→        │
Changes                   ├── createEditor()
                          │   (GUI dengan knob/slider)
                ←──────── │
Audio Out                 └── Parameter State
                              (save/load preset)
```

---

## Format Plugin yang Didukung JUCE

| Format | Platform | Keterangan |
|--------|----------|------------|
| VST3   | Win/Mac/Linux | Steinberg, paling universal |
| AU     | macOS/iOS | Apple native |
| AAX    | Win/Mac | Pro Tools (butuh Avid SDK) |
| LV2    | Linux | Open-source standard |
| Standalone | Semua | Bisa jalan tanpa DAW |

---

## CMakeLists.txt untuk Plugin

```cmake
cmake_minimum_required(VERSION 3.22)
project(EfekGain VERSION 1.0)

add_subdirectory(JUCE)

juce_add_plugin(EfekGain
    PRODUCT_NAME        "Efek Gain"
    COMPANY_NAME        "BelajarCpp"
    PLUGIN_MANUFACTURER_CODE  BCPP
    PLUGIN_CODE               EGai
    FORMATS                   VST3 Standalone
    VST3_CATEGORIES           "Fx"
    IS_SYNTH                  FALSE
    NEEDS_MIDI_INPUT          FALSE
    NEEDS_MIDI_OUTPUT         FALSE
    IS_MIDI_EFFECT            FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
)

target_sources(EfekGain
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_features(EfekGain PRIVATE cxx_std_17)

target_link_libraries(EfekGain
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
```

---

## Plugin 1: Gain (Volume Control) — Plugin Paling Sederhana

### PluginProcessor.h
```cpp
#pragma once
#include <JuceHeader.h>

class GainProcessor : public juce::AudioProcessor {
public:
    GainProcessor();

    // Wajib di-override
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Gain"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter
    juce::AudioProcessorValueTreeState parameters;

private:
    std::atomic<float>* gainParam  = nullptr;
    std::atomic<float>* muteParam  = nullptr;

    juce::dsp::Gain<float> gainDSP;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessor)
};
```

### PluginProcessor.cpp
```cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

GainProcessor::GainProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMS", createLayout())
{
    gainParam = parameters.getRawParameterValue("gain");
    muteParam = parameters.getRawParameterValue("mute");
}

juce::AudioProcessorValueTreeState::ParameterLayout GainProcessor::createLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain",                         // ID
        "Gain",                         // Nama
        juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), // Range dB
        0.0f                            // Default: 0 dB
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "mute", "Mute", false
    ));

    return { params.begin(), params.end() };
}

void GainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels      = (juce::uint32)getTotalNumOutputChannels();

    gainDSP.prepare(spec);
}

void GainProcessor::releaseResources() {}

void GainProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                  juce::MidiBuffer& /*midiMessages*/) {
    juce::ScopedNoDenormals noDenormals;

    // Jika di-mute, kosongkan buffer
    if (*muteParam >= 0.5f) {
        buffer.clear();
        return;
    }

    // Konversi dB ke gain linear
    float gainLinear = juce::Decibels::decibelsToGain(gainParam->load());
    gainDSP.setGainLinear(gainLinear);

    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto ctx   = juce::dsp::ProcessContextReplacing<float>(block);
    gainDSP.process(ctx);
}

void GainProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* GainProcessor::createEditor() {
    return new GainEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GainProcessor();
}
```

### PluginEditor.h + .cpp (GUI)
```cpp
// PluginEditor.h
#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class GainEditor : public juce::AudioProcessorEditor {
public:
    GainEditor(GainProcessor& p);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    GainProcessor& processor;

    juce::Slider   sliderGain;
    juce::Label    labelGain;
    juce::TextButton tombolMute;

    // Attachment menghubungkan slider ↔ parameter otomatis
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<ButtonAttachment> muteAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainEditor)
};

// PluginEditor.cpp
GainEditor::GainEditor(GainProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Slider gain
    sliderGain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    sliderGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainAttachment = std::make_unique<SliderAttachment>(
        processor.parameters, "gain", sliderGain);
    addAndMakeVisible(sliderGain);

    // Label
    labelGain.setText("GAIN (dB)", juce::dontSendNotification);
    labelGain.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(labelGain);

    // Tombol Mute
    tombolMute.setButtonText("MUTE");
    tombolMute.setClickingTogglesState(true);
    muteAttachment = std::make_unique<ButtonAttachment>(
        processor.parameters, "mute", tombolMute);
    addAndMakeVisible(tombolMute);

    setSize(200, 220);
}

void GainEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff2d2d30));
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("GAIN PLUGIN", getLocalBounds().removeFromTop(30),
               juce::Justification::centred);
}

void GainEditor::resized() {
    auto area = getLocalBounds().reduced(15);
    area.removeFromTop(30);

    labelGain .setBounds(area.removeFromTop(20));
    sliderGain.setBounds(area.removeFromTop(140));
    tombolMute.setBounds(area.removeFromTop(30));
}
```

---

## Plugin 2: Delay (Echo)

```cpp
// Bagian processBlock untuk Delay plugin
void DelayProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer&) {
    const float delayTime  = *parameters.getRawParameterValue("delay_time");  // ms
    const float feedback   = *parameters.getRawParameterValue("feedback");    // 0-0.9
    const float wetDry     = *parameters.getRawParameterValue("wet");         // 0-1

    const int delaySamples = (int)(delayTime * 0.001 * currentSampleRate);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        auto* data = buffer.getWritePointer(ch);
        auto& dBuf = delayBuffer[ch];  // circular buffer

        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            // Baca dari delay buffer (sinyal tunda)
            int readPos = (writePos - delaySamples + dBuf.size()) % dBuf.size();
            float delayed = dBuf[readPos];

            // Tulis ke delay buffer (masukan + feedback)
            dBuf[writePos] = data[i] + delayed * feedback;
            writePos = (writePos + 1) % dBuf.size();

            // Mix dry/wet
            data[i] = data[i] * (1.0f - wetDry) + delayed * wetDry;
        }
    }
}
```

---

## Plugin 3: Equalizer (EQ 3-Band)

```cpp
// Setup 3-band EQ menggunakan juce::dsp::IIR
void EQProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec { sampleRate,
        (juce::uint32)samplesPerBlock,
        (juce::uint32)getTotalNumOutputChannels() };

    // Low shelf: boost/cut frekuensi rendah (80 Hz)
    filterLow.reset();
    filterLow.prepare(spec);
    *filterLow.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 80.0, 0.707, juce::Decibels::decibelsToGain(0.0f));

    // Peak/Bell: frekuensi tengah (1 kHz)
    filterMid.reset();
    filterMid.prepare(spec);
    *filterMid.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 1000.0, 0.707, juce::Decibels::decibelsToGain(0.0f));

    // High shelf: frekuensi tinggi (8 kHz)
    filterHigh.reset();
    filterHigh.prepare(spec);
    *filterHigh.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 8000.0, 0.707, juce::Decibels::decibelsToGain(0.0f));
}

void EQProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                juce::MidiBuffer&) {
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto ctx   = juce::dsp::ProcessContextReplacing<float>(block);

    filterLow .process(ctx);
    filterMid .process(ctx);
    filterHigh.process(ctx);
}
```

---

## Komponen GUI Kustom — Knob ala Plugin Profesional

```cpp
class KnobKustom : public juce::Slider {
public:
    KnobKustom() {
        setSliderStyle(juce::Slider::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat().reduced(5);
        auto center = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

        // Lingkaran latar
        g.setColour(juce::Colour(0xff3a3a3a));
        g.fillEllipse(center.x - radius, center.y - radius,
                       radius * 2, radius * 2);

        // Arc nilai
        float startAngle = juce::MathConstants<float>::pi * 1.25f;
        float endAngle   = juce::MathConstants<float>::pi * 2.75f;
        float normalVal  = (float)valueToProportionOfLength(getValue());
        float valueAngle = startAngle + normalVal * (endAngle - startAngle);

        juce::Path arc;
        arc.addArc(center.x - radius * 0.8f, center.y - radius * 0.8f,
                    radius * 1.6f, radius * 1.6f,
                    startAngle, valueAngle, true);

        g.setColour(juce::Colour(0xff00aaff));
        g.strokePath(arc, juce::PathStrokeType(3.0f));

        // Indikator (garis)
        g.setColour(juce::Colours::white);
        auto tip = center.getPointOnCircumference(radius * 0.7f, valueAngle);
        g.drawLine({ center, tip }, 2.5f);
    }
};
```

---

## Menyimpan & Memuat Preset

```cpp
// Simpan preset ke XML
void PluginProcessor::savePreset(const juce::File& file) {
    auto state = parameters.copyState();
    auto xml = state.createXml();
    xml->writeToFile(file, {});
}

// Muat preset dari XML
void PluginProcessor::loadPreset(const juce::File& file) {
    auto xml = juce::XmlDocument::parse(file);
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
```

---

## Latihan

1. Buat plugin **Chorus** — delay pendek (10-50ms) dengan modulasi LFO
2. Buat plugin **Compressor** — deteksi RMS level, terapkan gain reduction
3. Buat plugin **Distorsi** — soft clip, hard clip, dan waveshaping
4. Tambahkan **spectrum analyzer** menggunakan `juce::dsp::FFT`

---

**[← Pengenalan JUCE](../01-pengenalan/README.md)** | **[Berikutnya → Synthesizer](../03-synthesizer/README.md)**
