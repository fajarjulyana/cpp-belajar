// ============================================================
// File  : plugin_gain.cpp
// Topik : Simulasi Plugin Gain (bisa dikompilasi tanpa JUCE)
//
// Kompilasi:
//   g++ -std=c++17 -o plugin_gain plugin_gain.cpp
//
// Untuk plugin JUCE nyata, lihat README.md dan setup project
// menggunakan Projucer atau CMake + JUCE library.
// ============================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <string>
#include <numeric>

// ====================================================
// Simulasi AudioBuffer (tanpa JUCE)
// ====================================================
struct AudioBuffer {
    std::vector<std::vector<float>> channels;
    int numChannels;
    int numSamples;

    AudioBuffer(int ch, int samples)
        : channels(ch, std::vector<float>(samples, 0.0f)),
          numChannels(ch), numSamples(samples) {}

    float* getWritePointer(int ch) { return channels[ch].data(); }
    const float* getReadPointer(int ch) const { return channels[ch].data(); }

    void clear() {
        for (auto& ch : channels)
            std::fill(ch.begin(), ch.end(), 0.0f);
    }

    // Isi dengan sine wave (untuk test)
    void fillSine(double frekuensi, double sampleRate, float amplitude = 0.8f) {
        for (int s = 0; s < numSamples; s++) {
            float val = amplitude * std::sin(2.0 * M_PI * frekuensi * s / sampleRate);
            for (int ch = 0; ch < numChannels; ch++)
                channels[ch][s] = val;
        }
    }

    // Peak level
    float getPeak() const {
        float peak = 0.0f;
        for (const auto& ch : channels)
            for (float s : ch)
                peak = std::max(peak, std::abs(s));
        return peak;
    }

    // RMS level
    float getRMS(int channelIndex = 0) const {
        const auto& ch = channels[channelIndex];
        float sum = 0.0f;
        for (float s : ch) sum += s * s;
        return std::sqrt(sum / numSamples);
    }
};

// ====================================================
// Efek 1: Gain
// ====================================================
class EfekGain {
public:
    void setGainDB(float dB) {
        gainLinear = std::pow(10.0f, dB / 20.0f);
    }

    void setMute(bool m) { mute = m; }

    void process(AudioBuffer& buffer) {
        if (mute) { buffer.clear(); return; }
        for (int ch = 0; ch < buffer.numChannels; ch++) {
            auto* data = buffer.getWritePointer(ch);
            for (int s = 0; s < buffer.numSamples; s++)
                data[s] *= gainLinear;
        }
    }

    std::string nama() const { return "Gain"; }

private:
    float gainLinear = 1.0f;
    bool  mute = false;
};

// ====================================================
// Efek 2: Hard Clipping Distorsi
// ====================================================
class EfekDistorsi {
public:
    void setThreshold(float t) { threshold = std::clamp(t, 0.01f, 1.0f); }
    void setDrive(float d) { drive = std::clamp(d, 1.0f, 50.0f); }

    void process(AudioBuffer& buffer) {
        for (int ch = 0; ch < buffer.numChannels; ch++) {
            auto* data = buffer.getWritePointer(ch);
            for (int s = 0; s < buffer.numSamples; s++) {
                float sample = data[s] * drive;
                // Hard clip
                sample = std::clamp(sample, -threshold, threshold);
                // Normalize kembali
                data[s] = sample / threshold;
            }
        }
    }

    std::string nama() const { return "Distorsi"; }

private:
    float threshold = 0.5f;
    float drive     = 5.0f;
};

// ====================================================
// Efek 3: Simple Delay
// ====================================================
class EfekDelay {
public:
    void prepare(double sampleRate, float delayMs, float feedbackAmount) {
        int delaySamples = (int)(delayMs * 0.001 * sampleRate);
        delayBuffer.assign(2, std::vector<float>(delaySamples, 0.0f));
        feedback = feedbackAmount;
    }

    void setWetDry(float w) { wet = w; }

    void process(AudioBuffer& buffer) {
        int delayLen = (int)delayBuffer[0].size();
        if (delayLen == 0) return;

        for (int ch = 0; ch < buffer.numChannels; ch++) {
            auto* data = buffer.getWritePointer(ch);
            auto& dBuf = delayBuffer[ch % delayBuffer.size()];

            for (int s = 0; s < buffer.numSamples; s++) {
                float delayed = dBuf[writePos % delayLen];
                dBuf[writePos % delayLen] = data[s] + delayed * feedback;
                writePos = (writePos + 1) % delayLen;
                data[s] = data[s] * (1.0f - wet) + delayed * wet;
            }
        }
    }

    std::string nama() const { return "Delay"; }

private:
    std::vector<std::vector<float>> delayBuffer;
    int   writePos = 0;
    float feedback = 0.4f;
    float wet      = 0.5f;
};

// ====================================================
// Efek 4: Simple Reverb (Schroeder)
// ====================================================
class EfekReverb {
public:
    void prepare(double sampleRate) {
        sr = (float)sampleRate;
        // 4 comb filter + 2 all-pass
        const float combTimes[] = {0.0297f, 0.0371f, 0.0411f, 0.0437f};
        const float apTimes[]   = {0.005f,  0.0017f};

        for (int i = 0; i < 4; i++) {
            int size = (int)(combTimes[i] * sr);
            combBuffers[i].assign(size, 0.0f);
            combPos[i] = 0;
        }
        for (int i = 0; i < 2; i++) {
            int size = (int)(apTimes[i] * sr);
            apBuffers[i].assign(size, 0.0f);
            apPos[i] = 0;
        }
    }

    void setRoomSize(float size) { roomSize = std::clamp(size, 0.0f, 1.0f); }
    void setWet(float w) { wet = w; }

    void process(AudioBuffer& buffer) {
        for (int s = 0; s < buffer.numSamples; s++) {
            float input = 0.0f;
            for (int ch = 0; ch < buffer.numChannels; ch++)
                input += buffer.getReadPointer(ch)[s];
            input /= buffer.numChannels;

            // Comb filters paralel
            float combOut = 0.0f;
            for (int i = 0; i < 4; i++) {
                int  sz  = (int)combBuffers[i].size();
                float yn = combBuffers[i][combPos[i]];
                combBuffers[i][combPos[i]] = input + yn * (0.5f + roomSize * 0.4f);
                combPos[i] = (combPos[i] + 1) % sz;
                combOut += yn;
            }
            combOut *= 0.25f;

            // All-pass filters seri
            float apOut = combOut;
            for (int i = 0; i < 2; i++) {
                int   sz  = (int)apBuffers[i].size();
                float buf = apBuffers[i][apPos[i]];
                apBuffers[i][apPos[i]] = apOut + buf * 0.5f;
                apPos[i] = (apPos[i] + 1) % sz;
                apOut = buf - apOut * 0.5f;
            }

            // Mix wet/dry
            for (int ch = 0; ch < buffer.numChannels; ch++) {
                auto* data = buffer.getWritePointer(ch);
                data[s] = data[s] * (1.0f - wet) + apOut * wet;
            }
        }
    }

    std::string nama() const { return "Reverb"; }

private:
    float sr = 44100.0f, roomSize = 0.5f, wet = 0.3f;
    std::vector<float> combBuffers[4], apBuffers[2];
    int combPos[4] = {}, apPos[2] = {};
};

// ====================================================
// Visualisasi Waveform ASCII
// ====================================================
void visualizeBuffer(const AudioBuffer& buf, const std::string& judul,
                     int channel = 0, int width = 60, int height = 9) {
    const auto* data = buf.getReadPointer(channel);
    int step = std::max(1, buf.numSamples / width);

    std::cout << "\n  [" << judul << "] Peak=" << std::fixed
              << std::setprecision(3) << buf.getPeak()
              << " RMS=" << buf.getRMS(channel) << "\n";

    for (int row = 0; row < height; row++) {
        float threshold = 1.0f - (float)(row * 2) / (height - 1);
        std::cout << "  |";
        for (int x = 0; x < width; x++) {
            int idx = std::min(x * step, buf.numSamples - 1);
            float s = data[idx];
            bool hit = (threshold >= 0) ? (s >= threshold - 0.12f && s < threshold + 0.12f)
                                        : (s >= threshold - 0.12f && s < threshold + 0.12f);
            std::cout << (hit ? '*' : (std::abs(threshold) < 0.05f ? '-' : ' '));
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(width, '-') << "+\n";
}

// ====================================================
// Main: Test semua efek
// ====================================================
int main() {
    std::cout << "============================================\n";
    std::cout << " Simulasi Audio Effect Plugin (JUCE Style)\n";
    std::cout << "============================================\n";

    const double SR = 44100.0;
    const int    N  = 512;

    // --- Test Gain ---
    {
        AudioBuffer buf(2, N);
        buf.fillSine(440.0, SR, 0.8f);
        std::cout << "\n1. Efek GAIN";
        visualizeBuffer(buf, "Input (0.8 amplitude)");

        EfekGain gain;
        gain.setGainDB(-6.0f);  // -6 dB = setengah amplitude
        gain.process(buf);
        visualizeBuffer(buf, "Setelah Gain -6 dB");

        AudioBuffer buf2(2, N);
        buf2.fillSine(440.0, SR, 0.8f);
        gain.setGainDB(6.0f);
        gain.process(buf2);
        visualizeBuffer(buf2, "Setelah Gain +6 dB (clipping)");
    }

    // --- Test Distorsi ---
    {
        AudioBuffer buf(2, N);
        buf.fillSine(440.0, SR, 0.5f);
        std::cout << "\n2. Efek DISTORSI";
        visualizeBuffer(buf, "Input");

        EfekDistorsi dist;
        dist.setThreshold(0.4f);
        dist.setDrive(8.0f);
        dist.process(buf);
        visualizeBuffer(buf, "Setelah Distorsi (threshold=0.4, drive=8)");
    }

    // --- Test Delay ---
    {
        // Buat sinyal impuls (satu sampel 1.0, sisanya 0)
        AudioBuffer buf(2, N * 4);
        buf.getWritePointer(0)[0] = 1.0f;
        buf.getWritePointer(1)[0] = 1.0f;
        std::cout << "\n3. Efek DELAY (impulse response)";

        EfekDelay delay;
        delay.prepare(SR, 50.0f, 0.5f);  // 50ms, 50% feedback
        delay.setWetDry(1.0f);
        delay.process(buf);

        // Visualisasi: cari sampel non-zero
        std::cout << "\n  Gema yang terdengar:\n";
        int gemaCount = 0;
        for (int s = 0; s < buf.numSamples; s++) {
            float v = std::abs(buf.getReadPointer(0)[s]);
            if (v > 0.01f) {
                std::cout << "  t=" << std::setw(5) << s << " ("
                          << std::fixed << std::setprecision(1)
                          << (s * 1000.0 / SR) << " ms): ";
                int bars = (int)(v * 30);
                std::cout << std::string(bars, '|') << " " << v << "\n";
                if (++gemaCount >= 8) break;
            }
        }
    }

    // --- Test Reverb ---
    {
        AudioBuffer buf(2, N * 8);
        // Sinyal test: 1/4 pertama berisi sine, sisanya kosong
        for (int s = 0; s < N * 2; s++) {
            float v = 0.5f * std::sin(2 * M_PI * 440.0 * s / SR);
            buf.getWritePointer(0)[s] = v;
            buf.getWritePointer(1)[s] = v;
        }

        std::cout << "\n4. Efek REVERB";
        float peakBefore = buf.getPeak();

        EfekReverb reverb;
        reverb.prepare(SR);
        reverb.setRoomSize(0.8f);
        reverb.setWet(0.6f);
        reverb.process(buf);

        std::cout << "\n  Peak sebelum : " << peakBefore << "\n";
        std::cout << "  Peak sesudah : " << buf.getPeak() << "\n";
        std::cout << "  (reverb menambah ekor suara setelah sinyal berhenti)\n";

        // Tampilkan decay
        std::cout << "\n  Decay reverb (setelah sampel " << N*2 << "):\n";
        float maxAfter = 0.0f;
        for (int s = N*2; s < buf.numSamples; s++)
            maxAfter = std::max(maxAfter, std::abs(buf.getReadPointer(0)[s]));
        std::cout << "  Level maksimum setelah sinyal: " << maxAfter << "\n";
    }

    // --- Rangkuman ---
    std::cout << "\n============================================\n";
    std::cout << " Rangkuman Plugin Effect Parameters:\n";
    std::cout << "============================================\n";
    std::cout << " Gain    : gainLinear = 10^(dB/20)\n";
    std::cout << " Distorsi: hard clip pada threshold\n";
    std::cout << " Delay   : circular buffer, feedback loop\n";
    std::cout << " Reverb  : comb + all-pass filters (Schroeder)\n\n";

    return 0;
}
