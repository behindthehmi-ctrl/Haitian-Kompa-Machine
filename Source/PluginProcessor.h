#pragma once
#include <JuceHeader.h>

class HaitianKompaProcessor : public juce::AudioProcessor
{
public:
    HaitianKompaProcessor();
    ~HaitianKompaProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Haitian Kompa Machine"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Island Groove"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
private:
    struct Voice { double phase = 0.0; float env = 0.0f; float freq = 440.0f; bool active = false; bool releasing = false; int note = -1; int age = 0; };
    static constexpr int voiceCount = 24;
    Voice voices[voiceCount];
    double sr = 44100.0;
    juce::Random rng;
    float noiseState = 0.0f;
    juce::Reverb reverbEngine;
    juce::Reverb::Parameters reverbParameters;
    std::vector<float> delayL, delayR;
    int delayPosition = 0;
    void handleMidi(const juce::MidiMessage&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HaitianKompaProcessor)
};
